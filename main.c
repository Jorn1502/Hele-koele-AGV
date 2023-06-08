/*
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define stepR 3                              // OC5C PL3 pin 46
#define stepL 5                              // OC1A PB5 pin 11
#define stop 0                               // PE0 pin 0           eneble pin steppers high

int basissnelheid = 20000;                    // basissnelheid
int speed;                                   // past het prog aan

int main(void)
{
    setupSteppers();                         // steppers initialiseren
    speed = basissnelheid;

    while(1)
    {
        line();
    }

    return 0;
}

int setupSteppers(void)
{
    DDRL |= (1 << stepR);                  // PWM steppers aan
    DDRB |= (1 << stepL);
    DDRE |= (1 << stop);


    TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << WGM11) | (0 << WGM10);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);

    TCCR5A = (1 << COM5A1) | (0 << COM5A0) | (1 << WGM51) | (0 << WGM50);
    TCCR5B = (1 << WGM53) | (1 << WGM52) | (0 << CS52) | (0 << CS51) | (1 << CS50);

    OCR1A = 2000;
    OCR5A = 2000;
    ICR1 = 20000;			                       // 32000 is 5ms 16000 is 1ms
    ICR5 = 20000;

    DDRG |= (1 << 2);                    // GND voor linesensor
    PORTG &= ~(1 << 2);                  // maakt port GND
    DDRC &= ~(1 << 0) | ~(1 << 2) | ~(1 << 4) | ~(1 << 6);  // ingang sensor
    DDRA &= ~(1 << 7) | ~(1 << 5) | ~(1 << 3) | ~(1 << 1);  // same

}

int line(void)
{

    int sensoren = 0;


    if(PINC & (1 << 4))
    {
        sensoren += 0b0001;
    }
    if(PINC & (1 << 6))
    {
        sensoren += 0b0010;
    }
    if(PINA & (1 << 7))
    {
        sensoren += 0b0100;
    }
    if(PINA & (1 << 5))
    {
        sensoren += 0b1000;
    }
    switch(sensoren)
    {
    case(0b0001):
        ICR1 = 20000;
        ICR5 = 60000;
        break;
    case(0b0011):
        ICR1 = 20000;
        ICR5 = 40000;
        break;
    case(0b0111):
        ICR1 = 20000;
        ICR5 = 30000;
        break;
    case(0b0110):
        ICR1 = 18000;
        ICR5 = 18000;
        break;
    case(0b1110):
        ICR1 = 30000;
        ICR5 = 20000;
        break;
    case(0b1100):
        ICR1 = 40000;
        ICR5 = 20000;
        break;
    case(0b1000):
        ICR1 = 60000;
        ICR5 = 20000;
        break;

    default:
        ICR1 = ICR5 = 63530;
    }
}
