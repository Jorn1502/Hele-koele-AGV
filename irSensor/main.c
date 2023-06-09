/*
 */

#include <avr/io.h>

#define sensor1 PK1
#define LED1 PB7
#define irSensor PK0

//PK0 ( ADC8/PCINT16 ) Analog pin 8 MUX5:0 100000


int sensorVal = 0;

void init()
{
    DDRK &= ~(1<<sensor1);
    DDRB |= (1<<LED1);
    PORTB |= (1<<LED1);

    ADMUX = (0 << REFS1) | (1 << REFS0);
    ADCSRB |= (1<<MUX5);
    ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);


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
