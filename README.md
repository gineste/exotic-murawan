# MURAWAN

## MVP1
* Verification de la présence d'une configuration (devEUI != 0). Si la config n'est pas faire l'objet se met dans un état d'attente infini. Il sera rebooté apres configuration.
* La solution lit les capteurs selon un intervalle de temps configurable.
* Se connecte au reseau LoRa en détectant les déconnection par des ack émis régulièrement et paramétrable. Après un nombre donné de Ack manqué consecutifs la deconnection et identifiée et un processus de reconnection est lancé avec une fréquence paramétrable.
* Une fois connectée, les données sont emises sur le réseau au format cayenne.

## MVP2 - NFC

* La mémoire NFC est accessible par la radio de facon standard jusqu'a 1KB (0xFF mots de 4 octets). 
* 2 zones sont défnie, chacune de 512 octets soit 64 blocs de 4 octets.
* Chaque zone contient un buffer circulaire.
* La zone 0 contient les dernières données captées. 
* La zone 1 contient les dernières erreurs/log enregistrées.

* La zone 0 est decoupées en entrées de 3 blocs (12 octets)
  Le premier bloc est une entete dont le format est le suivant:
```C
typedef struct {
  uint32_t	magic;				// Just to see if already setup.
  uint8_t	entriesStored;		// Number of entry in the nfcStore
  uint8_t 	writePointer;		// Next Entry ID pointer
  uint8_t	storeSize;			// size of the store in entry blocks
  uint8_t	entrySize;			// size of an entry in NFC block (32b)
} nfcStore_header_t;  
```
- Le magic est 0x4449534B
- Les entrées sont ensuite ajoutées les une à la suite des autres, occupant chacune 3 blocs. 

Les données en zone 0 ont la structure suivante:
```C
typedef struct {
  int16_t	temp;		// 1 block		- in c°C
  uint16_t	pressure;	//				- in Pa
  uint16_t	light;		// 2 block 		- in Lux
  uint16_t	humidity;	//				- in %age
  uint8_t	vcell1;		// 3 block		- in 0.1V
  uint8_t	vcell2;		//				- in 0.1V
  uint8_t	vcell3;		//				- in 0.1V
  uint8_t	vbat;		//				- in 0.1V
} nfcStore_data_entry_t;
```

* La lecture des données se fait de la façon suivante:
1. Lire l'entete de zone pour connaitre sa structure ! taille d'une entry, nombre d'entry possible.
2. Partir du pointeur WritePointer, retirer entrySize pour trouver l'adresse de la dernière valeur insérer. ( writePointer+1-1)*entrySize
3. Itérer entriesStored fois en decrementant l'adresse de entrySize. Lorsque l'adresse est < 1, il faut revenir au dernier bloc dont l'adresse est
(storeSize+1)*entrySize.



## MVP3
* Capture des informations de niveau de batterie. La periode est donnée par le parametre batDuty par pas de 10 minutes.
* Selon que l'alim est suffisante ou non seule le vbat est remonté ou l'ensemble des information des cellules sont remontés.
