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
#include <Time.h>

#define START_FRAME   0x02 // 0x02 STX
#define END_FRAME     0x03 // 0x03 END
#define LINE_START    0x0A // 0x0A \n
#define LINE_END      0x0D // 0x0D \r
#define LINE_LENGTH   30   //      taille de ligne maximum
#define VALUES_LENGTH 3    //      taille de tableau à enregitrer avant envoie
#define MODE_TEST     0    //      désactivé pour la "release" (gagne de la mémoire)

struct valuesWeCareAbout {
  byte ID;
  long HC;
  long HP;
  int PAPP;
  boolean IS_HP;
  unsigned long timestamp;
};

/****************   Config    *****************/
/* RF24 Configuration : */
/* CE : Pin 7 */
/* CSW : Pin 8 */
RF24 radio(7,8); 

/* On crée deux instances de SoftwareSerial */
SoftwareSerial* cptSerial;
SoftwareSerial* cptSerialFake;

/* Adresses (T = transmiter, R = receiver) */
byte addresses[][8] = {"T-duino", "R-duino"};

/* Tableau de stockage */
struct valuesWeCareAbout b[VALUES_LENGTH];
int currentIndex;

/* Variable intermédiaire */
struct valuesWeCareAbout frame;

/* Séparateur des lignes de la trame */
char sep = ' ';

int duree = 0;

/* FOR TEST */
#if MODE_TEST
int  __charIndex = 0;
char __simulatedString[] = ":\nADCO 030622716812 =\r\nOPTARIF HC.. <\r\nISOUSC 30 9\r\nHCHC 035318160 !\r\nHCHP 039061379 9\r\nPTEC HP..  \r\nIINST 001 X\r\nIMAX 029 J\r\nPAPP 00180 *\r\nHHPHC C .\r\nMOTDETAT 000000 B\r?";
char getCharSimulated(){
  char c = __simulatedString[__charIndex];
  __charIndex = (__charIndex + 1) % 170;
  return c;
}
#endif

/* Retourne le prochain caractère en entrée */
char getChar() {
  /*return getCharSimulated();*/
  while(!cptSerial->available()) {}
  return cptSerial->read() & 0x7F;
}
/* Reset le tableau de relevé de valeurs */
void resetTable()
{
#if MODE_TEST
  Serial.println("Reset table");
  Serial.flush();
#endif

 for(int i = 0 ; i < VALUES_LENGTH ; i++)
 {
  b[i].ID = -1;
  b[i].HC = -1;
  b[i].HP = -1;
  b[i].PAPP = -1;
  b[i].IS_HP = false;
  b[i].timestamp = -1;
 }

 currentIndex = 0;
}

/* Vérifie si on est au dernier index. */
boolean isLastIndex() {
  return currentIndex == VALUES_LENGTH;
}

/* Initialise la variable "frame". */
void initFrame() {
  frame.ID = -1;
  frame.HC = -1;
  frame.HP = -1;
  frame.PAPP = -1;
  frame.IS_HP = false;
  frame.timestamp = -1;
}

/* Sauvegarde la variable "frame" 
 * dans le tableau.
 */
void saveFrame() {
#if MODE_TEST
  Serial.print("Save frame : ");
  Serial.print(frame.ID);
  Serial.print("-");
  Serial.print(frame.HC);
  Serial.print("-");
  Serial.print(frame.HP);
  Serial.print("-");
  Serial.print(frame.PAPP);
  Serial.print("-");
  Serial.println(frame.timestamp);
  Serial.flush();
#endif
  b[currentIndex] = frame;
  currentIndex++;
}

/* Vérifie que la variable "frame"
 * est bien initialisée.
 */
boolean checkFrame() {
#if MODE_TEST
  Serial.println("Check frame");
  Serial.println(frame.HC);
  Serial.flush();
#endif
  return frame.ID >= 0 &&
         frame.HC >= 0 &&
         frame.HP >= 0 &&
         frame.timestamp > 0;
}

/* Lit une nouvelle trame et "feed"
 * la variable "frame".
 */
void readFrame() {
#if MODE_TEST
  Serial.println("Read frame");
  Serial.flush();
#endif

  char currentChar = 0;

  /* Lecture des caractères jusqu'au début
     de la trame */
  while (currentChar != START_FRAME)
   currentChar = getChar();

#if MODE_TEST
  Serial.println("Start frame found");
  Serial.flush();
#endif

  /* Boucle principale */
  int index = 0;
  String line;

  while (currentChar != END_FRAME)
  {
    /* Lecture d'un caractère */
    currentChar = getChar();

    /* Soit c'est le début d'une ligne */
    if (currentChar == LINE_START)
    {
      line = "";
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
    }
  }

  frame.timestamp = now();
  frame.ID = currentIndex;

#if MODE_TEST
  Serial.println("Frame has been read");
  Serial.flush();
#endif
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
  /* Tente 5s d'envoyer les données
   * sinon on laisse tomber et on passe à la suite
   */
   radio.stopListening();
#if MODE_TEST
   Serial.println("Send table...");
   Serial.flush();
#endif

   unsigned long start_millis = millis();
   bool fail = true;

   /* Tant qu'on a pas dépassé 5s, et que
    * ça n'a pas marché...
    */
   while(millis() - start_millis < 5000 && fail)
   {
    Serial.println("TENTATIVE..");
    for(int i = 0 ; i < VALUES_LENGTH ; i++)
    {
      if(!radio.write(&b[i], sizeof(b[i])))
        break;
    }
    fail = false;
    Serial.println("ENVOIE OK");
   }
#if MODE_TEST
   Serial.print("last envoie = ");
   Serial.print(now());
   setTime(0);
   Serial.flush();
#endif
}


void setup() {
  /* Configuration Serial */
  Serial.begin(9600);
  Serial.println("Lancement du EDF-duino transmetteur !");

  /* Configuration Serial for EDF */
  cptSerialFake = new SoftwareSerial(0,1);
  cptSerialFake->begin(1200);
  cptSerial = new SoftwareSerial(2,3);
  cptSerial->begin(1200);

  /* Lancement radio */
  radio.begin();

  /* A CHANGER : change la distance d'envoie avec d'autres paramètres */
  /* https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo : CTRL-f "Range" */
  radio.setPayloadSize(32);
  radio.setChannel(108);
  radio.setAutoAck(true);
  radio.setRetries(15, 15);
  radio.setDataRate(RF24_2MBPS);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);

  initFrame();
  resetTable();
  radio.stopListening();
}

void loop() {

/* * Lire nouvelle entrée
 * * Check nouvelle entrée. Si pas bonne, reprendre.
     * Si dernier index : lire entrée & envoyer tab & reset tab 
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
  radio.powerUp();
  sendTable();
  resetTable();
 }

#if MODE_TEST
 Serial.print("Bouh");
 Serial.print(" ");
 Serial.print(now() - duree);
 Serial.print(" ");
 Serial.flush();
#endif
 cptSerialFake->listen();
 radio.powerDown();
 LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
 adjustTime(8);
 duree = now();
 cptSerial->listen();
#if MODE_TEST
 Serial.println(" - hehe");
 Serial.flush();
#endif
}
