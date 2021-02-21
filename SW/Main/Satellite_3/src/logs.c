/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\src\logs.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 21/02/2021
* \brief Fichier d 'initialisation des logs
* \remarks None
*/

/*! Fichiers inclus*/
#include "logs.h"

/*!
* \def TAG_LOGS
* Description Entête des logs de ce fichier
*/
#define TAG_LOGS "Module logs"

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
    ESP_LOGI(TAG_LOGS, "%s", esp_log_system_timestamp());
#ifdef DEBUG
    esp_log_level_set("*", ESP_LOG_VERBOSE);
#else
    esp_log_level_set("*", ESP_LOG_INFO);
#endif
}
