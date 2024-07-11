cmake_minimum_required(VERSION 3.23)
project(lab4)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(lab4
        CommandsParser.cpp
        CommandsParser.h
        archive.cpp
        archive.h
        Hamming.h
        Hamming.cpp
        InOutBit.h
        InOutBit.cpp
        main.cpp)

