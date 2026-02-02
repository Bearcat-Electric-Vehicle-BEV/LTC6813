#ifndef CAN_H
#define CAN_H

#include <CONFIGURE.h>
#include <HEADER.h>
#include <FlexCAN_T4.h>
#include "../Ev4/Ev4.h"
#include "../Measurement/Measurement.h"


// CAN pins
#define CRX3        23
#define CTX3        22
#define STBY        21 // CAN Transceiver Standby

// CAN Bus Parameters
const uint16_t BMS_ID = 0x123;                // standard ID of BMS TX messages
const uint32_t INV_TX_ID = 0x0A7;             // CAN Message ID of message send from inverter of DC Bus Voltage (100 Hz frequency).
const uint32_t CHG_TX_ID = 0x18FF50E5;        // CAN Message ID of messages sent from charger

void Can_Init(Ev4_t *ctx);
CAN_message_t Can_Rx(Ev4_t *ctx);
void Can_Tx(Ev4_t *ctx);

#endif