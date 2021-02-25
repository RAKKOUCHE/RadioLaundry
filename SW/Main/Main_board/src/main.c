/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Main_board\src\main.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 23/02/2021
* \brief 
* \remarks None 
*/

//TODO:ssupprimer la lecture des paramètres dans la release.

/*! Fichiers inclus*/
#include "main.h"

/*!
* \def TAG_MAIN
* Description TAG utilisé par les logs pour ce fichier
*/
#define TAG_MAIN "\nMain module : "

/*!
* \def BUTTON
* Description
*/
#define BUTTON 0

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
    xTaskCreate(TaskIO, "Task IO", 2048, NULL, 1, &hTaskIO);
    //Création de la tâche ESPNOW
    xTaskCreate(TASKESPNOW, "Task ESPNOW", 2560, NULL, 1, &hTaskESPNOW);
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
    uint8_t delay = 10;
    //uint8_t newAddress = 11;
    printf("%s%s", TAG_MAIN, "Debut du programme");
    //Initialisation du programme
    InitApp();

    //Boucle permanente
    while (1)
    {
        //Permet de réinitialiser le watchdog
        vTaskDelay(100);
        //Si le bouton est utilisé
        if (!gpio_get_level(BUTTON))
        {
            printf("%s%s", TAG_MAIN, "Bouton utilisé");

            //Prépare l'envoie d'une commande

            setESPNOWTaskState(ESPNOWMSGSEND);
            // // //Envoie un poll.
            // printf("\n%s%s%s\n", TAG_MAIN, "le pool a ", ESPNOWPoll(MachineAddress) ? "réussi" : "échoué");

            // //Change l'adresse du module
            // printf("\n%s%s%s\n", TAG_MAIN, "Le changement d'adresse a ", ESPNOWSetNewAddress(MachineAddress, newAddress) ? "réussi" : "échoué");

            //Lecture du numéro de série de carte
            printf("%s%s%u", TAG_MAIN, "Le numéro de série est : ", ESPNOWGetSerialNumber(MachineAddress));

            //Attend le relachement du bouton.
            while (!gpio_get_level(BUTTON))
                ;
        }

        //Arrête le glignotement de la led
        if (delay && (--delay == 0))
        {
            setLED(LED_1);
            setIOState(IOLEDOff);
        }
    }
}
