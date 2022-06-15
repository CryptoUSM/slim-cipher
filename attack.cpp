//
// Created by Chan Yen Yee on 13/06/2022.
//

#include <iostream>
#include <utility>
#include <bit>
#include <bitset>
#include "key.h"


/*
 * r    pDiff     cDiff     w
 * 12 0B82 000A 0A00 801B 2^−28
 * 13 A208 A000 0A00 801B 2^−31
 * 14 0290 9000 0090 9004 2^−34
 * */

int main() {
    int pt_pairs_count = 2^30;
    int round1= 13; // for thre 13-round key recovery
    int round2= 14;

    std::pair<uint32_t, uint32_t> plaintext_pair[50];

    uint32_t alpha = 0x0B82000a; // pDiff after 12r
    uint32_t beta = 0x0a00801b; // cDiff which resulting pDiff

    uint32_t first = 0x00001234;
    uint32_t second;
    int increament = 3; //should be a prime number

    for (int j = 0; j < (2^10); j++) {

        second = first xor alpha;

        uint32_t encrypted_first = encrypt(first, round1);
        uint32_t encrypted_second = encrypt(second, round1);

//        uint32_t plaintext_diff = first xor second;
        uint32_t ciphertext_diff = encrypted_first xor encrypted_second;

        if ((ciphertext_diff & beta) != 0) { // matched at somewhere
            plaintext_pair[j] = std::make_pair(encrypted_first, encrypted_second);
        }

        first = first += increament;

    }




}

