/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\include\io.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 16/02/2021
* \brief 
* \remarks None
*/

#ifndef IO_H
#define IO_H

/*! Importation des librairies*/
/*! Fichiers includes*/
/*! Fichiers inclus*/
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*!
* \def POS_LED_1
* Description
*/
#define POS_LED_1 GPIO_SEL_2
/*!
* \def POS_LED_2
* Description
*/
#define POS_LED_2 GPIO_SEL_5
/*!
* \def POS_CTRL_MACHINE
* Description
*/
#define POS_CTRL_MACHINE GPIO_SEL_25
/*!
* \def POS_CTRL_MAIN
* Description
*/
#define POS_CTRL_MAIN GPIO_SEL_32
/*!
* \def POS_EMPTY
* Description
*/
#define POS_EMPTY GPIO_SEL_33
/*!
* \def POS_DIPSWITCH_2
* Description
*/
#define POS_DIPSWITCH_2 GPIO_SEL_4
/*!
* \def POS_BOOT
* Description
*/
#define POS_BOOT GPIO_SEL_0

/*!
* \def LED_1
* Description
*/
#define LED_1 GPIO_NUM_2

/*!
* \def LED_2
* Description
*/
#define LED_2 GPIO_NUM_5
/*!
* \def CTRL_MACHINE
* Description
*/
#define CTRL_MACHINE GPIO_NUM_25
/*!
* \def CTRL_MAIN
* Description
*/
#define CTRL_MAIN GPIO_NUM_32
/*!
* \def EMPTY
* Description
*/
#define EMPTY GPIO_NUM_33
/*!
* \def DIPSWITCH_2
* Description
*/
#define DIPSWITCH_2 GPIO_NUM_4
/*!
* \def BOOT
* Description
*/
#define BOOT GPIO_NUM_0

/**
 * @brief 
 */
TaskHandle_t hTaskIO;

/**
*\brief
*/
typedef enum
{
    IOTASKIDLE,
    IOTASKINIT,
    IOLEDFLASH,
    IOLEDON,
    IOLEDOFF,
    IORELAYMACHINEON,
    IORELAYMACHINEOFF,
} IOTaskState_t;

/*!
* \fn void setIOState(IOTaskState_t state)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param state 
*/
void setIOState(IOTaskState_t state);

/*!
* \fn IOTaskState_t getIOState(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  25/02/2021
* \brief 
* \remarks None
* \return 
*/
IOTaskState_t getIOState(void);

/*!
* \fn void setLED(const gpio_num_t led)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param[in] led 
*/
void setLED(const gpio_num_t led);

/*!
* \fn void TaskIO(void *vParameter)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief 
* \remarks None
* \param vParameter 
*/
void TaskIO(void *vParameter);

#endif