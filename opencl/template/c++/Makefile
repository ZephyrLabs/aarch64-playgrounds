CC=g++

SRC=main.cpp
OUT=main

INCLUDE=/usr/include/CL
CXXFLAGS=-lOpenCL

CLFLAGS=-D CL_HPP_TARGET_OPENCL_VERSION=210

default:
	$(CC) -Wall -Wextra -I $(INCLUDE) $(CLFLAGS) $(SRC) -o $(OUT) $(CXXFLAGS)

clean:
	rm -rf main
