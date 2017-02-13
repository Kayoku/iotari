#include <OneWire.h>
#include <DallasTemperature.h>


class TemperatureSensor
{
  public:
    TemperatureSensor(int pin): oneWire(pin){
      this->pin = pin;
      this->sensors = new DallasTemperature(&oneWire);

      this->sensors->begin(); //Activation des capteurs
      this->sensors->getAddress(sensorDeviceAddress, 0); //Demande l'adresse du capteur à l'index 0 du bus

      Serial.println("before setting sensor resolution");
      //this->sensors->setResolution(sensorDeviceAddress, 12); //Résolutions possibles: 9,10,11,12
      Serial.println("after setting sensor resolution");
    }

    inline float getValue(){
      this->sensors->requestTemperatures(); //Demande la température aux capteurs
      return this->sensors->getTempCByIndex(0);
    }

  private:
    int pin;
    OneWire oneWire;
    DallasTemperature* sensors;
    DeviceAddress sensorDeviceAddress; 
};
