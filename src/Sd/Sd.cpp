#include "Sd.h"

void Sd_DataWrite(Ev4_t *ctx) {
    String filename = "data" + String(ctx->data_file_num) + ".csv";
    File dataFile = SD.open(filename.c_str(), FILE_WRITE);
    if (dataFile) {
        dataFile.print("Mode: ");
        dataFile.println(ctx->mode);
        for (int n = 0; n < SD_interval; n++) {
            dataFile.print("Voltage:\n");
            if (n % volt_interval == 0 || ctx->mode != Drive) {
                for (int i = 0; i < num_boards; i++) {
                    for (int j = 0; j < num_cells; j++) {
                        if (ctx->mode == Drive)
                            dataFile.print(ctx->voltage_buffer[int(n / volt_interval)][i][j], 4);
                        else
                            dataFile.print(ctx->cell_voltage[i][j], 4);
                        dataFile.print(", ");
                    }
                    dataFile.print("\n");
                }
            }

            if (n % temp_interval == 0 || ctx->mode != Drive) {
                dataFile.print("\nTemperature:\n");
                for (int i = 0; i < num_boards; i++) {
                    for (int j = 0; j < 10; j++) {
                        if (ctx->mode == Drive)
                            dataFile.print(ctx->temp_buffer[int(n / temp_interval)][i][j], 2);
                        else
                            dataFile.print(ctx->cell_temp[i][j], 2);
                        dataFile.print(", ");
                    }
                    dataFile.print("\n");
                }
            }

            if (n % current_interval == 0 || ctx->mode != Drive) {
                dataFile.print("Current: ");
                if (ctx->mode == Drive)
                    ctx->currentbuffer_stat = ctx->current_buffer[int(n / current_interval)];
                dataFile.print(ctx->currentbuffer_stat);
            } else {
                dataFile.print(ctx->current);
                dataFile.print("\n");
            }
            dataFile.print("Time:\n");

            // time stamp
            if (ctx->mode == Drive)
                dataFile.println(ctx->time_buffer[n]);
            else
                dataFile.println(millis() - ctx->start_time);
            break;
        }
        dataFile.close();
    }
}