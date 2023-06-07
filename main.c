/*
 */

//timer 1 3 zijn 16 Mhz
//16.000.000 / 500 = 32 000                 500 Hz = 5ms
//65535 - 32000 = 33 535                    startgetal timer (prescaler niet nodig
//32000 * 0.1 = 3200                        timer duty


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "servo.h"

#define TCNT_INIT 33535
#define OC_INIT ((TCNT_INIT) + 3200)

#define stepR 3                              // OC5C PL3 pin 46
#define stepL 5                              // OC1A PB5 pin 11
#define stop 0                               // PE0 pin 0           eneble pin steppers high

#define trig 2                               // PB2 pin 51
#define echo 0                               // PB0 pin 53

int timeout = 26100;                          // tijd tot max afstand
int basissnelheid = 30000;                    // basissnelheid
int speed;                                    // past het prog aan

int main(void)
{
    setupSteppers();
    setupUltraPins();
    init_servo();

    while(1)
    {
        //ultrasoon();
        line();
        rijden();
    }

    return 0;
}


void setupSteppers(void)
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
//    ICR1;			                       // 32000 is 5ms 16000 is 1ms
//    ICR5;

    DDRG |= (1 << 2);                    // GND voor linesensor
    PORTG &= ~(1 << 2);                  // maakt port GND
    DDRC &= ~(1 << 0) | ~(1 << 2) | ~(1 << 4) | ~(1 << 6);  // ingang sensor
    DDRA &= ~(1 << 7) | ~(1 << 5) | ~(1 << 3) | ~(1 << 1);  // same

}
void setupUltraPins(void)
{
    DDRB |= (1 << trig);                // trigger output
    DDRB &= ~(1 << echo);               // echo input
    DDRL |= (1 << 0);                   // pin voor 5V
    PORTL |= (1 << 0);                  // 5v
}


int getPulse(void)
{

    PORTB |= (1 << trig);
    _delay_us(10);
    PORTB &= ~(1 << trig);

    int tijdecho = 0;
    int knop_ingedrukt = 0;


    for(tijdecho = 0; tijdecho < timeout;)
    {
        _delay_us(1);

        if (PINB & (1 << echo))
        {
            if (knop_ingedrukt == 0) // knop is niet al eerder ingedrukt
            {
                knop_ingedrukt = 1;
            }
        }
        if (knop_ingedrukt != 0) // knop is zojuist losgelaten
        {
            tijdecho++;
        }
        if ((PINB & (1<<echo)) == 0)
        {
            if (knop_ingedrukt != 0) // knop is zojuist losgelaten
            {
                knop_ingedrukt = 0;
                return tijdecho;
            }
        }
    }

    return timeout;

}
void ultrasoon(void)
{
    int tijd = 0;
    tijd = getPulse();
    int Cm = tijd / 58;
    if(Cm <30)
    {
        PORTE |= (1 << stop);
    }
    else
    {
        PORTE &= ~(1 << stop);
    }
}

int line(void)
{
    int speed = basissnelheid;

    if(PINC & (1 << 0))
    {
        speed = speed + 1000;
    }
    if(PINC & (1 << 2))
    {
        speed = speed + 500;
    }
    if(PINC & (1 << 4))
    {
        speed = speed + 100;
    }
    if(PINC & (1 << 6))
    {
        speed = speed + 20;
    }
    if(PINA & (1 << 1))
    {
        speed = speed - 1000;
    }
    if(PINA & (1 << 3))
    {
        speed = speed - 500;
    }
    if(PINA & (1 << 5))
    {
        speed = speed - 100;
    }
    if(PINA & (1 << 7))
    {
        speed = speed - 20;
    }
    if((PINC & (1 << 6)) & (PINA & (1 << 7)))
    {
        speed = basissnelheid;
    }
}

void rijden(void)
{
    ICR1 = speed;                               // stel snelheid motor in
    ICR5 = (basissnelheid*2) - speed;           // doet tegenovergestelde van ICR1
}
