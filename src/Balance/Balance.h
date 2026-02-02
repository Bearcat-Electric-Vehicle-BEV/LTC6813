#ifndef BALANCE_H
#define BALANCE_H

#include "../Ev4/Ev4.h"
#include "../Utils/Utils.h"
#include "../CONFIGURE.h"
#include "../Measurement/Measurement.h"

void discharge_cells(bool discharge[num_boards][18]);
void balance_cells(Ev4_t *ctx, bool set);

#endif