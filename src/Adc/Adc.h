#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "../Ev4/Ev4.h"
#include "../Utils/Utils.h"
#include "../CONFIGURE.h"

void Adc_Init(Ev4_t *ctx);
void Adc_Read();
void Adc_Poll(Ev4_t *ctx, uint16_t command);

#endif