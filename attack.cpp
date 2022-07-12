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

uint16_t generate_key() {

}

/*
 * r    pDiff     cDiff     w
 * 12 0B82 000A 0A00 801B 2^−28
 * 13 A208 A000 0A00 801B 2^−31
 * 14 0290 9000 0090 9004 2^−34
 * */

int attack(std::string run) {
//    std::ofstream filter_outdata;

    std::ofstream key_outdata;
    std::ofstream verify_outdata;

    std::string verify_file= go_to_line(2)+"R-verification-"+ run+".txt";
    std::string master_file= go_to_line(2)+"R-all_keys-"+run+".txt";

    verify_outdata.open(verify_file, std::fstream::app);
    key_outdata.open("13R-keyhit-2.txt", std::fstream::app);

    std::fstream file("input-param.txt");
    std::size_t pos{}; // for str--> int conversion

    int round_to_encrypt = stol(go_to_line(2), &pos, 10); // for thre 13-round key recovery

    uint8_t *master_key;
    master_key = randomize_master_key();

    clock_t tStart = clock();

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

//     template is actually preserved the ACTUAL known bits, then represent * with 0
//     in our case, template always all 0 as the all inactive bits=0
    uint32_t temp_late = ((r16_cDiff << 16) + 0);
    mask = ((0xffff << 16) + mask);
    std::cout << "mask: " << std::bitset<32>(mask) << std::endl;
    std::cout << "template: " << std::hex << (temp_late) << std::endl;

    uint32_t first = stol(go_to_line(5), &pos, 16);
    uint32_t second;
    int increment = stol(go_to_line(6), &pos, 10); //should be a prime number

// linear filtering
    uint16_t *round_keys;
    round_keys = key_scheduling(master_key, 15); // just in case, 20R encrypt is enough for all purpose

    for (int i=0; i< 15; i++){
        key_outdata << "key " <<i << ": " <<std::hex << round_keys[i] << std::endl;
    }

    for (int i=0; i< 15; i++){
        key_outdata << "master_key " <<i << ":0b " <<std::bitset<8>(master_key[i])<< std::endl;
//        key_outdata<< " - "<<  master_key[i]<< std::endl;
    }
    std::cout << "actual key: " << std::hex << round_keys[round_to_encrypt - 1] << std::endl;
    std::cout << "round_key actual key+1: " << std::hex << round_keys[round_to_encrypt] << std::endl;

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

        first = first +increment;
    }

//    std::ifstream in;
//    std::string file_name = "pt-input-13r.txt";
//    in.open(file_name); // opens the file
//    if (!in) {
//        std::cerr << "Cannot open the File : " << file_name << std::endl;
//        return false;
//    }
//    std::string extracted_input;
//    std::string str;
////    std::size_t pos{};
//    while (std::getline(in, str)) {
//        // Line contains string of length > 0 then save it in vector
//        if (str.size() > 0) {
//            if (str[0] == '1' || str[0] == '2' || str[0] == '3' || str[0] == '4') {
//                extracted_input = (str.substr(str.find(": ") + 1));
//                trim(extracted_input);
//                std::string x = "0x" + extracted_input;
//                std::uint32_t y = stol(x, &pos, 16);
//
//                if (str[0] == '3')
//                    first_ciphertext.push_back(y);
//                else if (str[0] == '4')
//                    second_ciphertext.push_back(y);
//
//                else if (str[0] == '1')
//                    first_plaintext.push_back(y);
//                else if (str[0] == '2')
//                    second_plaintext.push_back(y);
//            }
//        }
//    }

// verification
    int right_pairs_count = 0;
    for (int y=0; y<first_ciphertext.size(); y++) {
        verify_outdata << "first_pl: " << std::hex << first_plaintext.at(y) << "\n";
        verify_outdata << "second_pl: " << std::hex << second_plaintext.at(y) << "\n";

        verify_outdata << round_to_encrypt<<"R-first_cp: " << std::hex << first_ciphertext.at(y) << "\n";
        verify_outdata << round_to_encrypt<< "R-second_cp: " << std::hex << second_ciphertext.at(y) << "\n";
        verify_outdata << round_to_encrypt<< "R-cDiff: " << std::hex << (first_ciphertext.at(y) ^ second_ciphertext.at(y)) << "\n";

        uint32_t decrypted_first = one_round_decrypt(first_ciphertext.at(y), round_keys[round_to_encrypt - 1]);
        uint32_t decrypted_second = one_round_decrypt(second_ciphertext.at(y), round_keys[round_to_encrypt - 1]);
        verify_outdata << "decrypt-first: " << std::hex << decrypted_first << "\n";
        verify_outdata << "decrypt-second: " << std::hex << decrypted_second << "\n";
        verify_outdata << "decrypt-cDiff: " << std::hex << (decrypted_first ^ decrypted_second) << "\n";
        verify_outdata<< "===================" << "\n";

        if ((decrypted_first ^ decrypted_second) == beta)
            right_pairs_count++;

    }

//    if (right_pairs_count>0){
////        verify_outdata<< "right pairs count: "<< right_pairs_count<< "\n";
//        for (int i = 0; i < 20; i++) {
//            verify_outdata << "0b"<<std::bitset<4>(master_key[i])<<", \n";
//        }
//    }



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
    //pattern: 801b --> *0**

    // the logic is: one active unknown (in hex), increase the 16r-FOR loop by 1
    // exp: 801b- 16x16x16= 4096 keys
    // 1d48- 16^4= 65536 keys

    for (int i = 0; i < 16; i++) {
        uint16_t star_key = 0; //*0**--> 801b
        star_key = (star_key | i << 12);
        for (int j = 0; j < 256; j++) {
            uint16_t generated_key = (star_key | j);
            subkeys.push_back(generated_key);
//            key_outdata<< std::hex<< generated_key<< "\n";
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

    std::cout << "cur_elem: " << max_hit << "\n";

    std::vector<uint16_t> possible_subkeys;
    for (int j = 0; j < subkeys.size(); j++) {
        if (subkeys_hits[j] == max_hit) {
            possible_subkeys.push_back(subkeys[j]);
        }
    }
    std::cout << "Count of possible right key(s): " << possible_subkeys.size() << "\n";

    //print the possible subkeys
    for (int j = 0; j < possible_subkeys.size(); j++) {
        std::cout << possible_subkeys[j] << "\n";
    }


    verify_outdata<< "======================= \n";

    return right_pairs_count;
}

