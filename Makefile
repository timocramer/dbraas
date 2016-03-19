MAKEFLAGS += -r

CFLAGS = -O2 -flto -Wall -Wextra -Wshadow -Wstrict-overflow -std=c11 -D_POSIX_C_SOURCE=200809L -Wpedantic

HEADERS = $(wildcard *.h)

.PHONY: all clean

all: dbraas

dbraas: dbraas.o rpi-gpio.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) dbraas *.o
