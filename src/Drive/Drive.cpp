#include "Drive.h"

void Drive_State(Ev4_t *ctx, int t, CAN_message_t msg) {
    while (1) {
        ctx->time_buffer[t] = millis() - ctx->start_time;
        if (t % current_interval == 0) {
            measure_current(ctx);
            ctx->current_buffer[int(t / current_interval)] = ctx->current;
        }

        if (t % volt_interval == 0) {
            measure_voltage(ctx);
            Serial.println("New volt");
            for (int i = 0; i < num_boards; i++) {
                for (int j = 0; j < num_cells; j++) {
                    ctx->voltage_buffer[int(t / volt_interval)][i][j] = ctx->cell_voltage[i][j];
                }
            }
        }

        if (t % temp_interval == 0) {
            Serial.println("New Temp");
            measure_temp(ctx);
            for (int i = 0; i < num_boards; i++) {
                for (int j = 0; j < 10; j++) {
                    ctx->temp_buffer[int(t / temp_interval)][i][j] = ctx->cell_temp[i][j];
                }
            }
        }

        if (ctx->new_voltage && ctx->new_temp) {
            print_min_max(ctx);
            Watchdog_Reset(ctx);
        }

        if (t % SD_interval == 0 && !ctx->memory_fault) 
            Sd_DataWrite(ctx);

        if (t % CAN_interval == 0) {
            Serial.println("Send CAN");
            Soc_Update(ctx);
            Can_Tx(ctx);
        }

        // msg = RX_CAN();
        // if(msg.id == INV_TX_ID){
        //   inv_voltage = float(msg.buf[0]*256 + msg.buf[1]);
        // }
        // if(inv_voltage < pack_voltage * 0.5){    //checks inverter voltage
        // to see if tractive system voltage is dropping
        //   mode = "standby";                         //enter standby mode
        //   if ready to drive is exited
        //   //data_file_num = 0;
        //   //check_memory();         //assign a new data file number in
        //   case RTD is entered agian break;
        // }

        while (millis() - ctx->start_time <= ctx->time_buffer[t] + time_step) {} // this needs checked

        if (t < SD_interval - 1)
            t++;
        else
            t = 0;

        println_with_args("t: %d", t);
    }
}