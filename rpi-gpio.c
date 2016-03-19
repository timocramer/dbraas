#include "rpi-gpio.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BLOCK_SIZE (4*1024)
#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

static volatile uint32_t *gpio_base;

// inspired by http://elinux.org/RPi_GPIO_Code_Samples
void gpio_init(void) {
	/* open /dev/mem */
	int mem_fd = open("/dev/mem", O_RDWR|O_SYNC);
	if(mem_fd == -1) {
		fprintf(stderr, "can't open /dev/mem: %s\n", strerror(errno));
		exit(1);
	}
	
	/* mmap GPIO */
	void *gpio_map = mmap(
		NULL,             //Any address in our space will do
		BLOCK_SIZE,       //Map length
		PROT_READ|PROT_WRITE, // Enable reading & writting to mapped memory
		MAP_SHARED,       //Shared with other processes
		mem_fd,           //File to map
		GPIO_BASE         //Offset to GPIO peripheral
	);
	
	close(mem_fd); //No need to keep mem_fd open after mmap
	
	if(gpio_map == MAP_FAILED) {
		fprintf(stderr, "mmap failed: %s\n", strerror(errno));
		exit(1);
	}
	
	// Always use volatile pointer!
	gpio_base = (volatile uint32_t *)gpio_map;
}
