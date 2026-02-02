#ifndef DATA_H
#define DATA_H

#include "../Utils/Utils.h"
#include "../Sd/Sd.h"
#include "../Ev4/Ev4.h"
#include "../CONFIGURE.h"

void dump_data_to_serial();
void check_memory(Ev4_t *ctx);

#endif