#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "../Utils/Utils.h"
#include "../Adc/Adc.h"
#include "../CONFIGURE.h"
#include "../COMMANDS.h"
#include "../Ev4/Ev4.h"

void measure_voltage(Ev4_t *ctx); // 18 millisecond execution time
void measure_temp(Ev4_t *ctx, bool open_wire_check = false);
void measure_current(Ev4_t *ctx);
void measure_die_temp(Ev4_t *ctx);

#endif
