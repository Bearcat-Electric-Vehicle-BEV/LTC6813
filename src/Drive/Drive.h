#ifndef DRIVE_H
#define DRIVE_H

#include <Ev4/Ev4.h>
#include <CONFIGURE.h>
#include <Measurement/Measurement.h>
#include <Watchdog/Watchdog.h>
#include <Sd/Sd.h>
#include <Can/Can.h>

void Drive(Ev4_t *ctx, int t, CAN_message_t msg);

#endif