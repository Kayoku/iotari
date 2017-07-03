# Court terme

* Mettre au propre le code pour la première version avec capteur EDF.
	* Code récepteur.
	* Code émetteur.
	* Code serveur (réception & serveur).

* Découpage du projet en "module". L'ajout d'un module (par exemple ici, la gestion des capteurs) entraine la création automatique des tables nécessaires en BDD, ainsi que de l'affichage de ce module sur l'interface web.

* Mettre en place une librairie pour gérer plusieurs capteurs. (RF24-RF24Network-RF24Mesh).

# Long terme

* Tutoriel d'installation général du projet.
* Capteur de température
* Gestion des capteurs côté serveur.
	* Quels capteurs sont connectés ?
* Système d'alerte ? (système communiquant sur télégram ?)
	* Quand une valeur est considéré comme étrange (mise en place d'un système complet d'alerte)
	* Quand un capteur est désactivé ? (du coup, enregistrement au préalable des capteurs "fixes" du réseau IoT)
