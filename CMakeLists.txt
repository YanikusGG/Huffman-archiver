project(archiver)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -O2")

add_executable(archiver
    main.cpp
	coder.h
	coder.cpp
	huffmantree.h
	huffmantree.cpp
	bitstream.h
	bitstream.cpp
	huffman_constants.h
)
