/*
8 000 000 / 10 = 800 000                    // Timer is 8 MHz ik wil 10 Hz hebben
800 000 / 32 = 25 000                       // prescaler is 32
65 35 - 25 000 = 40 535                     // startwaarde timer om de gewenste tijd te krigen
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define stepR 3                             // OC5C PL3 pin 46
#define stepL 5                             // OC1A PB5 pin 11
#define stop 0                              // PE0 pin 0           eneble pin steppers high


#define trig PB2
#define echo PB0

#define TCNT2_INIT 20535                    // startwaarde timer

int basissnelheid = 20000;                  // basissnelheid
int speed;                                  // past het prog aan

int timeout = 1800;

int main(void)
{
    _delay_ms(2000);
    setupSteppers();                        // steppers initialiseren
    setupUltraPins();
    speed = basissnelheid;

    while(1)
    {
        line();

        if(TIFR2 & (1 << TOV2))
        {
            TCNT2 = TCNT2_INIT;
            TIFR2 = (1 << TOV2);

            ultrasoon();
        }

    }
}

int setupSteppers(void)
{
    DDRL |= (1 << stepR);                   // PWM aan (timer 1)
    DDRB |= (1 << stepL);                   // PWM aan (timer 5)
    DDRE |= (1 << stop);


    TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << WGM11) | (0 << WGM10);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);

    TCCR5A = (1 << COM5A1) | (0 << COM5A0) | (1 << WGM51) | (0 << WGM50);
    TCCR5B = (1 << WGM53) | (1 << WGM52) | (0 << CS52) | (0 << CS51) | (1 << CS50);

    OCR1A = 2000;
    OCR5A = 2000;
    ICR1 = 20000;			            // 32000 is 5ms 16000 is 1ms
    ICR5 = 20000;

    DDRB |= (1 << 3);                   // eneble pin rechts
    DDRL |= (1 << 1);                   // eneble pin links
    PORTB &= ~(1 << 3);                 // zet driver rechts aan
    PORTL &= ~(1 << 1);                 // zet driver links aan

    DDRG |= (1 << 2);                   // PS pin voor linesensor
    PORTG &= ~(1 << 2);                 // GND supply
    DDRC &= ~(1 << 0) | ~(1 << 2) | ~(1 << 4) | ~(1 << 6);  // ingang sensor
    DDRA &= ~(1 << 7) | ~(1 << 5) | ~(1 << 3) | ~(1 << 1);  // same
}

int setupUltraPins(void)
{
    DDRB |= (1 << trig);                // output voor trigger
    DDRB &= ~(1 << echo);               // input voor feedback

    DDRL |= (1 << 0);                   // PS pin ultrasoon
    PORTL |= (1 << 0);                  // 5V supply

    TCCR2A = 0;
    TCCR2B = ~(1 << CS22) | (1 << CS21) | (1 << CS20);

    TCNT2 = TCNT2_INIT;
}

int line(void)
{

    int sensoren = 0;

//    PORTB &= ~(1 << 3);                 // zet driver rechts aan
//    PORTL &= ~(1 << 1);                 // zet driver links aan;


    if(PINC & (1 << 0))
    {
        sensoren += 0b00000001;
    }
    if(PINC & (1 << 2))
    {
        sensoren += 0b00000010;
    }
    if(PINC & (1 << 4))
    {
        sensoren += 0b00000100;
    }
    if(PINC & (1 << 6))
    {
        sensoren += 0b00001000;
    }
    if(PINA & (1 << 7))
    {
        sensoren += 0b00010000;
    }
    if(PINA & (1 << 5))
    {
        sensoren += 0b00100000;
    }
        if(PINA & (1 << 3))
    {
        sensoren += 0b01000000;
    }
    if(PINA & (1 << 1))
    {
        sensoren += 0b10000000;
    }

    switch(sensoren)
    {
    case(0b00000001):
        ICR1 = 20000;
        PORTL |= (1 << 1);
        break;
    case(0b00000011):
        ICR1 = 20000;
        ICR5 = 63530;
        break;
    case(0b00000111):
        ICR1 = 20000;
        ICR5 = 53000;
        break;
    case(0b00000110):
        ICR1 = 20000;
        ICR5 = 40000;
        break;
    case(0b00001110):
        ICR1 = 20000;
        ICR5 = 30000;
        break;
    case(0b00001100):
        ICR1 = 20000;
        ICR5 = 25000;
        break;
    case(0b00011100):
        ICR1 = 20000;
        ICR5 = 22000;
        break;
    case(0b00011000):
        ICR1 = 20000;
        ICR5 = 20000;
        break;
    case(0b00111000):
        ICR1 = 22000;
        ICR5 = 20000;
        break;
    case(0b00110000):
        ICR1 = 25000;
        ICR5 = 20000;
        break;
    case(0b01110000):
        ICR1 = 30000;
        ICR5 = 20000;
        break;
    case(0b01100000):
        ICR1 = 40000;
        ICR5 = 20000;
        break;
    case(0b11100000):
        ICR1 = 53000;
        ICR5 = 20000;
        break;
    case(0b11000000):
        ICR1 = 63530;
        ICR5 = 20000;
        break;
    case(0b10000000):
        PORTB |= (1 << 3);
        ICR5 = 20000;
        break;

    default:
        ICR1 = ICR5 = 63530;                  //default langzaam rijden
    }
}


int getPulse(void)
{
    int knop_ingedrukt = 0;
    int tijdecho = 0;

    PORTB |= (1 << trig);
    _delay_us(10);
    PORTB &= ~(1 << trig);

   while(tijdecho < timeout)
    {
       if ((PINB & (1 << echo)) == 1)
        {
            if (knop_ingedrukt == 0) // knop is niet al eerder ingedrukt
            {
                knop_ingedrukt = 1;
            }
        }
        if (knop_ingedrukt != 0) // knop is zojuist losgelaten
        {
            tijdecho ++;
        }
        if ((PINB & (1 << echo)) == 0)
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

int ultrasoon(void)
{
    int tijd = timeout;
    tijd = getPulse();
    int Cm = tijd / 58;

    if(Cm < 10)
    {
    PORTB |= (1 << 3);                 // zet driver rechts uit
    PORTL |= (1 << 1);                 // zet driver links uit

            DDRB |= (1 << 7);
            PORTB &= ~(1 << 7);
    }
    else
    {
    PORTB &= ~(1 << 3);                 // zet driver rechts aan
    PORTL &= ~(1 << 1);                 // zet driver links aan;
    }
}

