/*
 */

#include <avr/io.h>

#define sensor1 PK1
#define LED1 PB7

int sensorVal = 0;

void init()
{
    DDRK &= ~(1<<sensor1);
    DDRB |= (1<<LED1);
    PORTB |= (1<<LED1);
}

void getBoom(void)
{
    sensorVal =
}

int main(void)
{


    while(1)
    {
       getBoom();
    }

    return 0;
}
