CC = gcc
CFLAGS  = -Wall -pedantic
DEPS = threadbank.h desk.h
OBJ = threadbank.o desk.o

# Define executables to be built
all: threadbank lock logger clean

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Compile threadbank executable
threadbank: $(OBJ)
	$(CC) -o threadbank threadbank.o desk.o -lpthread

# Compile file lock executable
lock: lock.c
	$(CC) -o lock lock.c $(CFLAGS)

# Compile log executable
logger: logger.c
	$(CC) -o logger logger.c $(CFLAGS)

# Clean
clean:
	rm *.o