/*
 */

#include <avr/io.h>
#include <util/delay.h>

#define LED1 PB7
#define LED2 PB6
#define LED3 PB5
#define LED4 PB4

/*
IR    PC1    Dpin 37
D2    PC2    Dpin 35
D3    PC3    Dpin 34
D4    PC4    Dpin 33
D5    PC5    Dpin 32
D6    PC6    Dpin 31
D7    PC7    Dpin 30
*/

void init(void)
{
    DDRB |= ((1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4));
    PORTB |= ((1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4));

    DDRC &= ~((1<<PC2)|(1<<PC3)|(1<<PC4)|(1<<PC5)|(1<<PC6)|(1<<PC7));
}

int main(void)
{
    init();
    int sensoren = 0;

    while(1)
    {
        if(PINC & (1<<PC3))
        {
            sensoren += 0b0001;
        }
        if(PINC & (1<<PC4))
        {
            sensoren += 0b0010;
        }
        if(PINC & (1<<PC5))
        {
            sensoren += 0b0100;
        }
        if(PINC & (1<<PC6))
        {
            sensoren += 0b1000;
        }

        switch(sensoren)
        {
        case(0b0001):
            //stuur ver naar links
            break;
        case(0b0011):
            //stuur beetje naar links
            break;
        case(0b0111):
            //stuur klein beetje naar links
            break;
        case(0b0110):
            // percies in het midden
            break;
        case(0b1110):
            // stuur klein beetje naar rechts
            break;
        case(0b1100):
            //stuur beetje naar recht
            break;
        case(0b1000):
            //stuur ver naar rechts
            break;

        default:
            //rij maar gewoon rechtdoor
            // of pas niet aan
        }

        PORTB &= ~(sensoren<<4);
        _delay_ms(10);
        PORTB |= ((1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4));
        sensoren = 0b0;
    }

    return 0;
}
