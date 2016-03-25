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

static FILE *printer;

static void print_du_bist_raus(void) {
	fprintf(printer, "\r\n\r\n"
			"      Du bist raus.\r\n");
	
	unsigned int number = read_and_increment_number();
	if(number != 0) {
		fprintf(printer, "          #%u\r\n", number);
	}
	
	fprintf(printer, "\r\n\r\n\r\n");
	fflush(printer);
}

#define PRINTER_PATH "/dev/usb/lp0"

#define BUTTON_PIN 17

int main(void) {
	gpio_init();
	
	// init printer connection
	printer = fopen(PRINTER_PATH, "w");
	if(printer == NULL) {
		perror("cannot open printer connection");
		return 1;
	}
	
	// open number file, error handling happens in read_and_increment_number
	number_file = fopen(NUMBER_FILE, "r+");
	
	drop_privileges();
	
	gpio_fsel(BUTTON_PIN, GPIO_INPUT);
	gpio_pull(BUTTON_PIN, GPIO_PULL_UP);
	
	const int default_level = 0; // we use a pull up and an opening switch
	while(1) {
		int current_level = gpio_level(BUTTON_PIN);
		
		if(current_level != default_level) {
			print_du_bist_raus();
			
			sleep(get_sleep_time()); // don't overuse
		}
		
		// wait for 5ms
		nanosleep(&(const struct timespec){.tv_sec = 0, .tv_nsec = 5000000}, NULL);
	}
	
	return 0;
}
