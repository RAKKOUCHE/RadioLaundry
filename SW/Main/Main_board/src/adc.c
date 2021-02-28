#include "adc.h"

#define TAG_ADC "\nModule ADC : "

uint16_t getADCValueBusy(uint8_t address)
{
    printf("%sDemande la valeur d'occupation", TAG_ADC);
    memset(msg_received, 0, sizeof(msg_received));
    prepareMessageToSend(address, REQUEST_MACHINE_STATUS, 0, NULL);
    printf("%s%s%u", TAG_ADC, "La valeur d'occupation : ", msg_received[5] + (msg_received[4] * 0X100));
    return (msg_received[5] + (msg_received[4] * 0X100));
}