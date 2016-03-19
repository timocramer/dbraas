#ifndef RPI_GPIO
#define RPI_GPIO

#include <stdint.h>

enum {
	GPIO_INPUT = 0,
	GPIO_OUTPUT = 1,
	GPIO_ALT0 = 4,
	GPIO_ALT1 = 5,
	GPIO_ALT2 = 6,
	GPIO_ALT3 = 7,
	GPIO_ALT4 = 3,
	GPIO_ALT5 = 2,
};

void gpio_init(void);
void gpio_fsel(unsigned int pin, uint32_t function);
void gpio_pull_up(unsigned int pin);
int gpio_level(unsigned int pin);

#endif
