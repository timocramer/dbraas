#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "rpi-gpio.h"

#define PRINTER_PATH "/dev/usb/lp0"

// both are "nobody" on my system
#define UID_AFTER_DROP 99
#define GID_AFTER_DROP 99
static void drop_privileges(void) {
	if(setgid(GID_AFTER_DROP) == -1) {
		perror("setgid");
		exit(1);
	}
	if(setuid(UID_AFTER_DROP) == -1) {
		perror("setuid");
		exit(1);
	}
}

int main(void) {
	gpio_init();
	drop_privileges();
	
	gpio_fsel(17, GPIO_INPUT);
	gpio_pull(17, GPIO_PULL_UP);
	while(1) {
		const struct timespec sleeptime = {.tv_sec = 0, .tv_nsec = 1000000};
		
		printf("%d\n", gpio_level(17));
		nanosleep(&sleeptime, NULL);
	}
	
	return 0;
}
