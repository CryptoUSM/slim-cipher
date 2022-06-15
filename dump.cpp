//
// Created by Chan Yen Yee on 03/06/2022.
//

//    int firstBit = (output >> (sizeof(output) * 8 - 4))&0xF;
//    std::cout << "first bitsss: " << std::hex <<firstBit << std::endl;
//
//    int remainder= output & 0xF;
//    std::cout << "sasas shifted: " << std::hex <<remainder << std::endl;
//
//    uint64_t new_output= (output& 0x0FFF0)+remainder;
//    std::cout << "circular shifted: " << std::hex <<new_output << std::endl;

//
// Created by Chan Yen Yee on 07/06/2022.
//

#include <iostream>
#include <bit>
#include <bitset>
#include "key.h"

int circular_shift(uint8_t input, int moves ){
    uint8_t output=0;
    moves =moves% 4;

    uint8_t  output1= (input<<moves);
    uint8_t  output2= (input>> (4- moves));
    output= output1 | output2;

    return (output&0xF);
}

uint16_t * key_scheduling(uint8_t input_master_key[20], int round){
    // starting from msb(left 40bit) towrads lsb(right 40bit), every element is a nibble
    // break the 80bit into 20 4bit nibbles
    uint8_t master_key[20];
//            {
//            0b0011, 0b1001, 0b1011, 0b0100,
//            0b1110, 0b1100, 0b1011,0b0110,
//            0b0001, 0b1001, 0b0110, 0b1001,
//            0b1010, 0b1000 ,0b1101 ,0b1001,
//            0b0011, 0b1101 ,0b0010, 0b1110,
//    };

    // on 8th rk-3rd nibble, lsb and msb will experienced key expanding
    // the new lsb and msb are taking intermediate value of the previous 10()
    uint8_t lsb[10];
    uint8_t msb[10];
    uint16_t round_key[16];


    uint8_t *temp;
    temp= input_master_key;

    for (int k=0; k<20; k++){
        master_key[k]= *(temp+k);
    }



    /* nibble index
     * k1- 16, 17, 18, 19
     * k2- 12, 13, 14, 15
     * k3- 8, 9, 10, 11
     * k4- 4, 5, 6, 7
     * k5- 0, 1, 2, 3,
     * */



    int reversed_index=19; // because master_key has max index=19

    // inject first five round keys
    for (int m=0; m<5; m++){
        round_key[m]=0;
        for (int n=0; n<4; n++){
            uint8_t base= master_key[reversed_index];
            round_key[m]+= (base<<(n*4)) ;
            reversed_index-=1;
//            if (m==4){
//                std::cout<< "base " << n << ": "<< std::bitset<16>(base)<<"\n";
//                std::cout<< "rk " << n << ": "<< std::bitset<16>(round_key[m])<<"\n";
//
//            }

        }
//        std::cout<< "round key " << m << ": "<< std::hex<< round_key[m]<<"\n";
//        std::cout<< "============ " <<"\n";
    }

    // for first state lsb and msb
    for (int i=0; i<10; i++){
        msb[i]= master_key[i];
        lsb[i]= master_key[i+10];

    }

//    for (int a=0; a<10; a++){
//        std::cout<< "msb:" << std::bitset<4>(msb[a])<<" | ";
//        std::cout<< "lsb:" << std::bitset<4>(lsb[a])<<" | ";
//    }
//    int slim_sBox[16] = {12, 5, 6, 11, 9, 0, 10, 13, 3, 14, 15, 8, 4, 7, 1, 2};

    int lbc_sbox[16]={0, 8, 6, 0xd,
                      5, 0xf, 7, 0xc, 4, 0xe,
                      2, 3, 9,  1, 0xb, 0xa
    };

    // rebuild the sbox

    uint8_t intermediate_msb_value[10];
    uint8_t intermediate_lsb_value[10]; // sbox value for the msb

    int nibble_index=9;
    int register_state=0;

    for (int p=0; p<(round-5); p++) {
        uint16_t output=0;
        uint16_t nibble_output = 0;

        uint8_t temp_round_key[4];
        for (int y=0; y<4; y++){

            if (nibble_index==-1){
                std::fill_n(lsb, 10, 0);
                std::fill_n(msb, 10, 0);
//                    std::cout<< "\n";

                //reset lsb msb, spit out the round key
                for (int i=0; i<10; i++){
                    lsb[i]= intermediate_lsb_value[i];
                    msb[i]= intermediate_msb_value[i];
                }

                std::fill_n(intermediate_lsb_value, 10, 0);
                std::fill_n(intermediate_msb_value, 10, 0);

//                    std::cout<< "LSB: ";
//                    for (int e=0; e<10; e++){
//                        std::cout<< std::bitset<4>(lsb[e])<< " | ";
////                        std::cout<< std::bitset<4>(msb[e])<< "\n";
//
//                    }

//                    std::cout<< "\n MSB: ";
//                    for (int e=0; e<10; e++){
//                        std::cout<< std::bitset<4>(msb[e])<< " | ";
//                        std::cout<< std::bitset<4>(msb[e])<< "\n";
//                    }

                nibble_index=9;
                register_state+=1;
//                    std::cout<< std::bitset<4>(lsb[0])<< " | "<< std::bitset<4>(msb[0])<< "\n";
//                    std::cout<< register_state<< "\n";

            }


            uint8_t shifted_lsb_nibble = circular_shift(lsb[nibble_index], 2);
            uint8_t shifted_msb_nibble = circular_shift(msb[nibble_index], 3);

            // important, serve as one of the new key's nibble
            uint8_t sbox_lsb_msb = lbc_sbox[int(shifted_lsb_nibble xor msb[nibble_index])];
            intermediate_lsb_value[nibble_index] = sbox_lsb_msb;

            nibble_output = (shifted_msb_nibble xor sbox_lsb_msb);
            intermediate_msb_value[nibble_index] = nibble_output;

            output += (nibble_output << (4 * y));

//                if (p==2&&y==3) {
//                    std::cout << "nibble index:" << nibble_index << "\n";

//                    intermediate_lsb_value[nibble_index] = 0xf;
//                    std::cout << "xxxx:" << std::bitset<4>(intermediate_lsb_value[nibble_index]) << "\n";
//
//                    intermediate_msb_value[nibble_index] = 0xf;

//                    std::cout << "shifted_lsb_nibble:" << std::bitset<4>(shifted_lsb_nibble) << "\n";
//            std::cout << "shifted_msb_nibble:" << std::bitset<4>(shifted_msb_nibble) << "\n";
//            std::cout << "xor:" << std::bitset<4>(shifted_lsb_nibble xor msb[nibble_index]) << "\n";
//            std::cout << "int-xor:" << std::hex << (int(shifted_lsb_nibble xor msb[nibble_index])) << "\n";
//
//            std::cout << "sbox_lsb_msb:" << std::bitset<4>(sbox_lsb_msb) << "\n";
//                    std::cout << "ori lsb[" << nibble_index<< "]:" << std::bitset<4>(lsb[nibble_index]) << "\n";
//                    std::cout << "ori msb[" << nibble_index<< "]:" <<  std::bitset<4>(msb[nibble_index]) << "\n";
//                std::cout << "intermediate_lsb_value[" << nibble_index <<"]:" << std::bitset<4>(intermediate_lsb_value[nibble_index]) << "\n";
//                std::cout << "intermediate_msb_value[" << nibble_index <<"]:" << std::bitset<4>(intermediate_msb_value[nibble_index]) << "\n";
//            std::cout << "nibble output:" << y<< "\n";
//
//            std::cout << "================" << "\n";

//             nibble_index--; 0001 | 1011 | 0110 | 1010

//            }

            nibble_index-=1;

        }
        round_key[int(p+5)]=0;
        round_key[int(p+5)]= output;
//            std::cout << "output " <<int(p+6)<< ": " << std::hex<< (output) << "\n";

        output=0;

    }

    for (int j=0; j<round; j++){
        std::cout << "output " <<j<< ": " << std::hex<< round_key[j]<< "\n";
    }

    return round_key;

}

//
// Created by Chan Yen Yee on 07/06/2022.
//

#include <iostream>
#include <bit>
#include <bitset>
#include "key.h"
//#include <cstdint>
// g++ -o s5r slim5R.cpp
// ./s5r

int substitution(uint64_t x) {
    int sBox[16] = {12, 5, 6, 11, 9, 0, 10, 13, 3, 14, 15, 8, 4, 7, 1, 2};

    uint64_t total = 0;

    for (int i = 0; i < 4; i++) {
        int last_digit = x & 0x000F;
        x = (x >> 4);
        uint64_t sub_x = sBox[ last_digit ];

        // sub_x << 4*(i) meaning add the proper digits for the sub_x before adding into total
        total += (sub_x << 4 * (i));
    }

    return total;
}

int permutation(uint64_t x) {
    int pBox[16] = {7, 13, 1, 8, 11, 14, 2, 5, 4, 10, 15, 0, 3, 6, 9, 12};

    uint64_t total = 0;

    for (int i = 0; i < 16; i++) { // 16 bit right halves
        uint64_t remainder = x % 2;
        int shift_bit = pBox[ i ]; // when you are at last bit, meaning your only choice is moving forward--> shift left
        total += remainder << shift_bit; // therefore the shift bit is just how many bits you should shift left
        x = x >> 1; // shift the next bits
    }
    return total;
}

int main() {

    int round=10;
    uint32_t plaintext = 0x4356cded;
    uint32_t ciphertext = 0;
    uint8_t master_key[20]={
            0b0011, 0b1001, 0b1011, 0b0100,
            0b1110, 0b1100, 0b1011,0b0110,
            0b0001, 0b1001, 0b0110, 0b1001,
            0b1010, 0b1000 ,0b1101 ,0b1001,
            0b0011, 0b1101 ,0b0010, 0b1110,
    };

//    int round= 0;
//    std::cin >> round;

    uint16_t l16 = ((plaintext >> 16) & 0xffff);
    uint16_t r16 = plaintext & 0xffff; // to get the original number--> x AND F= x cuz F==1111 in bin
    uint16_t round_key[round];
    uint16_t *temp;

    temp= key_scheduling(master_key, round);

    for (int i=0; i<round; i++){
        round_key[i]=0;
        round_key[i]= *(temp + i);
        std::cout << "key in main round "<< i << ": "<<std::hex<<round_key[i]<< std::endl;
    }

    /*
     * keyed_r16= output of right halves input xor with round key
     * sub_r16= output of right halves input after sbox
     * per_r16= output of right halves input after pbox
     * round_r16= variable to preserve original value of r16
     * */
//
//    for (int r = 0; r < round; r++) {
//        uint16_t keyed_r16 = r16 xor round_key[r]; // r16_x1
//        uint16_t sub_r16 = substitution(keyed_r16);
//        uint16_t per_r16 = permutation(sub_r16);
//
//        std::cout << "==============Round " << int(round) <<  ": " << "====================" << std::endl;
//        std::cout << "r16: " << std::hex << r16 << std::endl;
//        std::cout << "round_key: " << std::hex << round_key << std::endl;
//
//        std::cout << "keyed r16: " << std::hex << keyed_r16 << std::endl;
//        std::cout << "sub_r16: " << std::hex << sub_r16 << std::endl;
//        std::cout << "per_r16: " << std::hex << per_r16 << std::endl;
//        // switch position
//        uint16_t round_r16 = r16;
//        r16 = per_r16 xor l16;
//        l16 = round_r16;
//
//        std::cout << "next r16: " << std::hex << r16 << std::endl;
//        std::cout << "next l16: " << std::hex << l16 << std::endl;
//        std::cout << "====================" << std::endl;
//
//    }

    ciphertext= (l16<<16)+ r16;

    std::cout<< "output: "<< std::hex<<ciphertext<< "\n";

// incremebt by prime
}


//            std::cout<< "first: " <<std::hex<< first<< "\n";
//            std::cout<< "first xor k: " <<std::hex<< (first xor k)<< "\n";

//            plaintext_pair[j]= std::make_pair(encrypt(first), encrypt(second));
//            std::cout<< "first output: " << std::hex<< plaintext_pair[j].first<< "\n";
//            std::cout<< "second output: " <<std::hex<< (plaintext_pair[j].second)<< "\n";
//            std::cout<< "=========== \n";
