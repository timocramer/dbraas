#include <stdio.h>
#include <time.h>

#define START_SLEEP_TIME 10
#define MAXIMUM_SLEEP_TIME (5 * 60)

unsigned int get_sleep_time(void) {
	static unsigned int sleep_time = START_SLEEP_TIME;
	static time_t reset = 0;
	
	time_t now = time(NULL);
	
	if(now >= reset) {
		sleep_time = START_SLEEP_TIME;
	} else {
		sleep_time = sleep_time * 3 / 2;
	}
	
	// maximum waiting time is 5 minutes
	if(sleep_time > MAXIMUM_SLEEP_TIME) {
		sleep_time = MAXIMUM_SLEEP_TIME;
	}
	
	// reset happens after the double of sleep_time
	reset = now + (sleep_time * 2);
	
	return sleep_time;
}
