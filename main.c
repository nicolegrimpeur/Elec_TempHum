#define USE_AND_MASKS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <xc.h>
#include "./ressources/general.h"
#include "./ressources/spi.h"
#include "./ressources/SX1272.h"
#include "./ressources/RF_LoRa_868_SO.h"


/*****/
// CONFIG1H
#pragma config FOSC = INTIO7    // Oscillator Selection bits (Internal oscillator block, CLKOUT function on OSC2)
#pragma config PLLCFG = OFF     // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock is always enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = ON      // Power-up Timer Enable bit (Power up timer enaabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<5:0> pins are configured as analog input channels on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTD2   // ECCP2 B output mux bit (P2B is on RD2)
#pragma config MCLRE = EXTMCLR  // MCLR Pin Enable bit (MCLR pin enabled, RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection Block 0 (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection Block 1 (Block 1 (002000-003FFFh) not code-protected)
//#pragma config CP2 = OFF        // Code Protection Block 2 (Block 2 (004000-005FFFh) not code-protected)
//#pragma config CP3 = OFF        // Code Protection Block 3 (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (002000-003FFFh) not write-protected)
//#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (004000-005FFFh) not write-protected)
//#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
//#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
//#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)




int main(int argc, char** argv) {

    uint8_t reg_val;                // when reading SX1272 registers, stores the content (variable read in main and typically  updated by ReadSXRegister function)
    uint8_t RXNumberOfBytes;        // to store the number of bytes received
    uint8_t i;

    InitRFLoRaPins();           // configure pins for RF Solutions LoRa module
    SPIInit();                  // init SPI
    ResetRFModule();            // reset the RF Solutions LoRa module (should be optional since Power On Reset is implemented)


    // put module in LoRa mode (see SX1272 datasheet page 107)

    WriteSXRegister(REG_OP_MODE, FSK_SLEEP_MODE);       // SLEEP mode required first to change bit n�7
    WriteSXRegister(REG_OP_MODE, LORA_SLEEP_MODE);      // switch from FSK mode to LoRa mode
    WriteSXRegister(REG_OP_MODE, LORA_STANDBY_MODE);    // STANDBY mode required fot FIFO loading
    __delay_ms(100);
    GetMode();

    // initialize the module
    InitModule();

    AntennaRX();                // connect antenna to module input

    // for debugging purpose only: check configuration registers content
    //CheckConfiguration();

    // set FIFO_ADDR_PTR to FIFO_RX_BASE_ADDR
    WriteSXRegister(REG_FIFO_ADDR_PTR, ReadSXRegister(REG_FIFO_RX_BASE_ADDR));

    // set mode to LoRa continuous RX
    WriteSXRegister(REG_OP_MODE, LORA_RX_CONTINUOUS_MODE);
    __delay_ms(100);                                    // delay required to start oscillator and PLL
    GetMode();


    forever {

        // wait for valid header reception
        reg_val = ReadSXRegister(REG_IRQ_FLAGS);
        while ((reg_val & 0x10) == 0x00) {                  // check Valid Header flag (bit n�4)
            reg_val = ReadSXRegister(REG_IRQ_FLAGS);
        }

        // --- for debugging: display IRQ flags ---
        reg_val = ReadSXRegister(REG_IRQ_FLAGS);
        // ----------------------------------------

        // wait for end of packet reception
        reg_val = ReadSXRegister(REG_IRQ_FLAGS);
        while ((reg_val & 0x40) == 0x00) {                  // check Packet Reception Complete flag (bit n�6)
            reg_val = ReadSXRegister(REG_IRQ_FLAGS);
        }

        // --- for debugging: display IRQ flags ---
        reg_val = ReadSXRegister(REG_IRQ_FLAGS);
        // ----------------------------------------


        // read received data
        RXNumberOfBytes = ReadSXRegister(REG_RX_NB_BYTES);                              // read how many bytes have been received
        WriteSXRegister(REG_FIFO_ADDR_PTR, ReadSXRegister(REG_FIFO_RX_CURRENT_ADDR));   // to read FIFO at correct location, load REG_FIFO_ADDR_PTR with REG_FIFO_RX_CURRENT_ADDR value
        for (i = 0; i < RXNumberOfBytes; i++) {
            reg_val = ReadSXRegister(REG_FIFO);       // read FIFO
        }

        // display FIFO information
        reg_val = ReadSXRegister(REG_FIFO_RX_CURRENT_ADDR);
        reg_val = ReadSXRegister(REG_RX_NB_BYTES);

        // display RSSI
        reg_val = ReadSXRegister(REG_PKT_RSSI_VALUE);

        // reset all IRQs
        /*    reg_val = ReadSXRegister(REG_IRQ_FLAGS);
            UARTWriteStr("before clear: REG_IRQ_FLAGS = 0x");
            UARTWriteByteHex(reg_val);
        */
        WriteSXRegister(REG_IRQ_FLAGS, 0xFF);           // clear flags: writing 1 clears flag

        /*    // check that flags are actually cleared (useless if not debugging)
            reg_val = ReadSXRegister(REG_IRQ_FLAGS);
            UARTWriteStr("after clear: REG_IRQ_FLAGS = 0x");
            UARTWriteByteHex(reg_val);
         */

    }       // end of loop forever

}