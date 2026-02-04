#include "Charge.h"

void Charge_EnableCharger(Ev4_t *ctx, bool enable) {
    digitalWrite(STBY, LOW);
    digitalWrite(CTX3, HIGH);
    delay(1);
    CAN_message_t CHGR_EN;
    // CHGR_EN.id = 0x1806E5F4;  // Set the CAN message ID     //datasheet
    CHGR_EN.id = 0x1806E5F4; // Set the CAN message ID     //datasheet
    // CHGR_EN.id = 0x18FF50E5;  //charger send can id?
    CHGR_EN.flags.extended = 1;
    CHGR_EN.len = 8; // Set the data length
    // 7FF max CAN ID

    uint16_t voltage_int = (uint16_t)(CHG_voltage * 10);
    uint16_t current_int = (uint16_t)(CHG_current * 10);

    CHGR_EN.buf[0] = (uint8_t)(voltage_int >> 8); // High byte
    CHGR_EN.buf[1] = (uint8_t)(voltage_int);      // Low byte
    CHGR_EN.buf[2] = (uint8_t)(current_int >> 8); // High byte
    CHGR_EN.buf[3] = (uint8_t)(current_int);      // Low byte
    CHGR_EN.buf[4] = (uint8_t)(enable);
    CHGR_EN.buf[5] = 0;
    CHGR_EN.buf[6] = 0;
    CHGR_EN.buf[7] = 0;

    bool message_sent = ctx->can.write(CHGR_EN);
    for (int i = 0; i < CHGR_EN.len; i++) {
        Serial.print(CHGR_EN.buf[i], BIN);
        Serial.print(" ");
    }

    digitalWrite(CTX3, LOW);

    if (!message_sent)
    {
    }
}

void Charge_Precharge(Ev4_t *ctx, CAN_message_t msg, float charger_voltage, float charger_current) {
    while (1) {
        measure_voltage(ctx);
        measure_temp(ctx);
        Watchdog_Reset(ctx);
        Charge_EnableCharger(ctx, true); // send charge-disable message and clear comm fault on charger
        msg = Can_Rx(ctx);
        charger_voltage = ((uint16_t)msg.buf[0] << 8 | (uint16_t)msg.buf[1]) / 10;
        charger_current = ((uint16_t)msg.buf[2] << 8 | (uint16_t)msg.buf[3]) / 10;
        Serial.println(charger_voltage);
        Serial.println(ctx->pack_voltage);

        if ((msg.id == CHG_TX_ID && msg.buf[4] == 0) || true) { // && charger_voltage >= pack_voltage * 0.80) {  // if can id
                                                                // matches charger AND there are no charger faults AND precharge is complete
            break;
        }
    }
}

void Charge_State(Ev4_t *ctx, CAN_message_t msg, float charger_voltage, float charger_current, uint32_t charge_start_time) {
    while (1) { // charge cycle
        println_with_args("Time: %f minutes", (float)(millis() - charge_start_time) / 60000);
        println_with_args("Charge fault status: %f", ctx->charger_fault);

        measure_voltage(ctx);
        measure_temp(ctx);
        measure_current(ctx);

        println_with_args("Current: %f", ctx->current);
        println_with_args("Pack Voltage: %f", ctx->pack_voltage);
        print_min_max(ctx);

        if (!ctx->memory_fault) 
            Sd_DataWrite(ctx);

        if (Watchdog_Reset(ctx)) {
            msg = Can_Rx(ctx);
            charger_voltage = ((uint16_t)msg.buf[0] << 8 | (uint16_t)msg.buf[1]) / 10;
            charger_current = ((uint16_t)msg.buf[2] << 8 | (uint16_t)msg.buf[3]) / 10;
            println_with_args("Charger voltage: %f", charger_voltage);
            println_with_args("Charger current: %f", charger_current);

            if ((msg.id == CHG_TX_ID && msg.buf[4] == 0) || true) {
                Charge_EnableCharger(ctx, false);
            } else { // charger error
                digitalWrite(CS, LOW);
                Charge_EnableCharger(ctx, false);
                Serial.println("Charger Error");
                ctx->charger_fault = 1;
            }
        }

        delay(1000);
    }
}