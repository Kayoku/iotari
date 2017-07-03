# Court terme

* Mettre au propre le code pour la première version avec capteur EDF.
	* Code récepteur.
	* Code émetteur.
	* Code serveur (réception & serveur).

* Découpage du projet en "module". L'ajout d'un module (par exemple ici, la gestion des capteurs) entraine la création automatique des tables nécessaires en BDD, ainsi que de l'affichage de ce module sur l'interface web.
	* Création d'une page d'accueil simple avec liste des modules chargés
	* Créer premier module pour la gestion des capteurs
		* Un module contient de quoi créer la base de donnée
		* Les vues qu'elle permet d'afficher
		* Toute la logique pour manipuler les objets en BDD

* Mettre en place une librairie pour gérer plusieurs capteurs. (RF24-RF24Network-RF24Mesh).

# Long terme

* Tutoriel d'installation général du projet.
* Capteur de température
* Gestion des capteurs côté serveur.
	* Quels capteurs sont connectés ?
* Système d'alerte ? (système communiquant sur télégram ?)
	* Quand une valeur est considéré comme étrange (mise en place d'un système complet d'alerte)
	* Quand un capteur est désactivé ? (du coup, enregistrement au préalable des capteurs "fixes" du réseau IoT)
