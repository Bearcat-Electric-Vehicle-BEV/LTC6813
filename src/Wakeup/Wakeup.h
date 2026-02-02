#ifndef WAKEUP_H
#define WAKEUP_H

#include <cstdint>
#include <Spi/Spi.h>
#include <CONFIGURE.h>

void Wakeup_Sleep(uint8_t total_ic); 
void Wakeup_Idle(uint8_t total_ic); // idle after 4.3 ms of no isoSPI activity

#endif