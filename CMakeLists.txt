cmake_minimum_required(VERSION 3.21)
project(cppLab)

#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD 20)

add_executable(cppLab slim.cpp slim-key.cpp attack.cpp key.h read-input-file.cpp)
#add_executable(cppLab slim/slim-key.cpp slim/key_scheduling.h)

#add_executable(cppLab slimDDT.cpp)

