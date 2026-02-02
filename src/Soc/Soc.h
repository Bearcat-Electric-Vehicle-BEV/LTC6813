#ifndef SOC_H
#define SOC_H

#include <Ev4/Ev4.h>
#include <HEADER.h>
#include <CONFIGURE.h>
#include <SD.h>

void Soc_Get(Ev4_t *ctx);
float Soc_Update(Ev4_t *ctx);

#endif