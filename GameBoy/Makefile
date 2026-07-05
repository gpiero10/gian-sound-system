CC = gcc

CFLAGS = -Wall -Wextra -std=c17 -g -Iincludes

SRC = \
	src/bus.c \
	src/cartridge.c \
	src/cpu.c \
	src/cpu_fetch.c \
	src/cpu_proc.c \
	src/cpu_utils.c \
	src/devices.c \
	src/instrucciones.c \
	src/interrupciones.c \
	src/timer.c \
	src/emu.c

TEST_SRC = src/tests.c

OBJ = $(SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)

TARGET = gameboy
TEST_TARGET = gbtest

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

$(TEST_TARGET): $(OBJ) $(TEST_OBJ)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET) $(TEST_TARGET)

.PHONY: all clean