#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "../CONFIGURE.h"
#include "../Utils/Utils.h"
#include "../Ev4/Ev4.h"
#include "../Measurement/Measurement.h"

void Watchdog_Init(Ev4_t *ctx);
void Watchdog_Callback(Ev4_t *ctx);
void Watchdog_CallbackWrapper();
bool Watchdog_Reset(Ev4_t *ctx);

#endif