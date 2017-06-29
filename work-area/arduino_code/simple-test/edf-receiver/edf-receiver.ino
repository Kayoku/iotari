/*
 * Arduino qui récupère les valeurs EDF toutes les X secondes
 * et qui les renvoies à l'aide du module nRF24L01+.
 */

#include <SPI.h>
#include "RF24.h"
#include "Time.h"

#define VALUES_LENGTH 3    // taille de tableau à enregitrer avant envoie

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

/* Adresses (T = transmiter, R = receiver) */
byte addresses[][8] = {"T-duino", "R-duino"};

/* Variable intermédiaire, et tab de stockage */
struct valuesWeCareAbout b[VALUES_LENGTH];
struct valuesWeCareAbout frame;

int last_receive = 0;

bool checkB()
{
  for(int i = 0 ; i < VALUES_LENGTH ; i++)
    if(b[i].ID != i)
      return false;
  return true;
}

void clearB()
{
  for(int i = 0 ; i < VALUES_LENGTH ;i++)
  {
    b[i].ID = -1;
    b[i].HC = -1;
    b[i].HP = -1;
    b[i].PAPP = -1;
    b[i].IS_HP = true;
    b[i].timestamp = 0;
  }
}

void displayB()
{
  for(int i = 0 ; i < VALUES_LENGTH ;i++)
  {
    Serial.print("id n°");
    Serial.print(b[i].ID);
    Serial.print(" (");
    if(b[i].IS_HP)
      Serial.println("HP)");
    else
      Serial.println("HC)");
    Serial.print(" HC : ");
    Serial.println(b[i].HC);
    Serial.print(" HP : ");
    Serial.println(b[i].HP);
    Serial.print(" PAPP : ");
    Serial.println(b[i].PAPP);
    Serial.print(" Time : ");
    Serial.println(b[i].timestamp);
    Serial.println("");
  }
}

void setup() {
  /* Configuration Serial */
  Serial.begin(115200);
  Serial.println("Lancement du EDF-duino récepteur !");

  /* Lancement radio */
  radio.begin();

  /* A CHANGER : change la distance d'envoie avec d'autres paramètres */
  /* https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo : CTRL-f "Range" */
  radio.setPayloadSize(32);
  radio.setChannel(108);
  radio.setAutoAck(true);
  radio.setRetries(15, 15);
  radio.setDataRate(RF24_2MBPS);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);

  radio.startListening();
}

void loop() {
  if(radio.available())
  {
    Serial.print("last : ");
    Serial.println(now() - last_receive);
    last_receive = now();
    
    long start_millis = millis();
    for(int i = 0 ; i < VALUES_LENGTH ; i++)
    {
      long begin_millis = millis();
      while(!radio.available() && millis() - begin_millis < 200) {}

      if(millis() - begin_millis > 200)
       break;

      radio.read(&b[i], sizeof(b[i]));

      if(b[i].ID != i)
        i--;
    }

    long millis_total = millis() - start_millis;
    Serial.print("temps de reception : ");
    Serial.println(millis_total);

    if(checkB())
    {
      Serial.println("Tout est ok.");
      displayB();
    }
    else
      Serial.println("Des erreurs...");

    /* DO SOMETHING WITH */
      
    clearB();
  }
}
