/*
trig = pin 50 PB3
echo = pin 53 PB0
 */

#include <avr/io.h>
#include <avr/delay.h>

#define trig PB3
#define echo PB0
#define LED1 PB7
#define LED2 PB6
#define LED3 PB5
#define LED4 PB4

int timeout = 26100;

void setupUltraPins(void)
{
    DDRB |= (1<<trig);
    DDRB &= ~(1<<echo);

    DDRB |= ((1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4));
    PORTB |= ((1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4));

}

int getPulse(void)
{

    PORTB |= (1<<trig);
    _delay_us(10);
    PORTB &= ~(1<<trig);
            PORTB &= ~(1<<LED1);

    int tijdecho = 0;
    int knop_ingedrukt = 0;

    //PORTB &= ~(1<<LED2);
    for(tijdecho = 0; tijdecho < timeout;)
    {
        _delay_us(1);
        PORTB &= ~(1<<LED2);
        if (PINB & (1<<echo))
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
                PORTB &= ~(1<<LED3);
                return tijdecho;
            }
        }
    }

    return timeout;

}

int main()
{

    setupUltraPins();

    while(1)
    {
        int tijd = 0;
        tijd = getPulse();
        int Cm = tijd / 58;
        if(Cm <30)
        {
            PORTB &= ~(1<<LED4);
            //PORTB |= (1<<LED1);
        }
        else
        {
            //PORTB &= ~(1<<LED1);
            PORTB |= (1<<LED4);

        }
        _delay_ms(500);
        PORTB |= ((1<<LED1)|(1<<LED2)|(1<<LED3));

        _delay_ms(500);
    }

    return 0;
}
