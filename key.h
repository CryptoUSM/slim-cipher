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

uint32_t encrypt(uint32_t plaintext, int round);
uint32_t decrypt(uint32_t plaintext, u_int16_t subkey);

uint16_t substitution(uint16_t x);
uint16_t permutation(uint16_t x);

#endif //SLIM_H


