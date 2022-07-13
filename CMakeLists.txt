cmake_minimum_required(VERSION 3.21)
project(cppLab)

#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD 20)

add_executable(cppLab slim.cpp slim-key.cpp attack.cpp main.cpp key.h verification.cpp cmake-build-debug/Testing/Verification/read-input-dump.txt)

