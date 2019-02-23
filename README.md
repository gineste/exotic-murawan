# MURAWAN

## MVP1
* Verification de la présence d'une configuration (devEUI != 0). Si la config n'est pas faire l'objet se met dans un état d'attente infini. Il sera rebooté apres configuration.
* La solution lit les capteurs selon un intervalle de temps configurable.
* Se connecte au reseau LoRa en détectant les déconnection par des ack émis régulièrement et paramétrable. Après un nombre donné de Ack manqué consecutifs la deconnection et identifiée et un processus de reconnection est lancé avec une fréquence paramétrable.
* Une fois connectée, les données sont emises sur le réseau au format cayenne.

