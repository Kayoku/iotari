# Ressources :

 * [Mise en veille de la radio](https://tmrh20.github.io/RF24/pingpair_sleepy_8ino-example.html)


 # Consommation :


## Radio


Radio Modes & Current Consumption by function, when run in this order:

| Function | Radio Mode | Current Consumption |
| -------- | ---------- | ------------------- |
radio.begin(); | Standby-I | .026mA |
radio.startListening(); |  Active | 11-15mA |
delay(1000); | Active | 11-15mA |
radio.stopListening(); | Standby-I | .026mA |
radio.write(); | Active | 11-15mA |
delay(1000);  Standby-I  | .026mA |
radio.writeFast() |  Active |  11-15mA |
delay(1000); |  Standby-II |  0.320mA |
radio.powerDown(); | PowerDown  | .0009mA |


## Arduino

3.3V & 8MHz
 * wake Current 3.87mA
 * sleep current 4.3µA


## Total

 * Arduino allumé et radio qui écoute / envoie : 20mA
 * Arduino en veille et radio en power down : 6µA

## Calcul conso / autonomie

Conso en Ah en fonction du ratio allumé / éteind : `def conso(r): return 6*1e-6*(1-r) + r * 20e-3`
Autonomie en heures : `def autonomie(r, pile=1.2): return pile / conso(r)`

* Si allumé 10s par minute : 144j d'autonomie
* Si allumé 5s par heure : 4ans d'autonomie