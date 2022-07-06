//
// Created by Chan Yen Yee on 06/07/2022.
//


int main(){
    for (int y=0; y<first_ciphertext.size(); y++){
        key_outdata<< "first_pl: " << std::hex <<first_plaintext.at(y)<< "\n";
        key_outdata<< "second_pl: " << std::hex << second_plaintext.at(y)<< "\n";

        key_outdata<< "13R-first_cp: " << std::hex << first_ciphertext.at(y)<< "\n";
        key_outdata<< "13R-second_cp: " << std::hex << second_ciphertext.at(y)<< "\n";
        key_outdata<< "13R-cDiff: " << std::hex << ( first_ciphertext.at(y)^ second_ciphertext.at(y)) << "\n";

        uint32_t first_12r= encrypt(first_plaintext.at(y), 12, round_keys);
        uint32_t second_12r= encrypt(second_plaintext.at(y), 12, round_keys);

        key_outdata<< "12R-first_cp: " << std::hex <<first_12r<< "\n";
        key_outdata<< "12R-second_cp: " << std::hex << second_12r<< "\n";
        key_outdata<< "12R-cDiff: " << std::hex << (first_12r^ second_12r) << "\n";

        key_outdata<< "---verification---" << "\n";

        uint32_t decrypted_first = one_round_decrypt(first_ciphertext.at(y), 0xbe43);
        uint32_t decrypted_second = one_round_decrypt(second_ciphertext.at(y), 0xbe43);
        key_outdata<< "decrypt-13r-to-12r-first: " << std::hex <<decrypted_first<< "\n";
        key_outdata<< "decrypt-13r-to-12r-second: " << std::hex << decrypted_second<< "\n";
        key_outdata<< "ver-cDiff: " << std::hex << (decrypted_first^ decrypted_second) << "\n";

        key_outdata<< "===================" << "\n";


    }
}

