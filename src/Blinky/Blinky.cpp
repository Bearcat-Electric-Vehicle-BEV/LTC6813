#include "Blinky.h"
#include <CONFIGURE.h>
#include <Balance/Balance.h>

void flash_leds() {
    int time_on = 1000; // Time each led is on in milliseconds
    bool discharge[num_boards][18] = {0}; // '1': needs discharged, '0': does not need discharged
    for (int i = num_boards - 1; i >= 0; i--) {
        if (i % 4 < 2) {
            for (int j = 0; j < num_cells; j++) {
                discharge[i][j] = true;
                discharge_cells(discharge);
                delay(time_on);
                discharge[i][j] = false;
                discharge_cells(discharge);
            }
        } else {
            for (int j = num_cells; j >= 0; j--) {
                discharge[i][j] = true;
                discharge_cells(discharge);
                delay(time_on);
                discharge[i][j] = false;
                discharge_cells(discharge);
            }
        }
    }
}