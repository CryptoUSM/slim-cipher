//
// Created by Chan Yen Yee on 13/06/2022.
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


/*
 * r    pDiff     cDiff     w
 * 12 0B82 000A 0A00 801B 2^−28
 * 13 A208 A000 0A00 801B 2^−31
 * 14 0290 9000 0090 9004 2^−34
 * */

static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}


int main() {
    int pt_pairs_count = pow(2, 30);
    int round1 = 13; // for thre 13-round key recovery
    int round2 = 14;

    std::vector<uint32_t> first_plaintext;
    std::vector<uint32_t> second_plaintext;

    uint32_t alpha = 0x0B82000a; // pDiff after 12r
    uint32_t beta = 0x0a00801b; // cDiff which resulting pDiff

    uint16_t l16_cDiff = ((beta >> 16) & 0xffff); //0a00
    uint16_t r16_cDiff = (beta & 0xffff); //801b

//    l16_cDiff: a00
//    r16_cDiff: 801b
//    r16_cDiff_fbox: 834d ----> F(801b)
//    mask(make all star to 0): a000300
//    active_bits_position: ffff0f00

    uint16_t r16_cDiff_fbox = (permutation(substitution(r16_cDiff)) xor l16_cDiff);

    // only second sbox is fixed for r16:801b // filtering_pattern
    uint32_t mask = (l16_cDiff << 16) | (0x0f00 & r16_cDiff_fbox);

    //make all inactive bits to 1
    uint32_t active_bits_position = (((l16_cDiff | 0xffff) << 16) + (r16_cDiff_fbox | 0x0f00)) & 0xffff0f00;
    // mask=1000 0011 0100 1101 (right), all
    // pattern= 0a00 **** 0011 **** **** --> active sbox= non zero input

    uint32_t first = 0xa4321411;
    uint32_t second;
    int increment = 7; //should be a prime number


// this will need 4300++ seconds, so just use the input file to read the pairs
    for (int j = 0; j < pt_pairs_count; j++) {

        second = first xor alpha;

        uint32_t encrypted_first = encrypt(first, round1);
        uint32_t encrypted_second = encrypt(second, round1);

        uint32_t ciphertext_diff = encrypted_first xor encrypted_second;

        if (((ciphertext_diff xor mask) & active_bits_position) == 0) {
            first_plaintext.push_back(first);
            second_plaintext.push_back(second);
        }

        first = first + increment;
    }

    uint32_t key_counter = pow(2, 12);
    std::vector<uint16_t> subkeys;
    int subkeys_hits[key_counter];
    std::fill_n(subkeys_hits, key_counter, 0);

// prepare key
// pattern: 0a00801b--> 0000 *0**
    for (int i = 0; i < 16; i++) {
        uint16_t star_key = 0x0a00; //*a**
        star_key = (star_key | i << 12);

        for (int j = 0; j < 256; j++) {
            uint16_t generated_key = (star_key | j);
            subkeys.push_back(generated_key);
        }

    }

    for (int j = 0; j < subkeys.size(); j++) {
        for (int k = 0; k < first_plaintext.size(); k++) {
            uint32_t xx = decrypt(first_plaintext[k], subkeys.at(j));
            uint32_t yy = decrypt(second_plaintext[k], subkeys.at(j));

            if ((xx xor yy) == beta) {
                subkeys_hits[j] += 1;
            }
        }
    }



// most_possible_key= subkeys.find_max (encounter)
// match with your 12th- subkey
}

