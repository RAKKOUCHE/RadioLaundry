#include "adc.h"

#define TAG_ADC "\nModule ADC : "

uint16_t getADCValueBusy(uint8_t address)
{
    printf("%sDemande la valeur d'occupation", TAG_ADC);
    prepareMessageToSend(address, REQUEST_MACHINE_BUSY_LEVEL, 0, NULL);
    printf("%s%s%u", TAG_ADC, "La valeur d'occupation : ", msg_received[4] + (msg_received[5] * 0X100));
    return (msg_received[4] + (msg_received[5] * 0X100));
}