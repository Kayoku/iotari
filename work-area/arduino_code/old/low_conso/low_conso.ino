#include "LowPower.h"
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "temperature_ds18b20.h"

/* Définition des Pins CE, CSN et connexion de la LED sur ARDUINO */
#define RF_CE    9
#define RF_CSN   10

#define SENSOR_PIN 5

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

struct sensor_float_t
{
  char type[15];
  float value;
};

payload_request_t incoming;
sensor_float_t outgoing;

// sleep stuff
int nb_wake_up_since_last_send = 0;
int nb_sleeps_to_perform = 2;

TemperatureSensor sensor(SENSOR_PIN);

void setup()
{
  Serial.begin(9600);
  
  strcpy(outgoing.type, "temperature");
  outgoing.value = 0;

  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(1, 3);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1, pipes[0]);
  radio.printDetails();
}

void read_sensor(sensor_float_t &sensor_struct){
  // domain specific stuff...
  Serial.println("Reading sensor value...");
  sensor_struct.value = sensor.getValue();
  Serial.print("... OK"); Serial.println(sensor_struct.value);
}

void send_radio(sensor_float_t &sensor_struct){

  Serial.println("      Beginning sending radio...");
  radio.powerUp();
  radio.openWritingPipe(pipes[1]);
  delay(10);
  radio.write(&sensor_struct, sizeof(sensor_float_t) + 1);
  radio.powerDown();
  Serial.println("     ... End sending radio");

}

void loop()
{
  Serial.println("Before sleeping");
  // Enter power down state for 8 s with ADC and BOD module disabled
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  nb_wake_up_since_last_send++;
  outgoing.value += 0.01;

  Serial.print("Nb wake up since last send : "); Serial.println(nb_wake_up_since_last_send);
  if(nb_wake_up_since_last_send < nb_sleeps_to_perform){
    read_sensor(outgoing);
    send_radio(outgoing);
    nb_wake_up_since_last_send = 0;
  }
}
