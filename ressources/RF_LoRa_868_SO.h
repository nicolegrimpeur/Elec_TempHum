//
// Created by nico_ on 07/06/2022.
//

#ifndef ELECTEMP_HUM_RF_LORA_868_SO_H
#define ELECTEMP_HUM_RF_LORA_868_SO_H

// RF Solutions RF-LoRa-868-SO module //

#include "general.h"
#include <stdint.h>             // with this inclusion, the XC compiler will recognize standard types such as uint8_t or int16_t
// (so, their definition in "general.h" is useless)


#define RF_RXpin     LATDbits.LATD7                  // antenna switch RX control pin
#define RF_TXpin     LATDbits.LATD6                  // antenna switch TX control pin
#define RF_RESETpin  LATBbits.LATB1                  // Reset input

#define RF_RXpin_DIR       TRISDbits.TRISD7          // direction bit for RX control line
// attention: si on met en sortie la patte B5 au lieu de la patte B4,
// la tension � l'�tat haut sur la sortie TX (de l'UART) ne monte plus qu'� 500 mV au lieu de monter � Vcc
// ??????????????????? myst�re ?????????????????
// probl�me sur le demo board ?????????
#define RF_TXpin_DIR       TRISDbits.TRISD6          // direction bit for TX control line
#define RF_RESETpin_DIR    TRISBbits.TRISB1          // direction bit for Reset

#define PAYLOAD_LENGTH 6                             // for transmission: number of bytes to transmit
// (this value will be stored before transmission in REG_PAYLOAD_LENGTH_LORA register of SX1272 chip)
#define POUT 14                                      // output power (in dBm)
// (used to compute the data to store in REG_PA_CONFIG register during configuration of SX1272 chip)

void InitRFLoRaPins(void);                          // configure pins for RF Solutions module
void ResetRFModule(void);                           // configure pins for RF Solutions module
void AntennaTX(void);                               // connect antenna to module output
void AntennaRX(void);                               // connect antenna to module input

#endif //ELECTEMP_HUM_RF_LORA_868_SO_H
