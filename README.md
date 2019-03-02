# MURAWAN

## MVP1
* Verification de la pr�sence d'une configuration (devEUI != 0). Si la config n'est pas faire l'objet se met dans un �tat d'attente infini. Il sera reboot� apres configuration.
* La solution lit les capteurs selon un intervalle de temps configurable.
* Se connecte au reseau LoRa en d�tectant les d�connection par des ack �mis r�guli�rement et param�trable. Apr�s un nombre donn� de Ack manqu� consecutifs la deconnection et identifi�e et un processus de reconnection est lanc� avec une fr�quence param�trable.
* Une fois connect�e, les donn�es sont emises sur le r�seau au format cayenne.

## MVP2 - NFC

* La m�moire NFC est accessible par la radio de facon standard jusqu'a 1KB (0xFF mots de 4 octets). 
* 2 zones sont d�fnie, chacune de 512 octets soit 64 blocs de 4 octets.
* Chaque zone contient un buffer circulaire.
* La zone 0 contient les derni�res donn�es capt�es. 
* La zone 1 contient les derni�res erreurs/log enregistr�es.

* La zone 0 est decoup�es en entr�es de 3 blocs (12 octets)
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
- Les entr�es sont ensuite ajout�es les une � la suite des autres, occupant chacune 3 blocs. 

Les donn�es en zone 0 ont la structure suivante:
```C
typedef struct {
  int16_t	temp;		// 1 block		- in c�C
  uint16_t	pressure;	//				- in Pa
  uint16_t	light;		// 2 block 		- in Lux
  uint16_t	humidity;	//				- in %age
  uint8_t	vcell1;		// 3 block		- in 0.1V
  uint8_t	vcell2;		//				- in 0.1V
  uint8_t	vcell3;		//				- in 0.1V
  uint8_t	vbat;		//				- in 0.1V
} nfcStore_data_entry_t;
```

* La lecture des donn�es se fait de la fa�on suivante:
1. Lire l'entete de zone pour connaitre sa structure ! taille d'une entry, nombre d'entry possible.
2. Partir du pointeur WritePointer, retirer entrySize pour trouver l'adresse de la derni�re valeur ins�rer. ( writePointer+1-1)*entrySize
3. It�rer entriesStored fois en decrementant l'adresse de entrySize. Lorsque l'adresse est < 1, il faut revenir au dernier bloc dont l'adresse est
(storeSize+1)*entrySize.



## MVP3
* Capture des informations de niveau de batterie. La periode est donn�e par le parametre batDuty par pas de 10 minutes.
* Selon que l'alim est suffisante ou non seule le vbat est remont� ou l'ensemble des information des cellules sont remont�s.
