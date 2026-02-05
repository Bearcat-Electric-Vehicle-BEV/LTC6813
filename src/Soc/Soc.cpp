#include "Soc.h"

void Soc_Save(Ev4_t *ctx) {
    if (SD.exists("state.txt")) {
        File file = SD.open("state.txt", FILE_READ);
        if (file) {
            String line;
            while (file.available()) {
                line = file.readStringUntil('\n');
                Serial.println(line);
                int delim_index = line.indexOf(':');
                String name = line.substring(0, delim_index);
                String value = line.substring(delim_index + 1);
                map_text2var(ctx, name, value);
            }
            file.close();
        }
    } else {
        File file = SD.open("state.txt", FILE_WRITE);
        Serial.println("Initializing state of charge to 100%");
        file.close();
        Serial.println("state.txt initialized.");
    }
}

void Soc_Update(Ev4_t *ctx) {
    const int discharge_curve_length = sizeof(discharge_points) / sizeof(discharge_points[0]); // length of each discharge curve
    const float max_capacity = discharge_points[0]; // maximum capacity of a single cell
    // const int num_current_curves = sizeof(discharge_currents) / sizeof(discharge_currents[0]);  // number of discharge curves @ different currents
    float min_OC_cell_voltage = ctx->open_circuit_voltage[0][0]; // funct. min_max requires that the min and
                                                            // max values are initalized within the
                                                            // range of the min max values
    float max_OC_cell_voltage = ctx->open_circuit_voltage[0][0];
    min_max<num_boards, num_cells>(ctx->open_circuit_voltage, min_OC_cell_voltage, max_OC_cell_voltage);
    float discharged = interpolate<discharge_curve_length>( // capacity which has already been discharged (mAh)
        discharge_curves[0], 
        discharge_points, 
        min_OC_cell_voltage);
    ctx->soc = 100 - ((max_capacity - discharged) / max_capacity * 100);

    println_with_args("SOC: %f", ctx->soc);
}

float Soc_Get(Ev4_t *ctx) {
    Soc_Update(ctx);
    return ctx->soc;
}