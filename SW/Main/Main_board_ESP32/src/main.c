/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Main_board_ESP32\src\main.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 23/02/2021
* \brief 
* \remarks None 
*/

//TODO:supprimer la lecture des paramètres dans la release.

/*! Fichiers inclus*/
#include "main.h"

/*!
* \def TAG_MAIN
* \brief TAG utilisé par les logs pour ce fichier
*/
#define TAG_MAIN "\nMain module : "

/*!
* \def BUTTON
* \brief
*/
#define BUTTON 0

/*!
* \def newDevice
* Description
*/
#define newDevice 0XFF

/*!
* \fn static void createTasks(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief Crée les tâches utilisées par Freertos dans le firmware 
* \remarks None
* \return 
*/
static void createTasks(void)
{
    //Création la tâche des entrées sorties
    xTaskCreate(TaskIO, "Task IO", 4096, NULL, 1, &hTaskIO);
    //Création de la tâche ESPNOW
    xTaskCreate(TASKESPNOW, "Task ESPNOW", 4096, NULL, 2, &hTaskESPNOW);
}

/*!
* \fn static void InitApp(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  23/02/2021
* \brief 
* \remarks None
* \return 
*/
static void InitApp(void)
{
    //Initialisation nécessaire de la flash
    InitFlash();
    // //Initialisation des logs
    // InitESPLOG();
    //Lecture des paramètres
    //InitParameters();
    // //Creation des tâches
    createTasks();
    //Initialise le WIFI
    xTaskNotifyGive(hTaskESPNOW);
    //Initialise ESPNOW
    xTaskNotifyGive(hTaskESPNOW);
    //Led 1 clignotante
    setLED(LED_1);
    setIOState(IOLedFlash);
}

/*!
* \fn void app_main()
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief Point d'entrée du programme
* \remarks None
*/
void app_main()
{

    uint8_t newAddress = 11;
    volatile bool isMachineNew = false;
    printf("%s%s", TAG_MAIN, "Debut du programme");
    //Initialisation du programme
    InitApp();

    //TODO recherche des modules connectés de 10 à 99
    //Boucle permanente
    while (1)
    {
        //Permet de réinitialiser le watchdog
        vTaskDelay(100);
        //Si le bouton est utilisé
        if (!gpio_get_level(BUTTON))
        {
            printf("%s%s", TAG_MAIN, "Bouton utilisé");

            //Envoie un poll.
            printf("%s%s%s\n", TAG_MAIN, ((isMachineNew = ESPNOWPoll(newDevice)) ? "Une" : "Pas de"), " nouvelle machine detéctée.");
            printf("\n");
            if (isMachineNew)
            {
                //Change l'adresse du module
                printf("%s%s", TAG_MAIN, "Lecture de la configuration du nouveau module\n");
                if (getConfig(MachineAddress = newDevice))
                {
                    machineConfig.config.address = newAddress;
                    printf("%s%s%u%s", TAG_MAIN, "La configuration du module ", newAddress, setConfig(newDevice, machineConfig) ? " a réussi " : " a échoué");
                }
            }
            printf("%s", "\n");
            //Lecture du numéro de série de carte
            printf("%s%s%s\n", TAG_MAIN, "Le pool a ", (ESPNOWPoll(MachineAddress)) ? "réussi" : "échoué");
            printf("%s%s%u\n", TAG_MAIN, "Le numéro de série est : ", getESPNOWSerialNumber(MachineAddress));
            getConfig(MachineAddress);
            vTaskDelay(100);
            // machineConfig.config.lPulseInMS = 5000;
            // setConfig(MachineAddress, machineConfig);
            //Activation du relais de la machine
            printf("%s%s%s\n", TAG_MAIN, ((isMachineNew = ESPNOWPoll(newDevice)) ? "Une" : "Pas de"), " nouvelle machine detéctée.");
            vTaskDelay(100);
            printf("%s%s%s\n", TAG_MAIN, "Ecriture de l'etat du relais machine. Le relais est ", setESPNOWMachineRelay(MachineAddress, true) ? "activé" : " au repos");
            vTaskDelay(5);

            printf("%s%s%s\n", TAG_MAIN, "Lecture de l'etat du relais machine. Le relais est ", getESPNOWStateMachineRelay(MachineAddress) ? "activé" : "au repos");
            vTaskDelay(1000);
            printf("%s%s%s\n", TAG_MAIN, "Lecture de l'etat du relais machine. Le relais est ", getESPNOWStateMachineRelay(MachineAddress) ? "activé" : "au repos");
            vTaskDelay(100);

            printf("%s%s%s\n", TAG_MAIN, "Ecriture de l'etat du  relais. Le relais est ", setESPNOWMachineRelay(MachineAddress, false) ? "activé" : "repos");
            vTaskDelay(100);
            printf("%s%s%s\n", TAG_MAIN, "Lecture de l'etat du  relais machine. Le relais est ", getESPNOWStateMachineRelay(MachineAddress) ? "activé" : "repos");

            printf("%s%s%u\n", TAG_MAIN, "Le niveau d'occupation est : ", getADCValueBusy(MachineAddress));
            printf("%s%s%s%u\n", TAG_MAIN, "Ecriture de délai overbusy a ", setDelayOverBusy(MachineAddress, 60) ? "réussi : " : "echoué : ", 60);
            vTaskDelay(1 * SECONDE);

            printf("%s%s%u%s\n", TAG_MAIN, "Valeur du délai d'overlay : ", getDelayOverBusy(MachineAddress), " secondes.");
            printf("%s%s%u\n", TAG_MAIN, "Le niveau d'occupation est : ", getADCValueBusy(MachineAddress));

            //Activation du relais du secteur
            printf("%s%s%s\n", TAG_MAIN, "Ecriture de l'etat du  relais secteur. Le relais est ", setESPNOWMainRelay(MachineAddress, true) ? "activé" : "au repos");
            vTaskDelay(100);
            printf("%s%s%s\n", TAG_MAIN, "Lecture de l'etat du  relais secteur. Le relais est ", getESPNOWStateMainRelay(MachineAddress) ? "activé" : "au repos");
            vTaskDelay(600);
            printf("%s%s%s\n", TAG_MAIN, "Ecriture de l'etat du  relais secteur. Le relais est ", setESPNOWMainRelay(MachineAddress, false) ? "activé" : "au repos");
            printf("%s%s%s\n", TAG_MAIN, "Lecture de l'etat du  relais secteur. Le relais est ", getESPNOWStateMainRelay(MachineAddress) ? "activé" : "au repos");
            vTaskDelay(100);
            printf("%s%s%s%s%s", TAG_MAIN, "Lecture du niveau de la machine", " La machine ", isMachineEmpty(MachineAddress) ? "est" : "n'est pas", " vide\n");
            printf(" %s%s%s", TAG_MAIN, "L'écriture du délai d'activation du relais machine a ", setDelayActivationRelayMachine(MachineAddress, 10000) ? "réussi" : "échoué");

            vTaskDelay(100);

            printf("%s%s%u%s\n", TAG_MAIN, "Le délai d'activation du relais est de ", getDelayActivationRelayMachine(MachineAddress), " millisecondes.");
            printf("%s%s%s\n", TAG_MAIN, "Ecriture de l'etat du relais machine. Le relais est ", setESPNOWMachineRelay(MachineAddress, true) ? "activé" : "repos");

            printf("%s%s%u%s\n", TAG_MAIN, "Le temps d'activation restant est de ", getRestActivationRelayMachine(MachineAddress), " millisecondes.");

            printf("\n");
            //Attend le relachement du bouton.
            while (!gpio_get_level(BUTTON))
            {
                vTaskDelay(1);
            };
        }

        // //Arrête le glignotement de la led
        // if (delay && (--delay == 0))
        // {
        //     setLED(LED_1);
        //     setIOState(IOLEDOff);
        // }
    }
}
