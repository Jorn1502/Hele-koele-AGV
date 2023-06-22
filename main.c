/*
8 000 000 / 10 = 800 000                    // Timer is 8 MHz ik wil 10 Hz hebben
800 000 / 32 = 25 000                       // prescaler is 32
65 35 - 25 000 = 40 535                     // startwaarde timer om de gewenste tijd te krigen
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define stepR 3 // OC5C PL3 pin 46
#define stepL 5 // OC1A PB5 pin 11
#define stop 0  // PE0 pin 0           eneble pin steppers high

#define trig PB2
#define echo PB0

#define TCNT2_INIT 20535 // startwaarde timer

int basissnelheid = 20000; // basissnelheid
int speed;                 // past het prog aan

int timeout = 1800;

ISR(INT0_vect)
{
    _delay_ms(20);
    DDRB |= (1 << 7);
    PORTB |= (1 << 3); // eneble pin rechts
    PORTL |= (1 << 1);
    _delay_ms(2000);
}

ISR(INT1_vect)
{
    _delay_ms(20);
    DDRB |= (1 << 7);
    PORTB |= (1 << 3); // eneble pin rechts
    PORTL |= (1 << 1);
    _delay_ms(2000);
}

ISR(INT2_vect)
{
    while ((PORTB & (1 << 3)) != 0)
    {
        _delay_ms(50);
        DDRB |= (1 << 7);
        PORTB ^= (1 << 7);
    }
}

int main(void)
{
    // noodBump();
    setupSteppers(); // steppers initialiseren
    setupUltraPins();
    setupIRSensor(); // ir sensor initialiseren
    _delay_ms(1000);

    speed = basissnelheid;

    while (1)
    {
        line();

        if (TIFR2 & (1 << TOV2))
        {
            TCNT2 = TCNT2_INIT;
            TIFR2 = (1 << TOV2);

            ultrasoon();
        }
    }
}

int noodBump(void)
{
    DDRD &= ~(1 << 2);
    DDRD |= (1 << 3);
    PORTD |= (1 << 3);

    EIMSK |= (1 << INT2);
    EICRA |= (1 << ISC21) | (1 << ISC20);
}

int setupSteppers(void)
{
    DDRL |= (1 << stepR); // PWM aan (timer 1)
    DDRB |= (1 << stepL); // PWM aan (timer 5)
    DDRE |= (1 << stop);

    TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << WGM11) | (0 << WGM10);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10); // cs*1 aan en cs*0 uit geeft prescaler 8 ipv 1

    TCCR5A = (1 << COM5A1) | (0 << COM5A0) | (1 << WGM51) | (0 << WGM50);
    TCCR5B = (1 << WGM53) | (1 << WGM52) | (0 << CS52) | (0 << CS51) | (1 << CS50);

    OCR1A = 2000;
    OCR5A = 2000;
    // ICR1 = 20000; // 32000 is 5ms 16000 is 1ms
    // ICR5 = 20000;

    DDRB |= (1 << 3);   // eneble pin rechts
    DDRL |= (1 << 1);   // eneble pin links
    PORTB &= ~(1 << 3); // zet driver rechts aan
    PORTL &= ~(1 << 1); // zet driver links aan

    DDRG |= (1 << 2);                                      // PS pin voor linesensor
    PORTG &= ~(1 << 2);                                    // GND supply
    DDRC &= ~(1 << 0) | ~(1 << 2) | ~(1 << 4) | ~(1 << 6); // ingang sensor
    DDRA &= ~(1 << 7) | ~(1 << 5) | ~(1 << 3) | ~(1 << 1); // same
}

int setupUltraPins(void)
{
    DDRB |= (1 << trig);  // output voor trigger
    DDRB &= ~(1 << echo); // input voor feedback

    DDRL |= (1 << 0);  // PS pin ultrasoon
    PORTL |= (1 << 0); // 5V supply

    TCCR2A = 0;
    TCCR2B = ~(1 << CS22) | (1 << CS21) | (1 << CS20);

    TCNT2 = TCNT2_INIT;
}

int setupIRSensor(void)
{
    sei();
    EIMSK |= (1 << INT0);
    EICRA |= (1 << ISC01) | (0 << ISC00);
    EIMSK |= (1 << INT1);
    EICRA |= (1 << ISC11) | (0 << ISC10);

    // links
    DDRD &= ~(1 << 0);           // input vanaf ir links
    DDRA |= (1 << 6) | (1 << 4); // PS voor ir links
    PORTA &= ~(1 << 6);          // GND voor ir links
    PORTA |= (1 << 4);           // 5V voor ir links

    // rechts
    DDRD &= ~(1 << 1);           // input vanaf ir rechts
    DDRA |= (1 << 0) | (1 << 2); // PS voor ir rechts
    PORTA &= ~(1 << 2);          // GND voor ir rechts
    PORTA |= (1 << 0);           // 5V voor ir rechts
}

int line(void)
{

    int sensoren = 0;
    int Rechts = 1;
    int Links = 2;
    int Voor = 3;
    //    PORTB &= ~(1 << 3);                 // zet driver rechts aan
    //    PORTL &= ~(1 << 1);                 // zet driver links aan;

    int centerPin1 = PINC & (1 << 6);
    int centerPin2 = PINA & (1 << 7);

    // int rechterPin3 = PINC & (1 << 4);
    // int linkerPin1 = PINA & (1 << 5);

    int rechterPin1 = PINC & (1 << 0);
    int rechterPin2 = PINC & (1 << 2);

    int linkerPin2 = PINA & (1 << 3);
    int linkerPin3 = PINA & (1 << 1);

    if ((rechterPin1) || (rechterPin2))
    {
        sensoren = Rechts;
    }
    if ((linkerPin2) || (linkerPin3))
    {
        sensoren = Links;
    }
    if ((centerPin1) || (centerPin2))
    {
        sensoren = Voor;
    }

    switch (sensoren)
    {
    case (3):
        // vooruit
        ICR1 = 25000;
        ICR5 = 25000;
        break;
    case (1):
        // Rechtssaf
        ICR1 = 71530;
        ICR5 = 25000;
        break;
    case (2):
        // linksaf
        ICR1 = 25000;
        ICR5 = 71530;
        break;

    default:
        ICR1 = 65530;
        ICR5 = 65530; // default langzaam rijden
    }
    _delay_ms(50);
}

int getPulse(void)
{
    int knop_ingedrukt = 0;
    int tijdecho = 0;
    int stuk = 0;

    PORTB |= (1 << trig);
    _delay_us(10);
    PORTB &= ~(1 << trig);

    while ((tijdecho || stuk) < timeout)
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
            tijdecho++;
        }
        if ((PINB & (1 << echo)) == 0)
        {
            if (knop_ingedrukt != 0) // knop is zojuist losgelaten
            {
                knop_ingedrukt = 0;
                return tijdecho;
            }
        }
        stuk++;
    }

    return timeout;
}

int ultrasoon(void)
{
    int tijd = timeout;
    tijd = getPulse();
    int Cm = tijd / 58;

    if (Cm < 10)
    {
        PORTB |= (1 << 3); // zet driver rechts uit
        PORTL |= (1 << 1); // zet driver links uit

        DDRB |= (1 << 7);
        PORTB &= ~(1 << 7);
    }
    if (Cm > 13)
    {
        PORTB &= ~(1 << 3); // zet driver rechts aan
        PORTL &= ~(1 << 1); // zet driver links aan;
    }
}