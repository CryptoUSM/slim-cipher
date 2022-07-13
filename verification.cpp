//
// Created by Chan Yen Yee on 06/07/2022.
//

#include <iostream>
#include <utility>
#include <bit>
#include <bitset>
#include "key.h"
#include "math.h"
#include <vector>

#include <fstream>
#include <cstdlib>
#include <time.h>
#include <limits>


int verification(std::vector<uint32_t> first_ciphertext, std::vector<uint32_t> second_ciphertext,
                  std::vector<uint32_t> first_plaintext,std::vector<uint32_t> second_plaintext,
                  std::string verify_file, uint16_t *round_keys, int round_to_encrypt, uint32_t beta){
    std::ofstream verify_outdata;
    verify_outdata.open(verify_file, std::fstream::app);

    int right_pairs_count = 0;
    for (int y = 0; y < first_ciphertext.size(); y++) {
        verify_outdata << "first_pl: " << std::hex << first_plaintext.at(y) << "\n";
        verify_outdata << "second_pl: " << std::hex << second_plaintext.at(y) << "\n";

        verify_outdata << round_to_encrypt << "R-first_cp: " << std::hex << first_ciphertext.at(y) << "\n";
        verify_outdata << round_to_encrypt << "R-second_cp: " << std::hex << second_ciphertext.at(y) << "\n";
        verify_outdata << round_to_encrypt << "R-cDiff: " << std::hex
                       << (first_ciphertext.at(y) ^ second_ciphertext.at(y)) << "\n";

        uint32_t decrypted_first = one_round_decrypt(first_ciphertext.at(y), round_keys[round_to_encrypt - 1]);
        uint32_t decrypted_second = one_round_decrypt(second_ciphertext.at(y), round_keys[round_to_encrypt - 1]);
        verify_outdata << "decrypt-first: " << std::hex << decrypted_first << "\n";
        verify_outdata << "decrypt-second: " << std::hex << decrypted_second << "\n";
        verify_outdata << "decrypt-cDiff: " << std::hex << (decrypted_first ^ decrypted_second) << "\n";
        verify_outdata << "===================" << "\n";

        if ((decrypted_first ^ decrypted_second) == beta)
            right_pairs_count++;

    }

    int distinguisher_prob = first_ciphertext.size() / pow(2, 30);
    verify_outdata << "Probability of Distinguisher " << distinguisher_prob << "\n";

    return right_pairs_count;
}

