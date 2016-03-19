#ifndef RPI_GPIO
#define RPI_GPIO

#include <stdint.h>

void gpio_init(void);

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
void gpio_fsel(unsigned int pin, uint32_t function);

enum {
	GPIO_PULL_UP = 2,
	GPIO_PULL_DOWN = 1,
	GPIO_OFF = 0,
};
void gpio_pull(unsigned int pin, uint32_t pull_value);
int gpio_level(unsigned int pin);

#endif
