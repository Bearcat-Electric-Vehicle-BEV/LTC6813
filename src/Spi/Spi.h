#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <SPI.h>

// SPI pins
#define CS          10      // chip select pin isoSPI
#define CS2         38      // 3nd chip select pin isoSPI
#define SC          20      // shutdown circuit pin

void Spi_Init(uint8_t pin, uint8_t dataMode);

#endif