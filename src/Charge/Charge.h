#ifndef CHARGE_H
#define CHARGE_H

#include <Ev4/Ev4.h>
#include <FlexCAN_T4.h>
#include <Measurement/Measurement.h>
#include <Can/Can.h>
#include <Watchdog/Watchdog.h>
#include <Sd/Sd.h>

void Charge_EnableCharger(Ev4_t *ctx, bool enable);
void Charge_Precharge(Ev4_t *ctx, CAN_message_t msg, float charger_voltage, float charger_current);
void Charge(Ev4_t *ctx, CAN_message_t msg, float charger_voltage, float charger_current, uint32_t charge_start_time);

#endif