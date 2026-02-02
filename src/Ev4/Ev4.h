#ifndef EV4_H
#define EV4_H

#include <Watchdog_t4.h>
#include <FlexCAN_T4.h>
#include "../CONFIGURE.h"

typedef struct Ev4 {
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;
    WDT_T4<WDT1> wdt; // watchdog 1 holds output pin low until power-on-reset. This is desired for a shutdown circuit

    uint8_t mode;

    float cell_voltage[num_boards][num_cells]; // most recent cell voltages
    float open_circuit_voltage[num_boards][num_cells];
    float pack_voltage; // sum of cell voltages
    bool new_voltage;

    float cell_temp[num_boards][10]; // most recent cell temperatures. Contains raw voltage data for the duration of open wire checks
    float die_temps[num_boards]; // most recent sense board LTC6813 die temps
    bool new_temp;

    float current;
    float current_offset;

    float soc; // state of charge
    float inv_voltage; // inverter voltage read from CAN
    int data_file_num; // data.csv enumeration

    // measurement buffers
    unsigned int time_buffer[SD_interval];
    float voltage_buffer[SD_interval / volt_interval][num_boards][num_cells];
    float temp_buffer[SD_interval / temp_interval][num_boards][10];
    float current_buffer[SD_interval / current_interval];
    float currentbuffer_stat;

    unsigned int start_time = millis();
    unsigned int sense_watchdog_timer;  // senseboard watchdog timer. Sense boards will go to sleep after 2s
                                        // if no valid command with correct PEC is sent from master.

    // flags
    int wire_cut;
    bool memory_fault;
    bool comms_fault;
    bool curr_sense_fault;
    bool watchdog_callback;
    bool watchdog_reset;
    bool charger_fault;

    // RMS calc values
    long current_count;
    long current_sum;
    int RMS_Current ;

    // sense board flags
    float GPIO_open_wire[num_boards][10];
    bool overvoltage_flag[18];
    bool undervoltage_flag[18];
} Ev4_t;

#endif