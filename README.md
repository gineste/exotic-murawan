# MURAWAN

## MVP1
* Verification de la pr�sence d'une configuration (devEUI != 0). Si la config n'est pas faire l'objet se met dans un �tat d'attente infini. Il sera reboot� apres configuration.
* La solution lit les capteurs selon un intervalle de temps configurable.
* Se connecte au reseau LoRa en d�tectant les d�connection par des ack �mis r�guli�rement et param�trable. Apr�s un nombre donn� de Ack manqu� consecutifs la deconnection et identifi�e et un processus de reconnection est lanc� avec une fr�quence param�trable.
* Une fois connect�e, les donn�es sont emises sur le r�seau au format cayenne.

## MVP3
* Capture des informations de niveau de batterie. La periode est donn�e par le parametre batDuty par pas de 10 minutes.
* Selon que l'alim est suffisante ou non seule le vbat est remont� ou l'ensemble des information des cellules sont remont�s.
