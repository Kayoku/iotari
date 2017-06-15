/*
 * Arduino qui récupère les valeurs EDF toutes les X secondes
 * et qui les renvoies à l'aide du module nRF24L01+.
 */

 /**
  * ATTENTION AU MILLIS QUI REVIENNENT A 0
  * CHANGEMENT PIN SERIAL OU RADIO
  */

#include <SPI.h>
#include "LowPower.h"
#include "RF24.h"
#include <SoftwareSerial.h>

#define START_FRAME   0x02 // 0x02 STX
#define END_FRAME     0x03 // 0x03 END
#define LINE_START    0x0A // 0x0A \n
#define LINE_END      0x0D // 0x0D \r
#define LINE_LENGTH   30   //      taille de ligne maximum
#define VALUES_LENGTH 5   //      taille de tableau à enregitrer avant envoie

char sep = ' ';

struct valuesWeCareAbout {
  long HC;
  long HP;
  int IINST;
  int PAPP;
  boolean IS_HP;
  unsigned long timestamp;
};

/****************   Config    *****************/
/* RF24 Configuration : */
/* CE : Pin 7 */
/* CSW : Pin 8 */
RF24 radio(7,6); 

// On crée une instance de SoftwareSerial
SoftwareSerial* cptSerial;

/* Adresses (T = transmiter, R = receiver) */
byte addresses[][8] = {"T-duino", "R-duino"};

/* Tableau de stockage */
struct valuesWeCareAbout b[VALUES_LENGTH];
int currentIndex;

/* Variable intermédiaire */
struct valuesWeCareAbout frame;

/* FOR TEST */
int  __charIndex = 0;
char __simulatedString[] = ":\nADCO 030622716812 =\r\nOPTARIF HC.. <\r\nISOUSC 30 9\r\nHCHC 035318160 !\r\nHCHP 039061379 9\r\nPTEC HP..  \r\nIINST 001 X\r\nIMAX 029 J\r\nPAPP 00180 *\r\nHHPHC C .\r\nMOTDETAT 000000 B\r?";
char getCharSimulated(){
  char c = __simulatedString[__charIndex];
  __charIndex = (__charIndex + 1) % 170;
  return c;
}

/* Retourne le prochain caractère en entrée */
char getChar() {

  //return getCharSimulated();
  while(!cptSerial->available()) {}
  return cptSerial->read() & 0x7F;
}
/* Reset le tableau de relevé de valeurs */
void resetTable()
{
  Serial.println("Reset table");
  Serial.flush();
 for(int i = 0 ; i < VALUES_LENGTH ; i++)
 {
  b[i].HC = 0;
  b[i].HP = 0;
  b[i].IINST = 0;
  b[i].PAPP = 0;
  b[i].IS_HP = false;
  b[i].timestamp = 0;
 }

 currentIndex = 0;
}

/* Vérifie si on est au dernier index. */
boolean isLastIndex() {
  return currentIndex == VALUES_LENGTH;
}

/* Initialise la variable "frame". */
void initFrame() {
  frame.HC = 0;
  frame.HP = 0;
  frame.IINST = 0;
  frame.PAPP = 0;
  frame.IS_HP = false;
  frame.timestamp = 0;
}

/* Sauvegarde la variable "frame" 
 * dans le tableau.
 */
void saveFrame() {
  Serial.print("Save frame : ");
  Serial.print(frame.HC);
  Serial.print("-");
  Serial.print(frame.HP);
  Serial.print("-");
  Serial.print(frame.PAPP);
  Serial.print("-");
  Serial.println(frame.timestamp);
  Serial.flush();
  
  b[currentIndex] = frame;
  currentIndex++;
}

/* Vérifie que la variable "frame"
 * est bien initialisée.
 */
boolean checkFrame() {
  Serial.println("Check frame");
  Serial.println(frame.HC);
  Serial.flush();
  return frame.HC > 0 &&
         frame.HP > 0 &&
         frame.timestamp > 0;
}

/* Lit une nouvelle trame et "feed"
 * la variable "frame".
 */
void readFrame() {
  Serial.println("Read frame");
  Serial.flush();

  char currentChar = 0;

  /* Lecture des caractères jusqu'au début
     de la trame */
  while (currentChar != START_FRAME)
    currentChar = getChar();

  Serial.println("Start frame found");
  Serial.flush();
  
  /* Boucle principale */
  int index = 0;
  //char line[LINE_LENGTH] = {0};
  String line;

  while (currentChar != END_FRAME)
  {
    /* Lecture d'un caractère */
    currentChar = getChar();

    /* Soit c'est le début d'une ligne */
    if (currentChar == LINE_START)
    {
      line = "";
      //memset(line, 0, sizeof(line));
      index = 0;
    }
    /* Soit c'est la fin d'une ligne */
    else if (currentChar == LINE_END)
    {
      int name_index = line.indexOf(sep);
      int value_index = line.indexOf(sep, name_index+1);

      String name_s = line.substring(0, name_index);
      String value = line.substring(name_index + 1, value_index);
     
      if(name_s == "PTEC")
      {
        if(value.equals("HP.."))
          frame.IS_HP = true;
      }
      else if(name_s == "HCHC")
        frame.HC = value.toInt();
      else if(name_s == "HCHP")
        frame.HP = value.toInt();
      else if(name_s == "IINST")
        frame.IINST = value.toInt();
      else if(name_s == "PAPP")
        frame.PAPP = value.toInt();
    }
    /* Soit c'est un caractère à enregistrer */
    else
    {
      if(index > LINE_LENGTH)
      {
        Serial.println("ERREUR TAILLE LIGNE");
        continue;
      }
      line += currentChar;
      //line[index] = currentChar;
      //index++;
    }
  }

  frame.timestamp = millis();

  Serial.println("Frame has been read");
  Serial.flush();
}

/* Permet de vider le buffer du serial
 * pour récupérer EDF
 */
void clearSerial() {
  if(cptSerial->available() > 0)
    cptSerial->read();
}

/* Permet d'envoyer les infos des 
 * 50 derniers relevés.
 */
void sendTable() {
  /* Tente 3 fois d'envoyer les données
   * en espérant un accusé de recep,
   * sinon on laisse tomber et on passe à la suite
   */
   for (int i = 0 ; i < 3 ; i++)
   {
      radio.stopListening();
      Serial.println("Send table...");
      Serial.flush();

      /* Envoie des données */
      radio.write(&b, sizeof(b));

      /* On rallume la radio */
      radio.startListening();

      /* On essaie d'avoir l'ack */
      boolean timeout = false;
      unsigned long ack_time = millis();
            
      /* On voit si on a un message d'ack */
      while(!radio.available())
      {
        /* -5000 Permet de gérer le cas ou millis se reset à 0 */
        if(millis() - ack_time > 200 || millis() - ack_time < -5000)
        {
          timeout = true;
          break;
        }
      }

      /* Si timeout est faux, on a bien envoyé
       * alors on peut arrêter d'envoyer.
       */
      if(!timeout)
      {
        Serial.println("Send success !");
        Serial.flush();
        break;
      }
      Serial.println("Send failed");
      Serial.flush();
   }
}


void setup() {
  /* Configuration Serial */
  Serial.begin(9600);
  Serial.println("Lancement du EDF-duino transmetteur !");

  /* Configuration Serial for EDF */
  cptSerial = new SoftwareSerial(8, 9);
  cptSerial->begin(1200);

  /* Lancement radio */
  radio.begin();

  /* A CHANGER : change la distance d'envoie avec d'autres paramètres */
  /* https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo : CTRL-f "Range" */
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);

  initFrame();
  resetTable();
  radio.stopListening(); 

}

void loop() {

/* * Lire nouvelle entrée
 * * Check nouvelle entrée. Si pas bonne, reprendre.
     * Si index == 48 : lire entrée & envoyer tab 3 fois (jusqu'à accusé) & reset tab 
     * Sinon enregistrer entrée dans tab & incrémenter index
   * Dormir en mode oversleepofthedead
 */
 initFrame();
 clearSerial();
 
 do
 {
  readFrame();
 } while(!checkFrame());
 
 saveFrame();

 if(isLastIndex())
 {
  sendTable();
  resetTable();
 }

 Serial.print("Bouh");
 LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
 Serial.println(" - hehe");
}
