/*
 * Arduino qui récupère les valeurs EDF toutes les X secondes
 * et qui les renvoies à l'aide du module nRF24L01+.
 */

#include <SPI.h>
#include "RF24.h"

/****************   Config    *****************/
/* RF24 Configuration : */
/* CE : Pin 7 */
/* CSW : Pin 8 */
RF24 radio(7,8);

/* Adresses (T = transmiter, R = receiver) */
byte addresses[][8] = {"T-duino", "R-duino"};

bool change_mode = false;

void setup() {
  /* Configuration Serial */
  Serial.begin(115200);
  Serial.println("Lancement du EDF-duino récepteur !");

  /* Lancement radio */
  radio.begin();

  /* A CHANGER : change la distance d'envoie avec d'autres paramètres */
  /* https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo : CTRL-f "Range" */
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);

  radio.startListening();
}

void loop() {
  char r[14];
  
   if(radio.available()){
      while(radio.available()){
        radio.read(&r, sizeof(char[14]));
      }

      Serial.println("Lecture d'un nouveau paquet !");
      Serial.println(r);
      radio.stopListening();
      if(change_mode){
        int new_mode = random(0, 3);
        radio.write(&new_mode, sizeof(int));
      }
      change_mode = !change_mode;
      radio.startListening();
   }
}
