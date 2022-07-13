//
// Created by Chan Yen Yee on 08/07/2022.
//
#include <iostream>
#include "key.h"
#include <string>
#include <fstream>

//void A(int target, int current){
//    if (target== current) {
//        //do sth
//    }
//
//    else {
//        A(target, current+1 );
//    }
//}
//A(4, 0); // this suppose to create a 4 nested FOR-loop


//
//void AllCombos(int target, int current, int index) {
//    std::ofstream main_outdata;
//
//    main_outdata.open("111test1.txt");
//    for (int i=0; i<16; i++){
//        main_outdata<< std::hex<< (i <<(4*current)) << "\n";
//    }
//    if (target== target+1) {
//        std::cout<< "level: "<< current << " "<< current << "\n";
//
//    }
//
//    else {
//            AllCombos(target, current+1 );
//            std::cout<< "==================\n";
//    }
//}

int main(){
//    AllCombos(4, 0, 1);

    std::ofstream main_outdata;

    main_outdata.open("13R-output-10runs.txt");
//    std::size_t pos{};
//    std::string s;
//    std::cin >> s;
    int N= 10;
    int start=1;
    for (int i=start; i<N; i++){
        std::string run= std::to_string(i+1);
        int right_pairs_count=attack(run);
        main_outdata<< "run "<< (i+1)<< ": "<< right_pairs_count<< "\n";
    }

    main_outdata.close();

    return 0;

}

