//
// Created by nico_ on 21/06/2022.
//

#include "../ressources/include.h"
#include "../ressources/communication.h"
#include "../ressources/SX1272.h"
#include "../ressources/RF_LoRa_868_SO.h"
#include "../ressources/switch.h"

char attenteReception(void) {
    char rxBuffer[6];
    uint8_t reg_val;                // when reading SX1272 registers, stores the content (variable read in main and typically  updated by ReadSXRegister function)

    do {
        // wait for valid header reception
        do {
            reg_val = ReadSXRegister(REG_IRQ_FLAGS);
        } while ((reg_val & 0x10) == 0x00);

//        taillePayload = ReadSXRegister(REG_RX_NB_BYTES);

        WriteSXRegister(REG_FIFO_ADDR_PTR, ReadSXRegister(REG_FIFO_RX_CURRENT_ADDR));

        for (int i = 0; i < 5; ++i) {
            rxBuffer[i] = ReadSXRegister(REG_FIFO);
        }
    } while (rxBuffer[3] != 7 || (rxBuffer[4] != 2 && rxBuffer[4] != 5 && rxBuffer[4] != 6));

    return rxBuffer[4];
}

void transmission(char *txBuffer, uint8_t payload, float tension) {
    uint8_t reg_val;                // when reading SX1272 registers, stores the content (variable read in main and typically  updated by ReadSXRegister function)

    AntennaTX();

    // FifiAddrPtr takes value of FifoTxBaseAddr
    WriteSXRegister(REG_FIFO_ADDR_PTR, ReadSXRegister(REG_FIFO_TX_BASE_ADDR));
    // set the number of bytes to transmit (PAYLOAD_LENGTH is defined in RF_LoRa868_SO.h)
    WriteSXRegister(REG_PAYLOAD_LENGTH_LORA, payload);
    
    for (int i = 0; i < strlen(txBuffer); i++) {
        // donnée du capteur transformé
        WriteSXRegister(REG_FIFO, txBuffer[i]);         // load FIFO with data to transmit
    }

    //uint8_t tensionInt = (uint8_t) tension * 2;
    
    if (tension != -1)
        WriteSXRegister(REG_FIFO, tension);         // load FIFO with data to transmit

    // set mode to LoRa TX
    WriteSXRegister(REG_OP_MODE, LORA_TX_MODE);
    __delay_ms(100);                                    // delay required to start oscillator and PLL

    // wait end of transmission
    do {
        reg_val = ReadSXRegister(REG_IRQ_FLAGS); // wait for end of transmission (wait until TxDone is set)
    } while ((reg_val & 0x08) == 0x00);

    // delay is required before checking mode: it takes some time to go from TX mode to STDBY mode
    __delay_ms(200);

    // reset all IRQs
    WriteSXRegister(REG_IRQ_FLAGS, 0xFF);           // clear flags: writing 1 clears flag

    passageEcoute();
}