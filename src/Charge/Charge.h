#ifndef CHARGE_H
#define CHARGE_H

#include <FlexCAN_T4.h>
#include "../Can/Can.h"
#include "../Watchdog/Watchdog.h"
#include "../Sd/Sd.h"
#include "../Ev4/Ev4.h"
#include "../Measurement/Measurement.h"
#include "../Soc/Soc.h"

void Charge_ConfigureCharger(Ev4_t *ctx, bool enable, uint16_t charger_current = 0);
void Charge_Precharge(Ev4_t *ctx);
void Charge_State(Ev4_t *ctx, uint32_t charge_start_time); // Charge cycle loop

#endif