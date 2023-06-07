#ifndef F_CPU
#define F_CPU	16000000ul
#endif
#include <util/delay.h>

#include "servo.h"

void init(void)
{
	init_servo();
}

int main(void)
{
	init ();
	while (1)
	{
			servo1_set_percentage(100);
			_delay_ms (1000);
			servo1_set_percentage(-100);
            _delay_ms (10000);
	}
	return 0;
}
