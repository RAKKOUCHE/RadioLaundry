
#include "stdio.h"
#include "driver/gpio.h"
#include "esp_err.h"

#define RS GPIO_NUM_19
#define RW GPIO_NUM_21
#define EN GPIO_NUM_23

#define DB4 GPIO_NUM_18
#define DB5 GPIO_NUM_17
#define DB6 GPIO_NUM_16
#define DB7 GPIO_NUM_4

#define RS_POS GPIO_SEL_19
#define RW_POS GPIO_SEL_21
#define EN_POS GPIO_SEL_23

#define DB4_POS GPIO_SEL_18
#define DB5_POS GPIO_SEL_17
#define DB6_POS GPIO_SEL_16
#define DB7_POS GPIO_SEL_4

gpio_config_t io_conf;
char toto[40];

void app_main()
{
    io_conf.pin_bit_mask = DB4_POS | DB5_POS | DB6_POS | DB7_POS;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT_OD;
    io_conf.pull_up_en = true;
    io_conf.pull_down_en = false;
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    sprintf(toto, "%s", "toto");
}
