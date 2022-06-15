//
// Created by Chan Yen Yee on 13/06/2022.
//

#include <iostream>
#include <utility>
#include <bit>
#include <bitset>
#include "key.h"


int main() {
    int pt_pairs_count = 50;

    std::pair<uint32_t, uint32_t> plaintext_pair[pt_pairs_count];

    uint32_t alpha = 0x0B82000a; // delta p== plaintext diff after 12r
    uint32_t beta = 0x0a00801b; // delta c == ciphertxet diff from pair which resulting delta p

    uint32_t first = 0x00001234;
    uint32_t second;
    int increament = 3; //should be sth prime

    for (int j = 0; j < pt_pairs_count; j++) {

        second = first xor alpha;


        uint32_t encrypted_first = encrypt(first);
        uint32_t encrypted_second = encrypt(second);

        uint32_t plaintext_diff = first xor second;
        uint32_t ciphertext_diff = encrypted_first xor encrypted_second;
        if ((ciphertext_diff & beta) != 0) { // matched at somewhere
            plaintext_pair[j] = std::make_pair(encrypt(first), encrypt(second));
        }

        first = first += increament;

    }


    std::cout << "first output: " << std::hex << plaintext_pair[2].first << "\n";


}

