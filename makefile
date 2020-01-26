CC = gcc
CFLAGS  = -Wall -pedantic
DEPS = threadbank.h desk.h
OBJ = threadbank.o desk.o

# Define executables to be built
all: threadbank logger clean

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Compile threadbank executable
threadbank: $(OBJ) lock.o
	$(CC) -o threadbank threadbank.o desk.o lock.o -lpthread

# Compile log executable
logger: logger.c lock.o
	$(CC) -o logger logger.c lock.c $(CFLAGS)

# Clean
clean:
	rm *.o
