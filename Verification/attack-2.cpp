//
// Created by Chan Yen Yee on 13/06/2022.
//

#include <iostream>
#include <bit>
#include <bitset>
#include "../key.h"
#include "math.h"
#include <vector>

#include <fstream>
#include <limits>

std::string go_to_line(unsigned int num) {
    std::fstream file("input-param.txt");

    file.seekg(std::ios::beg);
    for (int i = 0; i < num - 1; ++i) {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::string x;
    file >> x;

    return x;
}

int attack(std::string run) {
    std::ofstream key_outdata;
    std::ofstream verify_outdata;

    // output file name
    std::string verify_file = go_to_line(2) + "R-verification-" + run + ".txt";
    std::string key_file = go_to_line(2) + "R-all_keys-" + run + ".txt";

    verify_outdata.open(verify_file, std::fstream::app);
    key_outdata.open(key_file, std::fstream::app);

    std::fstream file("input-param.txt");
    std::size_t pos{}; // for str--> int conversion

    int round_to_encrypt = stol(go_to_line(2), &pos, 10);

//    uint8_t *master_key;
//    master_key = randomize_master_key();

    uint8_t master_key[20] = {
            0b0110,
            0b0101,
            0b1111,
            0b0010,
            0b1101,
            0b1001,
            0b1000,
            0b0101,
            0b1001,
            0b1000,
            0b0000,
            0b0000,
            0b1101,
            0b1000,
            0b0100,
            0b0010,
            0b1101,
            0b1111,
            0b1110,
            0b1011,
    };

    int pt_pairs_count = pow(2, stol(go_to_line(1), &pos, 10));

    uint32_t alpha = stol(go_to_line(3), &pos, 16);
    uint32_t beta = stol(go_to_line(4), &pos, 16);

    std::vector<uint32_t> first_ciphertext;
    std::vector<uint32_t> second_ciphertext;

    std::vector<uint32_t> first_plaintext;
    std::vector<uint32_t> second_plaintext;

    uint16_t r16_cDiff = (beta & 0xffff);

    uint16_t fBox_sub_beta = 0;
    uint16_t x1 = r16_cDiff;
    for (int i = 0; i < 4; i++) {
        int rem = x1 & 0xf;
        if (rem != 0) {
            fBox_sub_beta += (0 << 4 * i);
        } else {
            fBox_sub_beta += (0b1111 << 4 * i);
        }
        x1 = x1 >> 4;
    }

    // represent known bits as 1
    // represent unknown * as 0
    uint16_t fBox_per_beta = permutation(fBox_sub_beta);
    uint32_t mask = fBox_per_beta; // no need to xor with l16 in this case

    // template is actually preserved the ACTUAL known bits, then represent * with 0
    // in our case, template always all 0 as the all inactive bits=0
    uint32_t temp_late = ((r16_cDiff << 16) + 0);
    mask = ((0xffff << 16) + mask);

    uint32_t first = stol(go_to_line(5), &pos, 16);
    uint32_t second;
    int increment = stol(go_to_line(6), &pos, 10); //should be a prime number

    // linear filtering
    uint16_t *round_keys;
    round_keys = key_scheduling(master_key, round_to_encrypt);

    for (int i = 0; i < round_to_encrypt; i++) {
        std::cout << "key " << i << ": " << std::hex << round_keys[i] << std::endl;
    }

    for (int i = 0; i < 20; i++) {
        key_outdata << "master_key " << i << ":0b " << std::bitset<8>(master_key[i]) << std::endl;
    }

    for (int j = 0; j < pt_pairs_count; j++) {

        second = first xor alpha;

        uint32_t encrypted_first = encrypt(first, round_to_encrypt, round_keys);
        uint32_t encrypted_second = encrypt(second, round_to_encrypt, round_keys);

        uint32_t ciphertext_diff = (encrypted_first ^ encrypted_second);

        if ((ciphertext_diff & mask) == temp_late) {
            first_plaintext.push_back(first);
            second_plaintext.push_back(second);

            first_ciphertext.push_back(encrypted_first);
            second_ciphertext.push_back(encrypted_second);
        }

        first = first + increment;
    }

    int right_pairs_count = verification(first_ciphertext, second_ciphertext, first_plaintext,
                                         second_plaintext, verify_file, round_keys, round_to_encrypt, beta);

    // how many pairs left
    int distinguisher_prob = first_ciphertext.size() / pow(2, stol(go_to_line(1), &pos, 10));
    verify_outdata << "Probability of Distinguisher " << distinguisher_prob << "\n";

// count how many inactive bits
//    int active_bits = 0;
//    for (int j = 0; j < 4; j++) {
//        int cur = r16_cDiff & 0xf;
//        if (cur != 0) {
//            active_bits++;
//        }
//        r16_cDiff = r16_cDiff >> 4;
//    }
//
    uint32_t key_counter = pow(2, 12);
    std::vector<uint16_t> subkeys;
    int subkeys_hits[key_counter];
    std::fill_n(subkeys_hits, key_counter, 0);

    //prepare key
    // the logic is: one active unknown (in hex), increase the 16r-FOR loop by 1
    // exp: 801b- 16x16x16= 4096 keys
    // 1d48- 16^4= 65536 keys
    // therefore 801b= 16x1x16x16
    for (int i = 0; i < 16; i++) {
        uint16_t star_key = 0; //*0**--> 801b
        star_key = (star_key | i << 12);
        for (int j = 0; j < 256; j++) {
            uint16_t generated_key = (star_key | j);
            subkeys.push_back(generated_key);
        }
    }


    for (int j = 0; j < subkeys.size(); j++) {
        for (int k = 0; k < first_ciphertext.size(); k++) {
            uint32_t decrypted_first = one_round_decrypt(first_ciphertext[k], subkeys.at(j));
            uint32_t decrypted_second = one_round_decrypt(second_ciphertext[k], subkeys.at(j));

            if ((decrypted_second ^ decrypted_first) == beta) {
                subkeys_hits[j] += 1;
            }

        }
        key_outdata << std::hex << subkeys[j] << ": " << subkeys_hits[j] << "\n";
    }

    int max_hit = subkeys_hits[0];
    for (int i = 0; i < key_counter; i++) {
        if (max_hit < subkeys_hits[i])
            max_hit = subkeys_hits[i];
    }
    std::vector<uint16_t> possible_subkeys;
    for (int j = 0; j < subkeys.size(); j++) {
        if (subkeys_hits[j] == max_hit) {
            possible_subkeys.push_back(subkeys[j]);
        }
    }
    std::cout << "Count of possible right key(s): " << std::dec << possible_subkeys.size() << " - " << max_hit
              << "hits \n";

    //print the possible subkeys
    for (int j = 0; j < possible_subkeys.size(); j++) {
        std::cout << possible_subkeys[j] << "\n";
    }

    verify_outdata.close();
    key_outdata.close();

    return right_pairs_count;
}

