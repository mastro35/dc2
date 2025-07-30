# SPDX-License-Identifier: GPL-2.0
VERSION = 0
PATCHLEVEL = 1
SUBLEVEL = 0
NAME = 'dc2 (codename Woz)'

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic

TARGET = dc2
SRC = dc2.c

all: clean $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
