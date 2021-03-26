
#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"

#define HI 1
#define LO 0

#define RS GPIO_NUM_19
#define RW GPIO_NUM_21
#define EN GPIO_NUM_23

#define DB4 GPIO_NUM_18
#define DB5 GPIO_NUM_33
#define DB6 GPIO_NUM_32
#define DB7 GPIO_NUM_4

#define RS_POS GPIO_SEL_19
#define RW_POS GPIO_SEL_21
#define EN_POS GPIO_SEL_23

#define DB4_POS GPIO_SEL_18
#define DB5_POS GPIO_SEL_32
#define DB6_POS GPIO_SEL_33
#define DB7_POS GPIO_SEL_4

gpio_config_t io_conf;
char toto[40];

void setInterface()
{
    uint8_t data = 0x30;
    gpio_set_level(RS, LO);
    gpio_set_level(RW, LO);
    gpio_set_level(EN, LO);
    gpio_set_level(DB4, (data >> 4) & 1);
    gpio_set_level(DB5, (data >> 5) & 1);
    gpio_set_level(DB6, (data >> 6) & 1);
    gpio_set_level(DB7, (data >> 7) & 1);
    gpio_set_level(EN, HI);
    vTaskDelay(1);
    gpio_set_level(EN, LO);
    vTaskDelay(1);
}

uint8_t getAddress()
{
    uint8_t result = 0;
    gpio_set_level(DB4, LO);
    gpio_set_level(DB5, LO);
    gpio_set_level(DB6, LO);
    gpio_set_level(DB7, LO);
    gpio_set_level(RS, LO);
    gpio_set_level(RW, HI);
    vTaskDelay(1);
    gpio_set_level(EN, HI);
    vTaskDelay(1);
    gpio_set_level(EN, LO);
    result += gpio_get_level(DB4) << 4;
    result += gpio_get_level(DB5) << 5;
    result += gpio_get_level(DB6) << 6;
    result += gpio_get_level(DB7) << 7;
    vTaskDelay(1);
    gpio_set_level(EN, HI);
    vTaskDelay(1);
    gpio_set_level(EN, LO);
    result += gpio_get_level(DB4);
    result += gpio_get_level(DB5) << 1;
    result += gpio_get_level(DB6) << 2;
    result += gpio_get_level(DB7) << 3;
    return result;
}

bool isBusy()
{
    return (bool)(getAddress() >> 7);
}

void setData(bool isCommand, uint8_t data)
{
    while (isBusy())
    {
        // vTaskDelay(1);
    };
    vTaskDelay(1);
    gpio_set_level(RS, !isCommand);
    gpio_set_level(RW, LO);
    gpio_set_level(EN, LO);
    gpio_set_level(DB4, (data >> 4) & 1);
    gpio_set_level(DB5, (data >> 5) & 1);
    gpio_set_level(DB6, (data >> 6) & 1);
    gpio_set_level(DB7, (data >> 7) & 1);
    gpio_set_level(EN, HI);
    vTaskDelay(1);
    gpio_set_level(EN, LO);
    vTaskDelay(1);
    gpio_set_level(DB4, (data >> 0) & 1);
    gpio_set_level(DB5, (data >> 1) & 1);
    gpio_set_level(DB6, (data >> 2) & 1);
    gpio_set_level(DB7, (data >> 3) & 1);
    gpio_set_level(EN, HI);
    vTaskDelay(1);
    gpio_set_level(EN, LO);
}

void InitLCD(void)
{
    // ESP_ERROR_CHECK(gpio_reset_pin(RS));
    // ESP_ERROR_CHECK(gpio_reset_pin(RW));
    // ESP_ERROR_CHECK(gpio_reset_pin(EN));
    // ESP_ERROR_CHECK(gpio_reset_pin(DB4));
    // ESP_ERROR_CHECK(gpio_reset_pin(DB5));
    // ESP_ERROR_CHECK(gpio_reset_pin(DB6));
    // ESP_ERROR_CHECK(gpio_reset_pin(DB7));
    ESP_ERROR_CHECK(gpio_set_direction(RS, GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(RW, GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(EN, GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(DB4, GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(DB5, GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(DB6, GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(DB7, GPIO_MODE_INPUT_OUTPUT));

    gpio_set_level(RS, LO);
    gpio_set_level(RW, LO);
    gpio_set_level(EN, LO);
    gpio_set_level(DB4, LO);
    gpio_set_level(DB5, LO);
    gpio_set_level(DB6, LO);
    gpio_set_level(DB7, LO);

    vTaskDelay(100);
    setInterface();

    vTaskDelay(100);
    setInterface();

    vTaskDelay(1);
    setInterface();

    gpio_set_level(DB4, LO);
    gpio_set_level(EN, HI);
    vTaskDelay(1);
    gpio_set_level(EN, LO);
    vTaskDelay(1);
    setData(true, 0x28);
    vTaskDelay(1);
    setData(true, 0x28);
    vTaskDelay(1);
    setData(true, 0x06);
    vTaskDelay(1);
    setData(true, 0x0F);
    vTaskDelay(1);
    setData(true, 0x01);
    setData(true, 0x02);
    while ((isBusy()))
    {
        vTaskDelay(1);
    }
    vTaskDelay(1);
    setData(false, 65);
    setData(false, 65);
    setData(false, 65);
    setData(false, 65);
    setData(false, 65);
    setData(false, 65);
}

void app_main()
{
    InitLCD();
    while (1)
    {

        vTaskDelay(1);
    }
}
