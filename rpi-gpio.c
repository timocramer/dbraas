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

void gpio_fsel(unsigned int pin, uint32_t function) {
	if(pin > 53) {
		return;
	}
	
	// find corresponding bits (gpio_base is GPFSEL0)
	volatile uint32_t *reg_addr = gpio_base + (pin / 10);
	const unsigned int shift = (pin % 10) * 3;
	
	// function has to be between 0 and 7
	function &= 0x7;
	
	// build new register value
	uint32_t reg_tmp = *reg_addr;
	reg_tmp &= ~(0x7 << shift);
	reg_tmp |= function << shift;
	
	// write it back
	*reg_addr = reg_tmp;
}

void gpio_pull_down(unsigned int pin) {
	if(pin > 53) {
		return;
	}
	
	// find corresponding GPPUD-registers
	volatile uint32_t *gppud = gpio_base + 37;
	volatile uint32_t *gppudclk = gpio_base + 38 + (pin / 32);
	
	// find corresponding shift value
	unsigned int shift = pin;
	if(shift >= 32) {
		shift -= 32;
	}
	
	// indicate we want to pull down a pin
	*gppud = 1;
	
	// wait 150 cycles
	for(volatile int i = 0; i < 150; ++i);
	
	// add clock for the pin
	*gppudclk |= (1 << shift);
	
	// wait 150 cycles
	for(volatile int i = 0; i < 150; ++i);
	
	// write to registers to apply changes
	*gppud = 0;
	*gppudclk = 0;
}

int gpio_level(unsigned int pin) {
	if(pin > 53) {
		return 0;
	}
	
	// find corresponding GPLEV-register
	volatile uint32_t *gplev = gpio_base + 13 + (pin / 32);
	
	// find corresponding shift value
	unsigned int shift = pin;
	if(shift >= 32) {
		shift -= 32;
	}
	
	return !!(*gplev & (1 << shift));
}
