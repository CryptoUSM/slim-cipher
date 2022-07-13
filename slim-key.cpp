//
// Created by Chan Yen Yee on 07/06/2022.
//

#include <iostream>
#include <bit>
#include <bitset>
#include "key.h"

uint8_t *randomize_master_key() {
    static uint8_t master_key[20];
    srand((unsigned int) time(NULL));

    for (int j = 0; j < 20; j++) {
        uint8_t cur = rand() % 1000;
        master_key[j] = cur & 0xf;
    }

    return master_key;

}

int circular_shift(uint8_t input, int moves) {
    uint8_t output = 0;
    moves = moves % 4;

    uint8_t output1 = (input << moves);
    uint8_t output2 = (input >> (4 - moves));
    output = output1 | output2;

    return (output & 0xF);
}

uint16_t *key_scheduling(uint8_t input_master_key[20], int round) {
    // starting from msb(left 40bit) towrads lsb(right 40bit), every element is a nibble
    // break the 80bit into 20 4bit nibbles
    uint8_t master_key[20];


    uint8_t lsb[10];
    uint8_t msb[10];
    static uint16_t round_key[16];


    uint8_t *temp;
    temp = input_master_key;

    for (int k = 0; k < 20; k++) {
        master_key[k] = *(temp + k);
//        std::cout<< std::bitset<8>(master_key[k])<< "\n";
    }

    /* nibble index
     * k1- 16, 17, 18, 19
     * k2- 12, 13, 14, 15
     * k3- 8, 9, 10, 11
     * k4- 4, 5, 6, 7
     * k5- 0, 1, 2, 3,
     * */

    int reversed_index = 19; // because master_key has max index=19

    // inject first five round keys
    for (int m = 0; m < 5; m++) {
        round_key[m] = 0;
        for (int n = 0; n < 4; n++) {
            uint8_t base = master_key[reversed_index];
            round_key[m] += (base << (n * 4));
            reversed_index -= 1;

        }
//        std::cout<< std::bitset<20>(round_key[m])<<"---";
//        std::cout<< std::hex<<(round_key[m])<< "\n";
    }


    // for first state lsb and msb
    for (int i = 0; i < 10; i++) {
        msb[i] = master_key[i];
        lsb[i] = master_key[i + 10];

    }

    int slim_sBox[16] = {12, 5, 6, 11, 9, 0, 10, 13, 3, 14, 15, 8, 4, 7, 1, 2};

    uint8_t intermediate_msb_value[10];
    uint8_t intermediate_lsb_value[10]; // sbox value for the msb

    int nibble_index = 9;
    int register_state = 0;

    for (int p = 0; p < (round - 5); p++) {
        uint16_t output = 0;
        uint16_t nibble_output = 0;

        uint8_t temp_round_key[4];
        for (int y = 0; y < 4; y++) {

            if (nibble_index == -1) {
                std::fill_n(lsb, 10, 0);
                std::fill_n(msb, 10, 0);

                //reset lsb msb, spit out the round key
                for (int i = 0; i < 10; i++) {
                    lsb[i] = intermediate_lsb_value[i];
                    msb[i] = intermediate_msb_value[i];
                }

                std::fill_n(intermediate_lsb_value, 10, 0);
                std::fill_n(intermediate_msb_value, 10, 0);


                nibble_index = 9;
                register_state += 1;
            }


            uint8_t shifted_lsb_nibble = circular_shift(lsb[nibble_index], 2);
            uint8_t shifted_msb_nibble = circular_shift(msb[nibble_index], 3);

            // important, serve as one of the new key's nibble
            uint8_t sbox_lsb_msb = slim_sBox[int(shifted_lsb_nibble xor msb[nibble_index])];
            intermediate_lsb_value[nibble_index] = sbox_lsb_msb;

            nibble_output = (shifted_msb_nibble xor sbox_lsb_msb);
            intermediate_msb_value[nibble_index] = nibble_output;

            output += (nibble_output << (4 * y));

            nibble_index -= 1;

        }
        round_key[int(p + 5)] = 0;
        round_key[int(p + 5)] = output;

        output = 0;

    }

    return round_key;

}
