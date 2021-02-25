/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Main_board\include\io.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 16/02/2021
* \brief 
* \remarks None
*/

#ifndef IO_H
#define IO_H

/*! Fichiers inclus*/
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*!
* \def POS_LED_1
* \brief
*/
#define POS_LED_1 GPIO_SEL_2
/*!
* \def POS_LED_2
* \brief
*/
#define POS_LED_2 GPIO_SEL_5
/*!
* \def POS_CTRL_MACHINE
* \brief
*/
#define POS_CTRL_MACHINE GPIO_SEL_25
/*!
* \def POS_CTRL_MAIN
* \brief
*/
#define POS_CTRL_MAIN GPIO_SEL_32
/*!
* \def POS_EMPTY
* \brief
*/
#define POS_EMPTY GPIO_SEL_33
/*!
* \def POS_DIPSWITCH_2
* \brief
*/
#define POS_DIPSWITCH_2 GPIO_SEL_4
/*!
* \def POS_BOOT
* \brief
*/
#define POS_BOOT GPIO_SEL_0

/*!
* \def LED_1
* \brief
*/
#define LED_1 GPIO_NUM_2

/*!
* \def LED_2
* \brief
*/
#define LED_2 GPIO_NUM_5
/*!
* \def CTRL_MACHINE
* \brief
*/
#define CTRL_MACHINE GPIO_NUM_25
/*!
* \def CTRL_MAIN
* \brief
*/
#define CTRL_MAIN GPIO_NUM_32
/*!
* \def EMPTY
* \brief
*/
#define EMPTY GPIO_NUM_33
/*!
* \def DIPSWITCH_2
* \brief
*/
#define DIPSWITCH_2 GPIO_NUM_4
/*!
* \def BOOT
* \brief
*/
#define BOOT GPIO_NUM_0


/**
*\brief
*/
typedef enum
{
    IOtASKIdle,
    IOTaskInit,
    IOLedFlash,
    IOLEDOn,
    IOLEDOff,
} IOTaskState_t;


/*!
* \fn void setIOState(IOTaskState_t state)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  24/02/2021
* \brief 
* \remarks None
* \param state 
*/
void setIOState(IOTaskState_t state);

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

/**
 * @brief 
 */
TaskHandle_t hTaskIO;

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