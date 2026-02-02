#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <CONFIGURE.h>
#include <COMMANDS.h>
#include <HEADER.h>
#include <Ev4/Ev4.h>
#include <Adc/Adc.h>

void measure_voltage(Ev4_t *ctx);
void measure_temp(Ev4_t *ctx, bool open_wire_check = false);
void measure_current(Ev4_t *ctx);
void measure_die_temp(Ev4_t *ctx);

#endif
