//
// Created by nico_ on 21/06/2022.
//

#include "../ressources/switch.h"
#include "../ressources/SX1272.h"
#include "../ressources/RF_LoRa_868_SO.h"

void passageEcoute(void) {
    AntennaRX();                // connect antenna to module input

    // set FIFO_ADDR_PTR to FIFO_RX_BASE_ADDR
    WriteSXRegister(REG_FIFO_ADDR_PTR, ReadSXRegister(REG_FIFO_RX_BASE_ADDR));

    // set mode to LoRa continuous RX
    WriteSXRegister(REG_OP_MODE, LORA_RX_CONTINUOUS_MODE);
}