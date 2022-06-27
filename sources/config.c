//
// Created by nico_ on 21/06/2022.
//

#include "../ressources/include.h"
#include "../ressources/config.h"
#include "../ressources/spi.h"
#include "../ressources/RF_LoRa_868_SO.h"
#include "../ressources/i2c.h"
#include "../ressources/SX1272.h"
#include "../ressources/switch.h"

void initCode(void) {
    InitRFLoRaPins();           // configure pins for RF Solutions LoRa module
    SPIInit();                  // init SPI
    ResetRFModule();            // reset the RF Solutions LoRa module (should be optional since Power On Reset is implemented)
    i2c_init();                             // configuration de l'interface I2C

    // config ADC pour mesurer la batterie restante
    ADCON1 = 0b00000000;
    ADCON2 = 0b00111110;
    ADCON0 = 0b00000001;

    LED_DIR = OUTP;

    // put module in LoRa mode (see SX1272 datasheet page 107)

    WriteSXRegister(REG_OP_MODE, FSK_SLEEP_MODE);       // SLEEP mode required first to change bit n�7
    WriteSXRegister(REG_OP_MODE, LORA_SLEEP_MODE);      // switch from FSK mode to LoRa mode
    WriteSXRegister(REG_OP_MODE, LORA_STANDBY_MODE);    // STANDBY mode required fot FIFO loading
    __delay_ms(100);

    // initialize the module
    InitModule();

    RCONbits.IPEN = SET;
    INTCON = 0b11010000;    // autorise les interruptions
//    INTCONbits.GIE = CLEAR;
    
    passageEcoute();
}