#ifndef DRIVE_H
#define DRIVE_H

#include "../CONFIGURE.h"
#include "../Soc/Soc.h"
#include "../Can/Can.h"
#include "../Watchdog/Watchdog.h"
#include "../Sd/Sd.h"
#include "../Ev4/Ev4.h"
#include "../Measurement/Measurement.h"

void Drive_State(Ev4_t *ctx, int t, CAN_message_t msg);

#endif