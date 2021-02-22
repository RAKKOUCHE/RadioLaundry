/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\src\main.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 16/02/2021
* \brief FW Satellite
* \remarks None
*/

/*!
* \def LOG_LOCAL_LEVEL
* Description Definition locale du niveau de log
*/
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

/*! Fichiers inclus*/
#include "main.h"

/*!
* \def TAG_MAIN
* Description TAG utilisé par les logs pour ce fichier
*/
#define TAG_MAIN "Main module"

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
* \date  16/02/2021
* \brief Initlialisation de l'application.
* \remarks None
* \return 
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
    //uint8_t oldAddress = 18;
    uint8_t newAddress = 11;

    int delay = 10;
    ESP_LOGI(TAG_MAIN, "%s", "Debut du programme");
    //Initialisation du programme
    InitApp();

    //Boucle permanente
    while (1)
    {
        //Permet de réinitialiser le watchdog
        vTaskDelay(100);
        ESP_LOGD(TAG_MAIN, "%s", "Loop\n");

        //Si le bouton est utilisé
        if (!gpio_get_level(BUTTON))
        {
            setESPNOWTaskState(ESPNOWMSGSEND);

            ESP_LOGI(TAG_MAIN, "%s%u\n", "Numéro de série : ", ESPNOWGetSerialNumber(newAddress));
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
