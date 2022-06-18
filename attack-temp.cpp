//
// Created by Chan Yen Yee on 18/06/2022.
//


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



int main() {
//    uint32_t p= 0x12345678;
//    uint32_t enc_p= encrypt(p, 1);
//    uint32_t dec_p= decrypt(enc_p, 0x3d2e);
//    std::cout<< std::hex<< "p: "<< p << "\n";
//    std::cout<< std::hex<< "enc_p: "<< enc_p << "\n";
//    std::cout<< std::hex<< "dec_p: "<< dec_p << "\n";



    clock_t tStart = clock();

    int pt_pairs_count = pow(2, 30);
//    int pt_pairs_count = 10000;

    int round1 = 13; // for thre 13-round key recovery
    int round2 = 14;

//    std::pair<uint32_t, uint32_t> plaintext_pair[pt_pairs_count];

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

    std::ofstream outdata;
    outdata.open("attack-output.txt"); // opens the file
    if( !outdata ) { // file couldn't be opened
        std::cerr << "Error: file could not be opened" << std::endl;
        exit(1);
    }


// this will need 4300++ seconds, so just use the input file to read the pairs
//    for (int j = 0; j < pt_pairs_count; j++) {
//
//        second = first xor alpha;
//
//        uint32_t encrypted_first = encrypt(first, round1);
//        uint32_t encrypted_second = encrypt(second, round1);
//
//        uint32_t ciphertext_diff = encrypted_first xor encrypted_second;
//
////        ciphertext_diff= 0x0a003311;
//
//        if (((ciphertext_diff xor mask) & active_bits_position) == 0) {
//            first_plaintext.push_back(first);
//            second_plaintext.push_back(second);
//
//            outdata<< "round: "<< std::dec<< j << ": \n";
//            outdata<< "first: "<< std::hex<< first << "\n";
//            outdata<< "second: "<< std::hex<< second << "\n";
//            outdata<< "--------------- "<< "\n";
//        }
//
//        first = first +increment;
//    }
//    outdata<< "done! \n";
//    outdata<< "Time taken: " << ((double)(clock() - tStart)/CLOCKS_PER_SEC);
//    outdata.close();


// Read the input file and insert into vector
    std::ifstream in;
    std::string file_name= "attack-input.txt";
    in.open(file_name); // opens the file
    if(!in)
    {
        std::cerr << "Cannot open the File : "<<file_name<<std::endl;
        return false;
    }

    std::string extracted_input;
    std::string str;

    std::size_t pos{};

//    std::cout<< std::hex<<std::stoi("0x68f6de2f", &pos, 16)<< std::endl;

    while (std::getline(in, str)){
        // Line contains string of length > 0 then save it in vector
        if(str.size() > 0){
            char round= str[0];
            if (round== 'f' || round== 's'){
                extracted_input= (str.substr(str.find(": ")+1));
                trim(extracted_input);
                std::string x= "0x"+extracted_input;
                std::uint32_t y= stol(x, &pos, 16);
//            std::cout<<std::hex<< (y)<< std::endl;

                if (round=='f')
                    first_plaintext.push_back(y);
                else
                    second_plaintext.push_back(y);

            }
        }

    }

    uint32_t key_counter= pow(2,12);
    std::vector<uint16_t> subkeys;
    int subkeys_hits [key_counter];
    std::fill_n(subkeys_hits, key_counter, 0);

// prepare key
// pattern: 0a00801b--> 0000 *0**
for (int i=0; i<16; i++) {
    uint16_t star_key = 0x0a00; //*a**
    star_key= (star_key | i<<12);

    for (int j=0; j<256; j++){
        uint16_t generated_key= (star_key | j);
        subkeys.push_back(generated_key);
    }

}

    for (int j=0; j<subkeys.size(); j++){
        for (int k=0; k<first_plaintext.size(); k++){
            uint32_t xx= decrypt(first_plaintext[k], subkeys.at(j));
            uint32_t yy= decrypt(second_plaintext[k], subkeys.at(j));

            if ((xx xor yy) == beta){
                subkeys_hits[j]+=1;
                outdata<< "subkey: " <<std::hex<< subkeys[j]<< "\n";
                outdata<< "pt1: " <<std::hex<< xx<< "\n";
                outdata<< "pt2: " <<std::hex<< yy<< "\n";
                outdata<< "xor: " <<std::hex<< (xx xor yy) << "\n";
            }
        }

        outdata<< "key: " <<std::hex<< subkeys[j]<< " hits: "<< std::dec<< subkeys_hits[j]<< "\n";
    }



// most_possible_key= subkeys.find_max (encounter)
// match with your 12th- subkey






}


