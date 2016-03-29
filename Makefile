MAKEFLAGS += -r

NUMBER_FILE = /var/local/dbr_number.txt

CFLAGS = -O2 -flto -Wall -Wextra -Wshadow -Wstrict-overflow \
	-std=c11 -Wpedantic -D_POSIX_C_SOURCE=200809L \
	-DNUMBER_FILE='"'$(NUMBER_FILE)'"'

HEADERS = $(wildcard *.h)

BINARY = dbraas
INSTALLDIR = /usr/local/bin/

.PHONY: all clean install install-number-file

all: $(BINARY)

$(BINARY): dbraas.o rpi-gpio.o backoff.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(BINARY) *.o

# install with suid-bit
install:
	install -s -m 4755 $(BINARY) $(INSTALLDIR)

install-number-file:
	echo "201" > $(NUMBER_FILE)
	chmod 600 $(NUMBER_FILE)
