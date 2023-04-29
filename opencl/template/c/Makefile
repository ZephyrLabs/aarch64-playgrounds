CC=gcc

SRC=main.c
OUT=main

INCLUDE=/usr/include/CL
CCFLAGS=-lOpenCL

CLFLAGS=-D CL_TARGET_OPENCL_VERSION=210

default:
	$(CC) -Wall -Wextra -I $(INCLUDE) $(CLFLAGS) $(SRC) -o $(OUT) $(CCFLAGS)

clean:
	rm -rf main
