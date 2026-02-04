#ifndef SOC_H
#define SOC_H

#include <SD.h>
#include "../Utils/Utils.h"
#include "../CONFIGURE.h"
#include "../Ev4/Ev4.h"

void Soc_Get(Ev4_t *ctx); // SOC should be written in the state.txt file as: "SOC:100"
float Soc_Update(Ev4_t *ctx);

#endif