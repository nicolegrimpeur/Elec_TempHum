//
// Created by nico_ on 07/06/2022.
//

#ifndef ELECTEMP_HUM_SPI_H
#define ELECTEMP_HUM_SPI_H

#include "general.h"
#include <stdint.h>             // with this inclusion, the XC compiler will recognize standard types such as uint8_t or int16_t
// (so, their definition in "general.h" is useless)

// PIC18F46K22 SPI master mode
// for MSSP n�2:    SCK is D0
//                  MISO is D1
//                  MOSI is D4
//
#define SPI_SCK_DIR             TRISBbits.TRISB2
#define SPI_MISO_DIR            TRISBbits.TRISB3
#define SPI_MOSI_DIR            TRISBbits.TRISB4
//
// Slave Select is wired on E0
//
#define SPI_SS_DIR              TRISBbits.TRISB5
#define SPI_SS_LAT              LATBbits.LATB5

#define SPI_SS_DISABLE          OUTP_HIGH
#define SPI_SS_ENABLE           OUTP_LOW


void SPIInit(void);                                                         // init SPI in master mode
void SPITransfer (uint8_t data_out);                                        // send a byte
uint8_t SPIReceive (uint8_t data_out);                                      // receive a byte and send another byte

#endif //ELECTEMP_HUM_SPI_H
