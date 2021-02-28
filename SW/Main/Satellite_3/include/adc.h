/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\include\adc.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 28/02/2021
* \brief 
* \remarks None
*/

#ifndef ADC_H
#define ADC_H

/*! Fichiers inclus*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc_cal.h"

/**
 * @brief 
 * 
 */
TaskHandle_t hTaskADC;

/*!
* \fn uint32_t getADCValue(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  28/02/2021
* \brief 
* \remarks None
* \return 
*/
uint16_t getADCValue(void);

/*!
* \fn void initADC(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  28/02/2021
* \brief 
* \remarks None
*/
void initADC(void);

/*!
* \fn void vTaskADC(void *vParameters)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  28/02/2021
* \brief 
* \remarks None
* \param vParameters 
*/
void vTaskADC(void *vParameters);

#endif