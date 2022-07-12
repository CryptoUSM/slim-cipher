//
// Created by Chan Yen Yee on 13/06/2022.
//

#ifndef SLIMDDT_CPP_KEY_SCHEDULING_H
#define SLIMDDT_CPP_KEY_SCHEDULING_H

#include <cstdint>

uint16_t * key_scheduling(uint8_t input_master_key[20], int round);
uint8_t * randomize_master_key();
#endif //SLIMDDT_CPP_KEY_SCHEDULING_H


#ifndef SLIM_H
#define SLIM_H

#include <cstdint>

uint32_t encrypt(uint32_t plaintext, int round, uint16_t *round_keys);
uint32_t one_round_decrypt(uint32_t plaintext, u_int16_t subkey);
uint32_t one_round_encrypt(uint32_t ciphertext, u_int16_t subkey);
uint16_t substitution(uint16_t x);
uint16_t permutation(uint16_t x);


#endif //SLIM_H


#ifndef ATTACK_H
#define ATTACK_H

#include <cstdint>

int attack(std::string run);

#endif //ATTACK_H