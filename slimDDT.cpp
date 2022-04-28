#include <iostream>
//using namespace std;
// https://www.scaler.com/topics/passing-array-to-function-in-c-cpp/#passing-multidimensional-array-to-a-function-in-c/c--
// https://stackoverflow.com/questions/19908456/c-subscripted-value-is-neither-array-nor-pointer-nor-vector-when-assigning-an-ar
// https://stackoverflow.com/questions/49414841/process-finished-with-exit-code-139-interrupted-by-signal-11-sigsegv

int print_screen(int arr[16][16][5]){
    for (int m=0; m<16; m++){
        std::cout << m << "--"<< std::endl;

        for (int n=0; n<16; n++){
            std::cout<< "{" << "";

            for (int k=0; k<5; k++){
                std::cout<< arr[m][n][k] << ", ";

            }
            std::cout<< "}" << std::endl << "";

        }
        std::cout<< std::endl << "";


    }
    return 1;
}

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
//            std::cout << length_of_input_X << std::endl;
            input_diff[xor_X][arr_index[xor_X]][0]= i;
            input_diff[xor_X][arr_index[xor_X]][1]= j;
            input_diff[xor_X][arr_index[xor_X]][2]= sBox[i];
            input_diff[xor_X][arr_index[xor_X]][3]= sBox[j];
            input_diff[xor_X][arr_index[xor_X]][4]= sBox[i] ^ sBox[j];

            arr_index[xor_X]+=1;

        }

    }
//    print_screen( input_diff);

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
    std::cout<< "{";
    for (int q=0; q<16; q++){
        std::cout<< ddt[p][q] << ", ";
    }
    std::cout<< "}" << std::endl;
}
    return 0;
}

//            int pentlep[5]={};
//
//            pentlep[0]= i;
//            pentlep[1]= j;
//            pentlep[2]= sBox[i];
//            pentlep[3]= sBox[j];
//            pentlep[4]= sBox[i] ^ sBox[j];
//
//            std::cout << "{ ";
//            for (int p=0; p<5; p++){
//                std::cout<< pentlep[p] << ", ";
//            }
//
//            std::cout << " },"<< std::endl;