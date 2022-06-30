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

static inline void ltrim(std::string &s){
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}
// trim from end (in place)
static inline void rtrim(std::string &s){
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}
// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}


/*
 * r    pDiff     cDiff     w
 * 12 0B82 000A 0A00 801B 2^−28
 * 13 A208 A000 0A00 801B 2^−31
 * 14 0290 9000 0090 9004 2^−34
 * */

int main() {
    std::ofstream filter_outdata;
    std::ofstream key_outdata;
//    filter_outdata.open("13R-filter-corrected-2.txt"); // opens the file
    key_outdata.open("13R-key-hit-corrected-2.txt"); // opens the file
    if( !filter_outdata || !key_outdata) { // file couldn't be opened
        std::cerr << "Error: file could not be opened" << std::endl;
        exit(1);
    }

    clock_t tStart = clock();

    int pt_pairs_count = pow(2, 30);
    int round1 = 13; // for thre 13-round key recovery
    int round2 = 14;

    std::vector<uint32_t> first_plaintext;
    std::vector<uint32_t> second_plaintext;

    // for 13R
    uint32_t alpha = 0x0B82000a; // pDiff after 12r
    uint32_t beta = 0x0a00801b; // cDiff which resulting pDiff

    uint16_t l16_cDiff = ((beta >> 16) & 0xffff);
    uint16_t r16_cDiff = (beta & 0xffff);

    uint16_t fBox_sub_beta=0;
//    uint16_t x1= r16_cDiff ^ 0xbe43 ;
    uint16_t x1= r16_cDiff;
//    std::cout<<"x1: "<< std::hex<< (x1)<< std::endl;

    for (int i=0; i<4; i++){
        int rem= x1&0xf;
        if (rem!=0){
            fBox_sub_beta+=(0<<4*i);

        }
        else{
            fBox_sub_beta+=(0b1111<<4*i);
        }
        x1= x1>>4;

    }

    // represent known bits as 1
    // represent unknown * as 0
    uint16_t fBox_per_beta= permutation(fBox_sub_beta);
    uint32_t mask= fBox_per_beta; // no need to xor with l16 in this case

//    std::cout<<"mask-l16: "<< std::bitset<32>(l16_cDiff<< 16)<< std::endl;

    // template is actually preserved the ACTUAL known bits, then represent * with 0
    // in our case, template always all 0 as the all inactive bits=0
    uint32_t temp_late= ((r16_cDiff<<16) + 0);
    mask= ((0xffff<< 16) + mask);
    std::cout<<"mask: "<< std::bitset<32>(mask)<< std::endl;
    std::cout<<"template: "<< std::hex<<(temp_late)<< std::endl;

    uint32_t first = 0xa4321411;
    uint32_t second;
    int increment = 7; //should be a prime number

    // 13r-be43, 12r-89e7

// linear filtering
//    for (int j = 0; j < pt_pairs_count; j++) {
//
//        second = first xor alpha;
//
//        uint32_t encrypted_first = encrypt(first, round1); //encrypt 13 rounds
//        uint32_t encrypted_second = encrypt(second, round1);
//
//        uint32_t ciphertext_diff = (encrypted_first ^ encrypted_second);
//
////        pattern: 801b 0*** *0** ***0 ***0
//        if ((ciphertext_diff & mask) == temp_late) {
//            first_plaintext.push_back(encrypted_first);
//            second_plaintext.push_back(encrypted_second);
//
//            filter_outdata<< "round: "<< std::dec<< j << ": \n";
//            filter_outdata<< "af_pl: "<< std::hex<< first << "\n";
//            filter_outdata<< "bs_pl: "<< std::hex<< second << "\n";
//            filter_outdata<< "cf_cp: "<< std::hex<< encrypted_first << "\n";
//            filter_outdata<< "ds_cp: "<< std::hex<< encrypted_second << "\n";
//            filter_outdata<< "cDiff: "<< std::hex<< ciphertext_diff << "\n";
//            filter_outdata<< "--------------- "<< "\n";
//        }
//
//        first = first +increment;
//    }
//    filter_outdata<< "done! \n";
//    filter_outdata<< "Time taken: " << ((double)(clock() - tStart)/CLOCKS_PER_SEC);
//    filter_outdata.close();

// Read the input file and insert into vector
    std::ifstream in;
    std::string file_name= "13R-filter-corrected-2.txt";
    in.open(file_name); // opens the file
    if(!in)
    {
        std::cerr << "Cannot open the File : "<<file_name<<std::endl;
        return false;
    }
    std::string extracted_input;
    std::string str;
    std::size_t pos{};
    while (std::getline(in, str)){
        // Line contains string of length > 0 then save it in vector
        if(str.size() > 0){
//            std::string round= str;
            if (str[0]== 'c' || str[0]== 'd'){
                extracted_input= (str.substr(str.find(": ")+1));
                trim(extracted_input);
                std::string x= "0x"+extracted_input;
                std::uint32_t y= stol(x, &pos, 16);

                if (str[0]== 'c')
                    first_plaintext.push_back(y);
                else
                    second_plaintext.push_back(y);
            }
        }
    }

    uint32_t key_counter = pow(2, 12);
    std::vector<uint16_t> subkeys;
    int subkeys_hits[key_counter];
    std::fill_n(subkeys_hits, key_counter, 0);

    // prepare key
    // pattern: 801b --> *0**
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
        for (int k = 0; k < first_plaintext.size(); k++) {
            uint32_t decrypted_first = one_round_decrypt(first_plaintext[k], subkeys.at(j));
            uint32_t decrypted_second = one_round_decrypt(second_plaintext[k], subkeys.at(j));

            uint16_t first_a= first_plaintext[k]&0xffff;
            uint16_t first_z= decrypted_first&0xffff;


            uint16_t second_a= second_plaintext[k]&0xffff;
            uint16_t second_z= decrypted_second&0xffff;

            uint16_t aDiff= first_a xor second_a;
            uint16_t zDiff= first_z xor second_z;

            key_outdata<< "pairs: "<< std::hex<< first_plaintext[k]<<" "<< second_plaintext[k]<<" "<< "\n";
            key_outdata<< "aDiff: "<< std::hex<< first_a<<" "<< second_a<<" "<< aDiff<< "\n";
            key_outdata<< "zDiff: "<< std::hex<<  first_z<<" "<< second_z<<" "<< zDiff<< "\n";

            if ((aDiff xor zDiff) == l16_cDiff) {
                subkeys_hits[j] += 1;
//                std::cout<< "sadadsdsadsd: "<< decrypted_first<<" "<< decrypted_second<<" "<< l16_cDiff<< "\n";

            }

            key_outdata<< "cDiff_r16: "<< std::hex<<  (aDiff xor zDiff)<<" "<< "\n";
            key_outdata<< "-------------------- \n";

        }

        key_outdata<< subkeys[j]<< ": "<< subkeys_hits[j]<< "\n";
        key_outdata<< "==================================""\n";

    }



// most_possible_key= subkeys.find_max (encounter)
// match with your 12th- subkey
}

