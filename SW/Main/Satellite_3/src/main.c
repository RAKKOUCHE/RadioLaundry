/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\src\main.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 16/02/2021
* \brief FW Satellite
* \remarks None
*/

//TODO: Supprimer l'enregistrement du numéro 11 dans la version release.

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
    printf("%s%s", TAG_MAIN, "Création des  tâches\n");
    //Création la tâche des entrées sorties
    xTaskCreate(vTaskIO, "Task IO", 1792, NULL, 1, &hTaskIO);
    //Création de la tâche ESPNOW
    xTaskCreate(vTaskESPNOW, "Task ESPNOW", 2304, NULL, 1, &hTaskESPNOW);
    //Création de la tâche ADC
    xTaskCreate(vTaskADC, "Tâche ADC", 1792, NULL, 1, &hTaskADC);
}

/*!
* \fn static void initApp(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief Initlialisation de l'application.
* \remarks None
* \return 
*/
static void initApp(void)
{
    //Initialisation nécessaire de la flash
    initFlash();
    //Lecture des paramètres
    initParameters();
    //Initialisation du convertisseur ADC
    initADC();
    //Creation des tâches
    createTasks();
    //Initialise le WIFI
    xTaskNotifyGive(hTaskESPNOW);
    //Initialise ESPNOW
    xTaskNotifyGive(hTaskESPNOW);
    //Led 1 clignotante
    setLED(LED_1);
    setIOState(IOLEDFLASH);
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
    printf("%s%s", TAG_MAIN, "Debut du programme");
    delayBlink = 5;
    //Initialisation du programme
    initApp();

    //Boucle permanente
    while (1)
    {
        //Si le bouton est utilisé
        if (!gpio_get_level(BUTTON))
        {
            printf("%s%s", TAG_MAIN, "Bouton utilisé");
            while (!gpio_get_level(BUTTON))
            {
                vTaskDelay(1);
            };
        }
        //Arrête le glignotement de la led
        if (delayBlink && (--delayBlink == 0))
        {
            setLED(LED_1);
            setIOState(IOLEDOFF);
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
