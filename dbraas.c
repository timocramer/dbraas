#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "rpi-gpio.h"
#include "backoff.h"

// both are "nobody" on my system
#define UID_AFTER_DROP 99
#define GID_AFTER_DROP 99
static void drop_privileges(void) {
	if(chdir("/") == -1) {
		perror("chdir");
		exit(1);
	}
	if(setgid(GID_AFTER_DROP) == -1) {
		perror("setgid");
		exit(1);
	}
	if(setuid(UID_AFTER_DROP) == -1) {
		perror("setuid");
		exit(1);
	}
}

static FILE *number_file;

static unsigned int read_and_increment_number(void) {
	// if it is not open, don't use the number
	if(number_file == NULL) {
		return 0;
	}
	
	// get to the start of the file and try to read the number
	rewind(number_file);
	
	unsigned int number;
	int result = fscanf(number_file, "%u", &number);
	if(result != 1) {
		fprintf(stderr, "fscanf failed\n");
		return 0;
	}
	
	// the first 200 are already gone
	if(number <= 200) {
		return 0;
	}
	
	// write back the new number
	rewind(number_file);
	fprintf(number_file, "%u", number + 1);
	fflush(number_file);
	
	return number;
}

#define PRINTER_PATH "/dev/usb/lp0"

#define BUTTON_PIN 17

int main(void) {
	gpio_init();
	
	// init printer connection
	FILE *printer = fopen(PRINTER_PATH, "w");
	if(printer == NULL) {
		perror("cannot open printer connection");
		return 1;
	}
	
	// open number file, error handling happens in read_and_increment_number
	number_file = fopen(NUMBER_FILE, "r+");
	
	drop_privileges();
	
	gpio_fsel(BUTTON_PIN, GPIO_INPUT);
	gpio_pull(BUTTON_PIN, GPIO_PULL_UP);

    // Set output lines for easier templating
    const char line1[] = "\xd5\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xb8";
    const char line2[] = "\xb3                      \xb3";
    const char line3[] = "\xb3     Du bist raus.    \xb3";
    const char line4pre[] = "\xb3         #";
    const char line4post[] = "         \xb3";
    const char line5[] = "\xb3                      \xb3";
    const char line6[] = "\xd4\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbe";
	
	const int default_level = 0; // we use a pull up and an opening switch
	while(1) {
		int current_level = gpio_level(BUTTON_PIN);
		
		if(current_level != default_level) {
            fprintf(printer, line1);
            fprintf(printer, "\r\n");
            fprintf(printer, line2);
            fprintf(printer, "\r\n");
            fprintf(printer, line3);
            fprintf(printer, "\r\n");
            fprintf(printer, line4pre);
			unsigned int number = read_and_increment_number();
			if(number != 0) {
				fprintf(printer, "%u", number);
			}
            fprintf(printer, line4post);
            fprintf(printer, "\r\n");
            fprintf(printer, line5);
            fprintf(printer, "\r\n");
            fprintf(printer, line6);
            fprintf(printer, "\r\n");
			fprintf(printer, "\r\n\r\n\r\n");
			fflush(printer);
			
			sleep(get_sleep_time()); // don't overuse
		}
		
		// wait for 5ms
		nanosleep(&(const struct timespec){.tv_sec = 0, .tv_nsec = 5000000}, NULL);
	}
	
	return 0;
}
