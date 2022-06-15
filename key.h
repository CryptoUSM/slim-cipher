//
// Created by Chan Yen Yee on 13/06/2022.
//

#ifndef SLIMDDT_CPP_KEY_SCHEDULING_H
#define SLIMDDT_CPP_KEY_SCHEDULING_H

#include <cstdint>

uint16_t * key_scheduling(uint8_t input_master_key[20], int round);

#endif //SLIMDDT_CPP_KEY_SCHEDULING_H


#ifndef SLIM_H
#define SLIM_H

#include <cstdint>

uint32_t encrypt(uint32_t plaintext);

#endif //SLIM_H


