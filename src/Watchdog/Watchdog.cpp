#include "Watchdog.h"

static Ev4_t *wdt_ctx = NULL;

void Watchdog_Init(Ev4_t *ctx) {
    if (watchdog_timeout != 0) { // callback function is having some issues
        WDT_timings_t config;
        int watchdog_trigger = watchdog_timeout - 1;
        if (watchdog_trigger < 1) 
            watchdog_trigger = 1;

        config.trigger = 11; /* in seconds, 0->128 */ // time until watchdog callback function is triggered.
        config.timeout = watchdog_timeout; /* in seconds, 0->128 */ // time until watchdog reset
        config.pin = SC; // pin to be driven low upon reset. WDT1 holds low, WDT2 pulses low
        config.callback = Watchdog_CallbackWrapper;
        ctx->wdt.begin(config);
    }
}

static void Watchdog_CallbackWrapper() {
    if (wdt_ctx)
        Watchdog_Callback(wdt_ctx);
}

void Watchdog_Callback(Ev4_t *ctx) {
    Serial.println("Callback called");
    measure_voltage(ctx);
    measure_temp(ctx);
    Watchdog_Reset(ctx);
    ctx->watchdog_callback = true; // set watchdog callback flag
}

bool Watchdog_Reset(Ev4_t *ctx) { // this needs to clear the voltage and temperature measurements after reading them
    ctx->new_voltage = false;
    ctx->new_temp = false;

    for (int i = 0; i < num_boards; i++) {
        for (int j = 0; j < num_cells; j++) {
            if (ctx->cell_voltage[i][j] < OV && ctx->cell_voltage[i][j] > UV) {
                ctx->cell_voltage[i][j] = 0;
                continue;
            } else {
                digitalWrite(20, LOW);
                delay(1000); // delay to overcome debounce of shutdown circuit
                println_with_args("Invalid voltage: %f", ctx->cell_voltage[i][j]);
                return false;
            }
        }
    }

    for (int i = 0; i < num_boards; i++) {
        for (int j = 0; j < 10; j++) {
            if (ctx->cell_temp[i][j] > min_temp && ctx->cell_temp[i][j] < max_temp) {
                ctx->cell_temp[i][j] = min_temp;
                continue;
            } else {
                digitalWrite(SC, LOW);
                delay(1000); // delay to overcome debounce of shutdown circuit
                println_with_args("Invalid temp -> Board: %d | Num: %d", i + 1, j + 1);
                return false;
            }
        }
    }

    digitalWrite(20, HIGH);
    ctx->wdt.feed();
    // Serial.println("Watchdog fed");
    return true;
}