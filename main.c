#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#define USE_AND_MASKS

#include "./ressources/include.h"
#include "./ressources/general.h"
#include "./ressources/spi.h"
#include "./ressources/SX1272.h"
#include "./ressources/RF_LoRa_868_SO.h"
#include "./ressources/i2c.h"
#include "./ressources/switch.h"
#include "./ressources/communication.h"
#include "./ressources/config.h"
 

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
#pragma config CP2 = OFF        // Code Protection Block 2 (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection Block 3 (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)


void __interrupt()ISR_interrupt(void) {
    uint8_t reg_val;

    reg_val = ReadSXRegister(REG_IRQ_FLAGS);

    if ((reg_val & 0x10) != 0x00) {
        WriteSXRegister(REG_FIFO_ADDR_PTR, ReadSXRegister(REG_FIFO_RX_CURRENT_ADDR));

        char rxBuffer[6];
        for (int i = 0; i < 5; ++i) {
            rxBuffer[i] = ReadSXRegister(REG_FIFO);
        }

        if (rxBuffer[3] == 7 && rxBuffer[4] == 1) {
            LED = SET;

            char etape1[6];
            etape1[0] = 0xAD;
            etape1[1] = 0x4E;
            etape1[2] = 0x01;
            etape1[3] = 0x07;
            etape1[4] = 0x07;
            etape1[5] = 10;

            transmission(etape1, sizeof etape1, -1);

            attenteReception();

            char etape2[5];
            etape2[0] = 0xAD;
            etape2[1] = 0x4E;
            etape2[2] = 0x01;
            etape2[3] = 0x07;
            etape2[4] = 0x03;

            transmission(etape2, sizeof etape2, -1);

            LED = CLEAR;


            ADCON0bits.GODONE = 1;         // start conversion
            while (ADCON0bits.GODONE);      // wait until conversion is finished
            float tension = ADRESL;
            tension = tension * 2;

            //////////// calcul buffer à envoyer
            int octet1 = 0b0000000; // humidite
            int octet2 = 0b0000000;
            int octet3 = 0b0000000; // temperature
            int octet4 = 0b0000000;

            int masque = 0b00111111;
            int octet1WithoutStatus = masque & octet1;

            float humidite = (float) (octet1WithoutStatus * 256 + octet2) / (16384 - 2);

            octet4 = octet4 >> 2;

            float temperature = (float) ((octet3 * 64 + octet4) * 165) / (16384 - 2) - 40;

            char tabTemp[7];
            snprintf(tabTemp, sizeof tabTemp, "%f", temperature);
            char tabHum[4];
            snprintf(tabHum, sizeof tabHum, "%f", humidite);

            char txBuffer[14];
            txBuffer[0] = 0xAD;
            txBuffer[1] = 0x4E;
            txBuffer[2] = 0x01;
            txBuffer[3] = 0x07;
            strncat(txBuffer, tabHum, 4);
            strncat(txBuffer, tabTemp, 7);
            ////////////////// fin calcul buffer

            transmission(txBuffer, 14, tension);

            uint8_t codeMessage = attenteReception56();

            if (codeMessage == 6)
                transmission(txBuffer, 14, tension);
        }
        LED = CLEAR;
    }
    
    WriteSXRegister(REG_IRQ_FLAGS, 0xFF);           // clear flags: writing 1 clears flag
    INTCONbits.INT0IF = CLEAR;     // flag d'interruption efface
    
    SLEEP();
}

int main(int argc, char **argv) {
    initCode();

    LED = CLEAR;

    SLEEP();

    return 0;
}

#pragma clang diagnostic pop