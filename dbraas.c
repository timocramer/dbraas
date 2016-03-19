#include <stdio.h>
#include <stdlib.h>
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
	
	
	return 0;
}
