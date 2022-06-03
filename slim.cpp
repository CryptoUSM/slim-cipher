#include <iostream>
#include <bit>
#include <bitset>
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

int circular_shift(uint64_t input, std::string direction, int moves){
    uint64_t output= 0;
    moves =moves% 40;
    if (direction== "L"){
        uint64_t  output1= (input<<moves);
        uint64_t  output2= (input>>(40- moves));
        output= output1 | output2;
        output= output& 0xFFFFFFFFFF; // eliminatet the first digit, strict the digits to 40bit(10 F) only
    }

    else if (direction== "R"){
        uint64_t  output1= (input>>moves);
        uint64_t  output2= (input<<(40- moves));
        output= output1 | output2;
        output= output& 0xFFFFFFFFFF; // eliminatet the first digit, strict the digits to 40bit(10 F) only
    }

//    std::cout << "output shifted: " << std::hex << (output) << std::endl;

    return output;

}

int key_scheduling(uint64_t lsb_master_key, uint64_t msb_master_key, int round) {
    uint64_t round_key = 0;
    // ffff1234abcdffff1234
    // 1-4321, 2-ffff, 3-dcba, 4: 4321, 5: ffff
    int index=15;

    // for case 3
    uint64_t ltp=0;
    uint64_t rtp=0;

    if (round >= 1 && round <= 5) {
        switch (round) {
            case 1:
                for (int i=4; i>0; i--){
                    uint64_t r= lsb_master_key & 0xF;
                    uint64_t k= (r<< 4*i);
                    round_key+=k;
//                    std::cout<< "r and k: "<< std::hex<< o<< " "<< std::hex<< (r) <<"\n" ;

                    lsb_master_key= lsb_master_key>>4;
                }
                round_key= round_key>>4;

                break;
            case 2:
                lsb_master_key= lsb_master_key>>16;
                for (int i=4; i>0; i--){
                    uint64_t r= lsb_master_key & 0xF;
                    uint64_t k= (r<< 4*i);
                    round_key+=k;
//                    std::cout<< "r and k: "<< std::hex<< o<< " "<< std::hex<< (r) <<"\n" ;

                    lsb_master_key= lsb_master_key>>4;
                }
                round_key= round_key>>4;

                break;
            case 3:
                for (int i=2; i>0; i--){
                    uint64_t r= msb_master_key & 0xF;
                    uint64_t k= (r<< 4*i);
                    ltp+= k;

                    msb_master_key= msb_master_key>>4;
                }

                for (int j=0; j<2; j++){
                    uint64_t r= (lsb_master_key >> 36)&0xF;
                    uint64_t k= (r<< 4*j);
                    rtp+= k;
//                    std::cout<< "r and k: "<< std::hex<< o<< " "<< std::hex<< (r) <<"\n" ;
                    lsb_master_key= lsb_master_key<<4;
                }
//                std::cout<< "rtp and ltp: "<< std::hex<< (rtp<<8)<< " "<< std::hex<< (ltp>>4) <<"\n" ;

                round_key=(rtp<<8)|(ltp>>4);

                break;
            case 4:
                msb_master_key= msb_master_key>>8;
                for (int i=4; i>0; i--){
                    uint64_t r= msb_master_key & 0xF;
                    uint64_t k= (r<< 4*i);
                    round_key+=k;
//                    std::cout<< "r and k: "<< std::hex<< o<< " "<< std::hex<< (r) <<"\n" ;

                    msb_master_key= msb_master_key>>4;
                }
                round_key= round_key>>4;

                break;
            case 5:
                msb_master_key= msb_master_key>>(16+8); // 16 is for case4, 8 is for case 3
                // lsb no need to shift becuz the bitshift is starting from behind, so no affect
                for (int i=4; i>0; i--){
                    uint64_t r= msb_master_key & 0xF;
                    uint64_t k= (r<< 4*i);
                    round_key+=k;
//                    std::cout<< "r and k: "<< std::hex<< o<< " "<< std::hex<< (r) <<"\n" ;

                    msb_master_key= msb_master_key>>4;
                }
                round_key= round_key>>4;

                break;
        }

    } else {


        uint64_t shifted_lsb = circular_shift(lsb_master_key, "L", 2);
        uint64_t temp_key = shifted_lsb xor msb_master_key;
        uint64_t sbox_shifted_lsb = substitution(shifted_lsb);


        round_key = lsb_master_key >> 2;
        // &=: That's a common way to set a specific bit to 0 in an integer that represents a bitfield.
    }
    std::cout << "key for round" << round << ": " << std::hex << round_key << std::endl;

    return round_key;
}

int main() {


//  int pBox[16] = {7, 13, 1, 8, 11, 14, 2, 5, 4, 10, 15, 0, 3, 6, 9, 12};



//  int sBox[16] = {12, 5, 6, 11, 9, 0, 10, 13, 3, 14, 15, 8, 4, 7, 1, 2};


    int input_round = 5;
    // 80-bit key split halves
    // msb---lsb (from right to left)
    uint64_t msb_master_key = 0xffff1234ab;
    uint64_t lsb_master_key = 0xcd12340123;

    uint64_t plaintext = 0x00001234;
    uint64_t ciphertext = 0;

    uint16_t l16 = plaintext >> 16;
    uint16_t r16 = plaintext & 0x00FFFF; // to get the original number--> x AND F= x cuz F==1111 in bin
    uint64_t k= key_scheduling(lsb_master_key, msb_master_key, 3);

    /*
     * keyed_r16= output of right halves input xor with round key
     * sub_r16= output of right halves input after sbox
     * per_r16= output of right halves input after pbox
     * round_r16= variable to preserve original value of r16
     * */

    for (int round = 1; round <= input_round; round++) {
        uint64_t round_key = key_scheduling(lsb_master_key, msb_master_key, round);

        uint64_t keyed_r16 = r16 xor round_key; // r16_x1


        uint64_t sub_r16 = substitution(keyed_r16);
        uint64_t per_r16 = permutation(sub_r16);
        std::cout << "==============Round " << round <<  ": " << "====================" << std::endl;
        std::cout << "r16: " << std::hex << r16 << std::endl;
        std::cout << "round_key: " << std::hex << round_key << std::endl;

        std::cout << "keyed r16: " << std::hex << keyed_r16 << std::endl;
        std::cout << "sub_r16: " << std::hex << sub_r16 << std::endl;
        std::cout << "per_r16: " << std::hex << per_r16 << std::endl;

        uint64_t round_r16 = r16;
        r16 = per_r16 xor l16;
        l16 = round_r16;

        std::cout << "next r16: " << std::hex << r16 << std::endl;
        std::cout << "next l16: " << std::hex << l16 << std::endl;

        std::cout << "====================" << std::endl;

    }

}