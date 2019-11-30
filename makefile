# Compiler for C
CC = gcc

# Compiler flags
CFLAGS  = -Wall -pedantic

# Define executables to be built
all: threadbank

# Compile threadbank executable
threadbank:  threadbank.c
	$(CC) $(CFLAGS) -o threadbank threadbank.c -lpthread
