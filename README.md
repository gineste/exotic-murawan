# MURAWAN

## MVP1
* Verification de la pr�sence d'une configuration (devEUI != 0). Si la config n'est pas faire l'objet se met dans un �tat d'attente infini. Il sera reboot� apres configuration.
* La solution lit les capteurs selon un intervalle de temps configurable.
* Se connecte au reseau LoRa en d�tectant les d�connection par des ack �mis r�guli�rement et param�trable. Apr�s un nombre donn� de Ack manqu� consecutifs la deconnection et identifi�e et un processus de reconnection est lanc� avec une fr�quence param�trable.
* Une fois connect�e, les donn�es sont emises sur le r�seau au format cayenne.

## MVP2 - NFC

Dans la version MVP#2, la memoire NFC utilise seulement 256 octet qui est la limite accessible par le premier lecteur � ma disposition.
Cette zone est d�compos�e en 2 sous zones:
### La zone 1 contient un historique des donn�es capt�es
* Un header situ� sur les 3 premier blocs contient les information d'acc�s au buffer circulaire h�berg� sur les blocs suivants.
```C
typedef struct {
   uint32_t magic; // Just to see if already setup.
   uint8_t entriesStored; // Number of entry in the nfcStore
   uint8_t writePointer;
   uint8_t storeSize; // size of the store in entry blocks
   uint8_t entrySize; // size of an entry in NFC block (32b)
} nfcStore_header_t;
```
* Les blocs suivants sont compos�s des donn�es historique lues. Il est possible de remonter l'historique en utilisant comme bloc de base le *writePointer* puis de remonter dans le temps *entriesStored* fois. Arriv� a d�but de la zone il faudra revenir � la fin en utilisant *storeSize*. Chaque d�placement dans le buffer se fait en utilisant des sauts de *entrySize* blocks. Les *entrySize* premiers blocks de la zone �tant le header il faudra prendre en compte cet offset. 
* La structure des blocs de donn�es est la suivante:
```C
typedef struct {
   int16_t temp; // 1 block - in c�C
   uint16_t pressure; // - in hPa
   uint16_t light; // 2 block - in Lux
   uint16_t humidity; // - in %age
   uint8_t vcell1; // 3 block - in 0.1V
   uint8_t vcell2; // - in 0.1V
   uint8_t vcell3; // - in 0.1V
   uint8_t vbat; // - in 0.1V
} nfcStore_data_entry_t;
``` 
Cette premi�re zone fait 168 octets et permet d'acc�der � 13 mesures d'historique des donn�es.

* La lecture des donn�es se fait de la fa�on suivante:
1. Lire l'entete de zone pour connaitre sa structure ! taille d'une entry, nombre d'entry possible.
2. Partir du pointeur WritePointer, retirer entrySize pour trouver l'adresse de la derni�re valeur ins�rer. ( writePointer+1-1)*entrySize
3. It�rer entriesStored fois en decrementant l'adresse de entrySize. Lorsque l'adresse est < 1, il faut revenir au dernier bloc dont l'adresse est
(storeSize+1)*entrySize.

Un exemple de client est disponible dans le github murawan.

### La zone 2 permet une communication s�rie avec le device 

Ell utilise la m�moire et donc un protocole de synchronisation avec l'h�te bas� sur un header. Ce header occupe 1 bloc (32b) dans la NFC
```C
typedef struct { // Size %4 must be == 0
   uint16_t magic;
   uint8_t hostRfu:4;
   uint8_t hostIsWriting:1;
   uint8_t hostWriteDone:1;
   uint8_t hostReadDone:1;
  
   uint8_t mcuRfu:4;
   uint8_t mcuIsWriting:1;
   uint8_t mcuWriteDone:1;
   uint8_t mcuReadDone:1;
} drivers_st25dv_serial_header_t;
```
Le principe est le suivant: 
* Si le host (RF) veut �crire il positionne le flag *hostIsWriting* puis *hostWriteDone* 
* Le mcu va alors lire le message et placer *mcuReadDone* a 1 tout en positionnant *hostWriteDone* a zero.
* Si le mcu (I2C) veut �crire il positionne le flag *mcuIsWriting* puis *mcuWriteDone*
* Le host va alors lire le message et placer *hostReadDone* et unsetter le flag *mcuWriteDone*

Les donn�es sont toujours �crites dans les blocs suivants le bloc d'ent�te. Toute communication d�marre sur le premier des blocs. La taille n'est pas transmise car les communications sont pr�vue pour �tre de type String => la fin est indiqu�e par un **0**.

La taille du buffer est de 84 octets. Le total de la zone 2 est de 88 octets avec le header. 

Sur cette communication s�rie passe les commande de la console du firmware. Il est possible de l'utiliser pour configurer un device, acc�der aux logs historiques, status, config... Pour l'utiliser, le point d'entr�e est la commande **?** qui affiche l'aide. La console poss�de un mode public et un mode prot�g� par un mot de passe permettant d'acc�der � la configuration par exemple. Le mot de passe par d�faut et **changeme** il doit �tre tap� comme une commande pour se logger, la r�ponse est OK. 

La documentation de la console est accessible dans les docs du SDK et via l'aide int�gr�e **?**

Un exemple de client est disponible dans le github murawan.


## MVP3
* Capture des informations de niveau de batterie. La periode est donn�e par le parametre batDuty par pas de 10 minutes.
* Selon que l'alim est suffisante ou non seule le vbat est remont� ou l'ensemble des information des cellules sont remont�s.
