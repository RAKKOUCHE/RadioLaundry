/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\include\logs.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 21/02/2021
* \brief Fichier d'entête du fichier d'initialisation des logs
* \remarks None
*/

#ifndef LOGS_H
#define LOGS_H

/*! Fichiers inclus*/
#include "esp_log.h"

/*!
* \def DEBUG
* Description Flag à retirer pour la version release
*/
#define DEBUG

/*!
* \fn void InitESPLOG()
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief Protoype de la  fonction d'intialisation des logs
* \remarks None
*/
void InitESPLOG();

#endif