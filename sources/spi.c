//
// Created by nico_ on 07/06/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xc.h>
#include "../ressources/general.h"
#include "../ressources/spi.h"

void SPIInit(void) {

    SPI_SCK_DIR = OUTP;                     // SCK = output
    ANSELDbits.ANSD0 = DISABLE;             // disable analog input for SCK
    SPI_MISO_DIR = INP;                     // SDI = input
    ANSELDbits.ANSD1 = DISABLE;             // disable analog input for MISO
    SPI_MOSI_DIR = OUTP;                    // MOSI

    SPI_SS_DIR = OUTP;                      // SS = output
    SPI_SS_LAT = SPI_SS_DISABLE;            // disable SS (SS = High)

    SSP2STATbits.SMP = CLEAR;               // SMP = 0 (input sampled at middle of output)
    SSP2STATbits.CKE = SET;                 // CKE = 1 (transmit occurs from active (1) to idle (0) SCK state, and sampling occurs from idle to active state)
    SSP2CON1 = 0b00100000;                  // WCOL not used in SPI mode, SSPOV not used in SPI master mode
    // SSPEN set (enable serial port), CKP cleared (idle state is low level)
    // SSP2CON1<3:0> = 0000 (clock = FOSC / 4) = 250kHz
}


void SPITransfer(uint8_t data_out) {        // Warning: Slave Select is not managed in this function
    // don't forget to control SS before and after calling this function
    uint8_t dummy_byte;
    dummy_byte = SSP2BUF;                   // clear BF (Buffer Full Status bit)
    PIR3bits.SSP2IF = CLEAR;                // clear interrupt flag
    SSP2BUF = data_out;                     // transmit data
    while (!PIR3bits.SSP2IF);               // wait until transmission is complete
}

uint8_t SPIReceive(uint8_t data_out) {      // Warning: Slave Select is not managed in this function
    // don't forget to control SS before and after calling this function
    uint8_t data_in, dummy_byte;
    dummy_byte = SSP2BUF;                   // clear BF (Buffer Full Status bit)
    PIR3bits.SSP2IF = CLEAR;                // clear interrupt flag
    SSP2BUF = data_out;                     // transmit data
    while (!PIR3bits.SSP2IF);               // wait until transmission is complete
    data_in = SSP2BUF;                      // store received data
    return(data_in);
}