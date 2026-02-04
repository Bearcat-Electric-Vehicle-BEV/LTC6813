#ifndef SD_H
#define SD_H

#include <FS.h>
#include <SD.h>
#include "../Ev4/Ev4.h"
#include "../CONFIGURE.h"

// SD card pins
const int chipSelect = BUILTIN_SDCARD;

void Sd_DataWrite(Ev4_t *ctx);

#endif