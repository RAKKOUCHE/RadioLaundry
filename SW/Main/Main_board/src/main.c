/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Main_board\src\main.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 23/02/2021
* \brief 
* \remarks None 
*/

/*! Fichiers inclus*/
#include "main.h"

/*!
* \def MAIN_TAG
* Description TAG utilisé par les logs pour ce fichier
*/
#define MAIN_TAG "Main module"

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
*/
static void InitApp(void)
{
    //Initialisation nécessaire de la flash
    InitFlash();
    //Initialisation des logs
    InitESPLOG();
    //Lecture des paramètres
    InitParameters();
    //Creation des tâches
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
    int delay = 10;
    ESP_LOGI(MAIN_TAG, "%s", "Debut du programme");
    //Initialisation du programme
    InitApp();

    //Boucle permanente
    while (1)
    {
        //Permet de réinitialiser le watchdog
        vTaskDelay(100);
        ESP_LOGD(MAIN_TAG, "%s", "Loop\n");

        //Si le bouton est utilisé
        if (!gpio_get_level(BUTTON))
        {
            ESPNOWPoll(MachineAddress);
            ESP_LOGD(MAIN_TAG, "%s\n", "Bouton utilisé");
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
