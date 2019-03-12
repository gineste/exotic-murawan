# MURAWAN

## MVP1
* Verification de la présence d'une configuration (devEUI != 0). Si la config n'est pas faire l'objet se met dans un état d'attente infini. Il sera rebooté apres configuration.
* La solution lit les capteurs selon un intervalle de temps configurable.
* Se connecte au reseau LoRa en détectant les déconnection par des ack émis régulièrement et paramétrable. Après un nombre donné de Ack manqué consecutifs la deconnection et identifiée et un processus de reconnection est lancé avec une fréquence paramétrable.
* Une fois connectée, les données sont emises sur le réseau au format cayenne.

## MVP2 - NFC

Dans la version MVP#2, la memoire NFC utilise seulement 256 octet qui est la limite accessible par le premier lecteur à ma disposition.
Cette zone est décomposée en 2 sous zones:
### La zone 1 contient un historique des données captées
* Un header situé sur les 3 premier blocs contient les information d'accès au buffer circulaire hébergé sur les blocs suivants.
```C
typedef struct {
   uint32_t magic; // Just to see if already setup.
   uint8_t entriesStored; // Number of entry in the nfcStore
   uint8_t writePointer;
   uint8_t storeSize; // size of the store in entry blocks
   uint8_t entrySize; // size of an entry in NFC block (32b)
} nfcStore_header_t;
```
* Les blocs suivants sont composés des données historique lues. Il est possible de remonter l'historique en utilisant comme bloc de base le *writePointer* puis de remonter dans le temps *entriesStored* fois. Arrivé a début de la zone il faudra revenir à la fin en utilisant *storeSize*. Chaque déplacement dans le buffer se fait en utilisant des sauts de *entrySize* blocks. Les *entrySize* premiers blocks de la zone étant le header il faudra prendre en compte cet offset. 
* La structure des blocs de données est la suivante:
```C
typedef struct {
   int16_t temp; // 1 block - in c°C
   uint16_t pressure; // - in hPa
   uint16_t light; // 2 block - in Lux
   uint16_t humidity; // - in %age
   uint8_t vcell1; // 3 block - in 0.1V
   uint8_t vcell2; // - in 0.1V
   uint8_t vcell3; // - in 0.1V
   uint8_t vbat; // - in 0.1V
} nfcStore_data_entry_t;
``` 
Cette première zone fait 168 octets et permet d'accéder à 13 mesures d'historique des données.

* La lecture des données se fait de la façon suivante:
1. Lire l'entete de zone pour connaitre sa structure ! taille d'une entry, nombre d'entry possible.
2. Partir du pointeur WritePointer, retirer entrySize pour trouver l'adresse de la dernière valeur insérer. ( writePointer+1-1)*entrySize
3. Itérer entriesStored fois en decrementant l'adresse de entrySize. Lorsque l'adresse est < 1, il faut revenir au dernier bloc dont l'adresse est
(storeSize+1)*entrySize.

Un exemple de client est disponible dans le github murawan.

### La zone 2 permet une communication série avec le device 

Ell utilise la mémoire et donc un protocole de synchronisation avec l'hôte basé sur un header. Ce header occupe 1 bloc (32b) dans la NFC
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
* Si le host (RF) veut écrire il positionne le flag *hostIsWriting* puis *hostWriteDone* 
* Le mcu va alors lire le message et placer *mcuReadDone* a 1 tout en positionnant *hostWriteDone* a zero.
* Si le mcu (I2C) veut écrire il positionne le flag *mcuIsWriting* puis *mcuWriteDone*
* Le host va alors lire le message et placer *hostReadDone* et unsetter le flag *mcuWriteDone*

Les données sont toujours écrites dans les blocs suivants le bloc d'entête. Toute communication démarre sur le premier des blocs. La taille n'est pas transmise car les communications sont prévue pour être de type String => la fin est indiquée par un **0**.

La taille du buffer est de 84 octets. Le total de la zone 2 est de 88 octets avec le header. 

Sur cette communication série passe les commande de la console du firmware. Il est possible de l'utiliser pour configurer un device, accéder aux logs historiques, status, config... Pour l'utiliser, le point d'entrée est la commande **?** qui affiche l'aide. La console possède un mode public et un mode protégé par un mot de passe permettant d'accéder à la configuration par exemple. Le mot de passe par défaut et **changeme** il doit être tapé comme une commande pour se logger, la réponse est OK. 

La documentation de la console est accessible dans les docs du SDK et via l'aide intégrée **?**

Un exemple de client est disponible dans le github murawan.


## MVP3
* Capture des informations de niveau de batterie. La periode est donnée par le parametre batDuty par pas de 10 minutes.
* Selon que l'alim est suffisante ou non seule le vbat est remonté ou l'ensemble des information des cellules sont remontés.

* status de consommation:
![alt conso](https://github.com/disk91/exotic-murawan/raw/master/Capture-ecran-2019-03-06-21.13.30.png)



## Format des messages

### Message des Capteurs d'environnement

Port 1 - Format Cayenne
```C
// +------------+---------------+-----------------+-------------+
// | Temp 0.1C  | Humidity 0.5% | Pressure 0.1hPa | Light 1 Lux |
// +------------+---------------+-----------------+-------------+
```

### Message d'état des batteries

Port 2 - Format Cayenne
```C
// +------------+-------------+-------------+-------------+----------+
// | VBat 0.01V | Cell1 0.01V | Cell2 0.01V | Cell3 0.01V | Coulomb  |
// +------------+-------------+-------------+-------------+----------+
```

### Message de boot emis 5m apres un restart

Port 5 - Format Raw
```C 
// +------+------+------+------+------+-----+-----+----------+---------+-----------+---------+
// | 0x01 | Reset Cause | VBat 0.001V | Temp 0.1C | SendDuty | Antenna | SleepDuty | BatDuty |
// +------+------+------+------+------+-----+-----+----------+---------+-----------+---------+
```

* Reset Cause
	0 - Under voltage
	1 - Reset Pin
	2 - Power on
	3 - Soft Reset
	4 - IWDG
	5 - WWDG 
	6 - Low Power

* SendDuty: temps entre deux mesure environmental et communication associée x 10s
* Antenna: 
	0 - PiFa
	1 - PCB
* SleepDuty: temps entre deux tentatives de reconnexion au reseau lorawan exprimé en nombre de SendDuty
* BatDuty: temps entre deux emission de message de status batterie x 10m

### Downlink configuration message

Port 5 - Format Raw
```C
// +------+------------+----------+---------+----------+-----------+---------+---------------+
// | 0xA5 | SetupFlags | sendDuty | ackDuty | ackRetry | sleepDuty | batDuty | antennaChoice |
// +------+------------+----------+---------+----------+-----------+---------+---------------+
// Setup Flags field:
//  1 - Reset					0x10 - Update ackRetry
//  2 - Factory defaults		0x20 - Update sleepDuty
//  4 - Update sendDuty		    0x40 - Update batDuty
//  8 - Update ackDuty			0x80 - Update antenna Choice
``` 

* SendDuty: temps entre deux mesure environmental et communication associée x 10s
* AckDuty: nombre de SendDuty periode avant d'emettre une trame avec ack pour vérifier l'accès au reseau
* AckRetry: nombre de AckDuty manqué avant de considérer la connexion comme perdue et se reconnecter
* SleepDuty: temps entre deux tentatives de reconnexion au reseau lorawan exprimé en nombre de SendDuty
* BatDuty: temps entre deux emission de message de status batterie x 10m
* Antenna: 
	0 - PiFa
	1 - PCB
	

## MVP 5
 * Acces à la memoire NFC en utilisant le lecteur Devkit ST
 * Utiliser le client adhoc Java disponible dans le repo dans _nfc_client/st25r3911b/nfcReader/out/artifects/nfcReader_jar_
 * utilisation en mode interractif:
 ```C
 java -d32 -jar nfcReader.jar
 ```

 * Acces à l'aide
 ```C
 java -d32 -jar nfcReader.jar -h
 ```
 
 * Utilisation dans un script automatique => exemple
 ```C
 java -d32 -jar nfcReader.jar -ftms:changeme:s,c
 ```
 Affiche le status et la config apres s'etre loggé sur la console.
 
