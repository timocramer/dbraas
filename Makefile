MAKEFLAGS += -r

CFLAGS = -O2 -flto -Wall -Wextra -Wshadow -Wstrict-overflow -std=c11 -D_POSIX_C_SOURCE=200809L -Wpedantic

HEADERS = $(wildcard *.h)

BINARY = dbraas
INSTALLDIR = /usr/local/bin/

.PHONY: all clean install

all: $(BINARY)

$(BINARY): dbraas.o rpi-gpio.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(BINARY) *.o

install:
	install -s $(BINARY) $(INSTALLDIR)
