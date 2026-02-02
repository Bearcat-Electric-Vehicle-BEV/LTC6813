#ifndef CAN_H
#define CAN_H

#include <FlexCAN_T4.h>
#include "../Ev4/Ev4.h"
#include "../Utils/Utils.h"
#include "../CONFIGURE.h"
#include "../Measurement/Measurement.h"

void Can_Init(Ev4_t *ctx);
CAN_message_t Can_Rx(Ev4_t *ctx);
void Can_Tx(Ev4_t *ctx);

#endif