cmake_minimum_required(VERSION 3.21)
project(slim_cipher)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        slim_cipher
        slim.cpp
        slim-key.cpp
        attack.cpp verification.cpp
        main.cpp
        key.h )

