#ifndef UTILS_H  
#define UTILS_H

#include <stdint.h>
#include <cstddef>
#include <stdarg.h>
#include "../COMMANDS.h"
#include "../CONFIGURE.h"
#include "../Spi/Spi.h"
#include "../Pec/Pec.h"
#include "../Ev4/Ev4.h"
#include "../Wakeup/Wakeup.h"

String format_string(const char* format, va_list args);
void print_with_args(const char* format, ...);
void println_with_args(const char* format, ...);

// finds the max and min values in any static 2D array of floats, min and max must be initalized to sensible values beforehand
template<size_t rows, size_t cols> void min_max(const float arr_2D[rows][cols], float &min, float &max);

// searches a sorted DECREASING list for the nearest element and returns its index, the "lower" flag if set will return the
// nearest element that is equal or lower online function testbench: https://www.programiz.com/online-compiler/5fkt3FMi4yJhY
template<size_t length> int search(const float arr[length], const float value, bool return_lower = false);

// linearly interpolate an x-value from sorted DECREASING arrays: Y = Y1 + (Y2-Y1) / (X2-X1) * (X-X1)
template<size_t length> float interpolate(const float arr_x[length], const float arr_y[length], const float x_value);

uint8_t power_limit(const float max_cell_temp);
uint8_t float_2_uint8_t(const float float_val, const float min, const float max); // float to uint8_t, clips values under/over min or max
float map_voltage_to_temp(float V); // voltage -> actual temp
void map_text2var(Ev4_t *ctx, String name, String value); // map text name and value to a variable

void send_command(Ev4_t *ctx, uint16_t command);
void read_register_group(Ev4_t *ctx, uint16_t command, uint8_t response[num_boards][6]); // register group is always 6 bytes
void write_register_group(Ev4_t *ctx, uint16_t command, uint8_t data[num_boards][6]);

void print_min_max(Ev4_t *ctx); // This function prints the min and max parameters
bool determineMode(Ev4_t *ctx, CAN_message_t msg, bool CAN_baud_alt); // Determine starting mode (return true to break outer loop)
void configure_sense(Ev4_t *ctx);

#endif