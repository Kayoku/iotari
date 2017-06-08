/*
 * Arduino qui récupère les valeurs EDF toutes les X secondes
 * et qui les renvoies à l'aide du module nRF24L01+.
 */

#include <SPI.h>
#include "LowPower.h"
#include "RF24.h"

#define startFrame 0x02
#define endFrame 0x03

struct values_we_care_about_s {
  long HC;
  long HP;
  int IINST;
  boolean HC_OR_HP;
  unsigned long timestamp;
};

/****************   Config    *****************/
/* Détermine le temps d'attente entre chaque envoie (en milliseconde) 
 * Attention ! millisecond_wait_for_order doit être plus petit
 * que millisecond_wait.
 */
int millisecond_wait = 3000;
int millisecond_wait_for_order = 2000;

/* RF24 Configuration : */
/* CE : Pin 7 */
/* CSW : Pin 8 */
RF24 radio(7,8);

/* Adresses (T = transmiter, R = receiver) */
byte addresses[][8] = {"T-duino", "R-duino"};

/* Tableau de stockage */
values_we_care_about_s b[50];

void setup() {
  /* Configuration Serial */
  Serial.begin(9600);
  Serial.println("Lancement du EDF-duino transmetteur !");

  /* Lancement radio */
  radio.begin();

  /* A CHANGER : change la distance d'envoie avec d'autres paramètres */
  /* https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo : CTRL-f "Range" */
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);

 for(int i = 0 ; i < 1000 ; i++)
 {
  b[i].HC = i%250;
  b[i].HP = i%250;
  b[i].IINST = i%250;
  b[i].HC_OR_HP = true;
  b[i].timestamp = 0;
 }
}

void loop() {

  radio.stopListening();
  Serial.println("Envoie d'un paquet EDF");

  /* Variable de stockage des caractères reçus */
  char charIn = 0;

  /* Récupération du paquet EDF (à faire) */
  char string_test[14] = "T'es mauvais.";
  
  /* Envoie du paquet */
  if(!radio.write(&string_test, sizeof(char[14]))) {
    Serial.println("Failed.");
  }

  /* On écoute pendant 2 secondes pour voir si on a de nouveaux ordres. */
  Serial.println("On écoute 2 secondes pour voir si on a de nouveaux ordres.");

  unsigned long started_waiting_at = millis();
  boolean timeout = false;

  radio.startListening();
  while (!radio.available()){
    if(millis() - started_waiting_at > millisecond_wait_for_order){
      timeout = true;
      break;
    }
  }

  if(timeout)
    Serial.println("Pas de nouvel ordre");
  else
  {
    int mode = 0;
    Serial.println("Nouvel ordre !");
    radio.read(&mode, sizeof(int));
    Serial.println(mode);

    switch(mode){
      case 0:
        millisecond_wait = 3000;
        Serial.println("Nouveau temps : 3000");
        break;
      case 1:
        millisecond_wait = 5000;
        Serial.println("Nouveau temps : 5000");
        break;
      case 2:
        millisecond_wait = 10000;
        Serial.println("Nouveau temps : 10000");
        break;
      default:
        millisecond_wait = 1000;
        Serial.println("Nouveau temps : 1000");
    }
  }
  
  /* Délai d'attente*/
  delay(millisecond_wait);
}
