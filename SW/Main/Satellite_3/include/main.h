/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\include\main.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 16/02/2021
* \brief Fichier enête du fichier prinicpal
* \remarks Supprimer la definition DEBUG pour la version release
*/

#ifndef MAIN_H
#define MAIN_H

/*! Fichiers inclus*/
#include <stdio.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "esp_err.h"
#include "Freertos/Freertos.h"
#include "Freertos/task.h"
#include "io.h"
#include "flash.h"
#include "wifi.h"
#include "espnow.h"
#include "parameters.h"

int delay;

#endif