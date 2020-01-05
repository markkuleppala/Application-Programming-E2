CC = gcc
CFLAGS  = -Wall -pedantic#-lpthread
DEPS = threadbank.h desk.h
OBJ = threadbank.o desk.o

# Define executables to be built
all: threadbank lock clean

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Compile threadbank executable
threadbank: $(OBJ)
	$(CC) -o threadbank threadbank.o desk.o

# Compile file lock executable
lock:  lock.c
	$(CC) -o lock lock.c $(CFLAGS)

# Clean
clean:
	rm *.o