#include "Can.h"

void Can_Init(Ev4_t* ctx) {
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can = ctx->can;
    pinMode(CRX3, INPUT);
    pinMode(CTX3, OUTPUT);
    pinMode(STBY, OUTPUT);
    can.begin();
    can.setBaudRate(250000);
    can.setMaxMB(3); // number of CAN message mailboxes
    digitalWrite(STBY, LOW);
    // https://github.com/tonton81/FlexCAN_T4/blob/master/examples/mailbox_filtering_example_with_interrupts/mailbox_filtering_example_with_interrupts.ino
    // Mailboxes must be configured for all messages - both TX and RX
    can.setMB((FLEXCAN_MAILBOX)0, RX, STD); // Standard mailbox for Inverter ID
    can.setMB((FLEXCAN_MAILBOX)1, RX, EXT); // Extended id for charger
    can.setMB((FLEXCAN_MAILBOX)2, TX, EXT); // BMS TX -> charger id
    can.setMBFilter(MB0, INV_TX_ID);        // Mailbox for Inverter CAN messages
    can.setMBFilter(MB1, CHG_TX_ID);        // Mailbox for Charger CAN Messages
    can.setMBFilter(MB2, 0x1806E5F4);       // Mailbox for Charger CAN Messages
}

CAN_message_t Can_Rx(Ev4_t *ctx) { // grabs the first message in the FIFO.
    // left bit in charger flag is highest bit (bit 4)
    CAN_message_t msg = {};
    digitalWrite(STBY, LOW);
    // bool received = false;
    ctx->can.read(msg);
    // can.readMB(msg);
    if (msg.id != 0 && debug) {
        Serial.print("ID: ");
        Serial.print(msg.id, HEX);
        Serial.println(" Data: ");
        // msg.len = 8;
        for (int i = 0; i < msg.len; i++) {
            Serial.print(msg.buf[i], BIN);
            Serial.print(" ");
        }
        Serial.print('\n');
    }
    return msg; // always check the ID of the returned message. No messages in buffer returns 0 ID with 8 byte of zero data
}

void Can_Tx(Ev4_t *ctx) {
    measure_voltage(ctx);
    measure_temp(ctx);
    float min_cell_voltage = ctx->cell_voltage[0][0];
    float max_cell_voltage = ctx->cell_voltage[0][0];
    float min_cell_temp = ctx->cell_temp[0][0];
    float max_cell_temp = ctx->cell_temp[0][0];
    min_max<num_boards, num_cells>(ctx->cell_voltage, min_cell_voltage, max_cell_voltage);
    min_max<num_boards, 10>(ctx->cell_temp, min_cell_temp, max_cell_temp);
    uint8_t inst_power_limit = power_limit(max_cell_temp);
    println_with_args("Power Limit: %u", inst_power_limit);

    digitalWrite(STBY, LOW);
    digitalWrite(CTX3, HIGH);
    delay(1);

    CAN_message_t BMS_data;
    // BMS_data.id = BMS_ID;
    BMS_data.id = 0x00000007;
    BMS_data.flags.extended = 0;
    BMS_data.len = 8; // Set the data length

    BMS_data.buf[0] = float_2_uint8_t(ctx->soc, 0, 100);                // SOC
    BMS_data.buf[1] = float_2_uint8_t(ctx->currentbuffer_stat, 0, 200); // current
    BMS_data.buf[2] = float_2_uint8_t(max_cell_voltage, 0, 5);     // max cell
    BMS_data.buf[3] = float_2_uint8_t(max_cell_temp, 0, 150);      // max cell temp
    BMS_data.buf[4] = float_2_uint8_t(min_cell_voltage, 0, 5);     // min cell voltage
    BMS_data.buf[5] = float_2_uint8_t(max_cell_temp, 0, 150);      // min cell temp
    BMS_data.buf[6] = inst_power_limit;                            // BMS Suggested Power Limit
    BMS_data.buf[7] = 0;

    if (ctx->can.write(BMS_data))
        Serial.println("CAN message sent 2");
    else
        Serial.println("CAN message TX Failed");

    digitalWrite(CTX3, LOW);
}