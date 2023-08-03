CC=gcc

LIB=cl_init
SRC=main
OUT=main

INCLUDE=/usr/include/CL
CCFLAGS=-lOpenCL

CLFLAGS=-D CL_TARGET_OPENCL_VERSION=210 -c

default:
	$(CC) -Wall -Wextra -I $(INCLUDE) $(CLFLAGS) $(LIB).c -o $(LIB).o $(CCFLAGS)
	$(CC) -Wall -Wextra -I $(INCLUDE) $(CLFLAGS) $(SRC).c -o $(SRC).o $(CCFLAGS)
	$(CC) -o $(OUT) $(SRC).o $(LIB).o $(CCFLAGS)

clean:
	rm -rf *.o
	rm -rf main
