/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\src\adc.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 28/02/2021
* \brief 
* \remarks None
*/

/*! Fichiers inclus*/
#include "adc.h"
#define TAG_ADC "\nADC Module : "
/*!
* \def NO_OF_SAMPLES
* Description
*/
#define NO_OF_SAMPLES 16

/**
 * @brief 
 * 
 */
static uint16_t adc_readed;

/**
 * @brief 
 * 
 */
static const adc_channel_t channel = ADC_CHANNEL_6;

/*!
* \fn uint16_t getADCValue(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  28/02/2021
* \brief 
* \remarks None
* \return 
*/
uint16_t getADCValue(void)
{
    return adc_readed;
}

/*!
* \fn void initADC(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  28/02/2021
* \brief 
* \remarks None
*/
void initADC(void)
{
    printf("%s%s", TAG_ADC, "Initialisation de l'ADC");
    printf("\n");
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11));
}

/*!
* \fn void vTaskADC(void *vParameters)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  28/02/2021
* \brief 
* \remarks None
* \param vParameters 
*/
void vTaskADC(void *vParameters)
{
    while (1)
    {
        int adc_read = 0;
        for (int i = 0; i < NO_OF_SAMPLES; i++)
        {
            adc_read += abs(adc1_get_raw((adc1_channel_t)channel));
        }
        adc_readed = (adc_read / NO_OF_SAMPLES);
        printf("%s%s%d\n", TAG_ADC, "Valeur ADC : ", adc_readed);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}