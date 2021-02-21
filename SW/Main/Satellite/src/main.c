/*!
* \file d:\Projets\RadioLaundry\SW\Main\Satellite\src\main.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 16/02/2021
* \brief FW Satellite
* \remarks None
*/

#define DEBUG

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

/*! Importation des librairies*/
#include "main.h"

//A supprimer pour la version release
#define DEBUG

/*!
* \def MAIN_TAG
* Description TAG utilisé par les logs
*/
#define MAIN_TAG "Main module"

static void InitESPLOG();
static void createTasks(void);
static void InitApp(void);

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
        if (!gpio_get_level(0))
        {
            poll(10);
            ESP_LOGD(MAIN_TAG, "%s\n", "Bouton utilisé");
            while (!gpio_get_level(0))
                ;
        }
    }
}

/*!
* \fn void InitApp(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief 
* \remarks None
*/
void InitApp(void)
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
}

/*!
* \fn void InitESPLOG()
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief Initialisation des logs
* \remarks None
*/
void InitESPLOG()
{
    ESP_LOGI(MAIN_TAG, "%s", esp_log_system_timestamp());
#ifdef DEBUG
    esp_log_level_set("*", ESP_LOG_VERBOSE);
#else
    esp_log_level_set("*", ESP_LOG_INFO);
#endif
    ESP_LOGI(MAIN_TAG, "%s%u", "Début du programme \n", 1);
}

/*!
* \fn void createTasks(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief 
* \remarks None
*/
void createTasks(void)
{
    //Création la tâche des entrées sorties
    xTaskCreate(TaskIO, "Task IO", 2048, NULL, 1, &hTaskIO);
    //Création de la tâche ESPNOW
    xTaskCreate(TASKESPNOW, "Task ESPNOW", 2560, NULL, 1, &hTaskESPNOW);
}