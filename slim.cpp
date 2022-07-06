//
// Created by Chan Yen Yee on 07/06/2022.
//

#include <iostream>
#include <bit>
#include <bitset>
#include "key.h"

int pBox[16] = {7, 13, 1, 8, 11, 14, 2, 5, 4, 10, 15, 0, 3, 6, 9, 12};
int sBox[16] = {12, 5, 6, 11, 9, 0, 10, 13, 3, 14, 15, 8, 4, 7, 1, 2};

uint16_t substitution(uint16_t x) {

    uint16_t total = 0;

    for (int i = 0; i < 4; i++) {
        int last_digit = x & 0xf;
        x = (x >> 4);
        uint16_t sub_x = sBox[last_digit];

        // sub_x << 4*(i) meaning add the proper digits for the sub_x before adding into total
        total += (sub_x << 4 * (i));
    }

    return total;
}

uint16_t permutation(uint16_t x) {

    uint16_t total = 0;

    for (int i = 0; i < 16; i++) { // 16 bit right halves
        uint16_t remainder = x % 2;
        int shift_bit = pBox[i]; // when you are at last bit, meaning your only choice is moving forward--> shift left
        total += remainder << shift_bit; // therefore the shift bit is just how many bits you should shift left
        x = x >> 1; // shift the next bits
    }
    return total;
}

uint32_t one_round_decrypt(uint32_t ciphertext, uint16_t subkey ) {
    uint16_t l16 = ((ciphertext >> 16) & 0xffff);
    uint16_t r16 = ciphertext & 0xffff;

    // to retrieve back the value after F funxtion
    uint16_t fBox_l16 =permutation(substitution(( l16 xor subkey)));
    uint16_t rev_xor= fBox_l16 xor r16; // left side

    uint32_t plaintext= (rev_xor<<16)| l16;

//    std::cout<< std::hex<< "final: "<< plaintext << "\n";

    return plaintext;

}

uint32_t one_round_encrypt(uint32_t plaintext, uint16_t subkey ) {
    uint16_t l16 = ((plaintext >> 16) & 0xffff);
    uint16_t r16 = plaintext & 0xffff;

    uint16_t keyed_r16 = r16 xor subkey; // r16_x1
    uint16_t sub_r16 = substitution(keyed_r16);
    uint16_t per_r16 = permutation(sub_r16);

    uint16_t next_r16 = l16 xor per_r16;
    l16 = r16;
    r16 = next_r16;

//    std::cout<< std::hex<< "final: "<< plaintext << "\n";

    uint32_t ciphertext = (l16 << 16) + r16;

    return ciphertext;

}

uint32_t encrypt(uint32_t plaintext, int round, uint16_t *round_keys) { //start from round 0

//    uint32_t plaintext = 0x4356cded;
    uint32_t ciphertext = 0;

    uint16_t l16 = ((plaintext >> 16) & 0xffff);
    uint16_t r16 = plaintext & 0xffff; // to get the original number--> x AND F= x cuz F==1111 in bin
//    std::cout << "l16, r16 " << std::hex << l16 << " " << std::hex << r16 << "\n";

    uint16_t round_key[round];

    for (int i = 0; i < round; i++) {
        round_key[i] = 0;
        round_key[i] = *(round_keys + int(i));
    }

//    std::cout << "round_key 13R: " << std::hex << round_key[12] << std::endl;


    /*
     * keyed_r16= output of right halves input xor with round key
     * sub_r16= output of right halves input after sbox
     * per_r16= output of right halves input after pbox
     * round_r16= variable to preserve original value of r16 for the current round
     * */

    for (int r = 0; r < round; r++) {
        uint16_t keyed_r16 = r16 xor round_key[r]; // r16_x1
        uint16_t sub_r16 = substitution(keyed_r16);
        uint16_t per_r16 = permutation(sub_r16);

//        std::cout << "==============Round " << int(r) <<  ": " << "====================" << std::endl;
//        std::cout << "r16: " << std::hex << r16 << std::endl;
//        std::cout << "round_key: " << std::hex << round_key[r] << std::endl;
//        std::cout << "xor r16 with round_key: " << std::hex << keyed_r16 << std::endl;
//        std::cout << "sub_r16: " << std::hex << sub_r16 << std::endl;
//        std::cout << "per_r16: " << std::hex << per_r16 << std::endl;

        // switch position
        uint16_t next_r16 = l16 xor per_r16;
        l16 = r16;
        r16 = next_r16;

//        std::cout << "next r16: " << std::hex << r16 << std::endl;
//        std::cout << "next l16: " << std::hex << l16 << std::endl;

    }

    ciphertext = (l16 << 16) + r16;

//    std::cout << "output2: " << std::hex << ciphertext << "\n";

    return ciphertext;

}