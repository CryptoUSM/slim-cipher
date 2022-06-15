#include <iostream>
//using namespace std;
// https://www.scaler.com/topics/passing-array-to-function-in-c-cpp/#passing-multidimensional-array-to-a-function-in-c/c--
// https://stackoverflow.com/questions/19908456/c-subscripted-value-is-neither-array-nor-pointer-nor-vector-when-assigning-an-ar
// https://stackoverflow.com/questions/49414841/process-finished-with-exit-code-139-interrupted-by-signal-11-sigsegv

int main() {
    int sBox[16] = {12, 5, 6, 11, 9, 0, 10, 13, 3, 14, 15, 8, 4, 7, 1, 2};
    int input_diff[16][16][5]= {
//            {{0, 0, 12, 12, 0}, {1, 1, 5, 5, 0},},
//            {{0, 1, 12, 5, 9}, {1, 0, 5, 12, 9},},
    };
    int ddt[16][16]={};
    int arr_index[16]= {};


    // construct deltaX and deltaY
    // {X1, X2, Y1, Y2, deltaY}
    for (int i=0; i<16; i++){
        for (int j=0; j< 16; j++){
            int X1= i;
            int X2= j;

            int xor_X= i^j;

//            int length_of_input_X= sizeof input_diff[xor_X][0] / sizeof(int);
//            std::std::cout << length_of_input_X << std::std::endl;
            input_diff[xor_X][arr_index[xor_X]][0]= i;
            input_diff[xor_X][arr_index[xor_X]][1]= j;
            input_diff[xor_X][arr_index[xor_X]][2]= sBox[i];
            input_diff[xor_X][arr_index[xor_X]][3]= sBox[j];
            input_diff[xor_X][arr_index[xor_X]][4]= sBox[i] ^ sBox[j];

            arr_index[xor_X]+=1;

        }

    }

for (int a=0; a<16; a++){
    for (int b=0; b<16; b++){
        int current_count=0; // count of a pair of x,y

        int output= input_diff[a][b][4];
        current_count= ddt[a][output];


        if (current_count==0){
            current_count=1;
        }

        else{
            current_count= current_count+1;

        }
        ddt[a][output]= current_count;

    }


}


for (int p=0; p< 16; p++){
    for (int q=0; q<16; q++){
        std::cout<< ddt[p][q] << ", ";
    }
    std::cout<< "\n";

}

}
