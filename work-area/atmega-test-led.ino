#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

/* Définition des Pins CE, CSN et connexion de la LED sur ARDUINO */
#define RF_CE    9
#define RF_CSN   10
#define LED_PIN  4

RF24 radio(RF_CE, RF_CSN);

/* Définition de l'id de l'Arduino */
byte myID = 0x01;

byte pipes[][7] = {"master", "slave", "idle"};

struct payload_request_t
{
  uint8_t number;
  uint8_t destination;
  char message[14];
};

struct payload_general_t
{
  uint8_t number;
  char message[15];
};

payload_request_t incoming;
payload_general_t outgoing;


/* Envoi de la réponse */
/* ******************* */
void sendReply()
{
  /* Message visible si la console est ouverte */
  Serial.print("Message de retour de la commande \"");
  Serial.print(outgoing.message);
  Serial.println("\"");

  /* Retour de la reponse au demandeur */
  radio.stopListening();
  radio.openWritingPipe(pipes[1]);
  delay(10);
  radio.write(&outgoing, sizeof(payload_general_t) + 1);
  delay(10);
  radio.startListening();
}

/* Par défaut, la LED est enregistrée à l'état "off" */
byte last = 0;

/* Initialisation du programme */
/* *************************** */
void setup()
{
  Serial.begin(9600);
  /* Définition de pin de la LED comme SORTIE */
  pinMode(LED_PIN, OUTPUT);
  /* On met le pin à l'état éteind */
  digitalWrite(LED_PIN, LOW);

  printf_begin();
  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(1, 3);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1, pipes[0]);
  radio.startListening();
  radio.printDetails();
}

/* Boucle de "travail" */
/* ******************* */
void loop(void)
{
  if (radio.available()) {
    radio.read(&incoming, sizeof(payload_request_t));
    /* Affichage de la commande demandée dans la console */
    Serial.print("Commande recu \"");
    Serial.println(incoming.message);

    /* Aduino vérifie que c'est bien à son ID que la demande s'adresse */
    if (incoming.destination == myID) {

      /* Si le message est "get", on retourne l'état enregistré de la LED */
      if (strncmp(incoming.message, "get", 3) == 0) {
        if (last > 0)
          strcpy(outgoing.message, "on");
        else
          strcpy(outgoing.message, "off");

        /* Sinon, la commande est t'elle de type "set" */
      } else if (strncmp(incoming.message, "set", 3) == 0) {
        if (strncmp(&incoming.message[4], "on", 2) == 0) {
          /* Si la commande est "on", on allume la LED */
          digitalWrite(LED_PIN, HIGH);
          last = 1;
          strcpy(outgoing.message, "ok");
        } else if (strncmp(&incoming.message[4], "off", 3) == 0) {
          /* Si la commande est "off", on éteind la LED */
          digitalWrite(LED_PIN, LOW);
          last = 0;
          strcpy(outgoing.message, "ok");
        } else
          /* Sinon, Commande inconnue, ni"on", ni "off" */
          strcpy(outgoing.message, "?");

        /* Sinon, Commande inconnue, ni "get", ni "set" */
      } else {
        strcpy(outgoing.message, "?");
      }

      /* On retourne le résultat du traitement */
      outgoing.number = incoming.number;
      sendReply();
    }
  }
  delay(100);
}

