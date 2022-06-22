//
// Created by nico_ on 21/06/2022.
//

#ifndef ELEC_TEMPHUM_X_COMMUNICATION_H
#define ELEC_TEMPHUM_X_COMMUNICATION_H

#include "general.h"

char attenteReception(void);
char attenteReception2(void);
char attenteReception56(void);

void transmission(char*, uint8_t, float);

#endif //ELEC_TEMPHUM_X_COMMUNICATION_H
