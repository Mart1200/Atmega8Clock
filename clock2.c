#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

volatile uint8_t sekunde = 50;
volatile uint8_t minute = 59;
volatile uint8_t stunde = 11;

volatile uint8_t ka;

volatile uint8_t counter = 0;




void setSek(int n){
	DDRB |= (1 << PB4); //D für IC2
	DDRB |= (1 << PB3);	// Clock
	DDRB |= (1 << PB2);	// Strobo
	DDRB |= (1 << PB1); //Spalte 1
	DDRB |= (1 << PB0); //Spalte 2

	
	PORTB |= (1 << PB2);

	uint8_t n1, n2;

	n1 = n%10;
	n2 = n/10;

	int i;

	for(i = 0; i < n1; i++){
		PORTB &= ~(1 << PB3);
		PORTB &= ~(1 << PB4);
		PORTB |= (1 << PB3);
	}

	for(i = 0; i < 12 - n1; i++){
		PORTB &= ~(1 << PB3);
		PORTB |= (1 << PB4);
		PORTB |= (1 << PB3);
	}

	PORTB |= (1 << PB1);
	_delay_ms(1);
	PORTB &= ~(1 << PB1);
	

	for(i = 0; i < n2; i++){
		PORTB &= ~(1 << PB3);
		PORTB &= ~(1 << PB4);
		PORTB |= (1 << PB3);
	}

	for(i = 0; i < 12 - n2; i++){
		PORTB &= ~(1 << PB3);
		PORTB |= (1 << PB4);
		PORTB |= (1 << PB3);
	}
	

	PORTB |= (1 << PB0);
	_delay_ms(1);
	PORTB &= ~(1 << PB0);

}

void setMin(int n){
	DDRB |= (1 << PB4); //D für IC2
	DDRB |= (1 << PB3);	// Clock
	DDRB |= (1 << PB2);	// Strobo
	DDRD |= (1 << PD7); //Spalte 3
	DDRD |= (1 << PD6); //Spalte 4

	
	PORTB |= (1 << PB2);

	uint8_t n1, n2;

	n1 = n%10;
	n2 = n/10;

	int i;

	for(i = 0; i < n1; i++){
		PORTB &= ~(1 << PB3);
		PORTB &= ~(1 << PB4);
		PORTB |= (1 << PB3);
	}

	for(i = 0; i < 12 - n1; i++){
		PORTB &= ~(1 << PB3);
		PORTB |= (1 << PB4);
		PORTB |= (1 << PB3);
	}

	PORTD |= (1 << PD7);
	_delay_ms(1);
	PORTD &= ~(1 << PD7);
	

	for(i = 0; i < n2; i++){
		PORTB &= ~(1 << PB3);
		PORTB &= ~(1 << PB4);
		PORTB |= (1 << PB3);
	}

	for(i = 0; i < 12 - n2; i++){
		PORTB &= ~(1 << PB3);
		PORTB |= (1 << PB4);
		PORTB |= (1 << PB3);
	}
	

	PORTD |= (1 << PD6);
	_delay_ms(1);
	PORTD &= ~(1 << PD6);

}

void setHour(int n){
	DDRB |= (1 << PB4); //D für IC2
	DDRB |= (1 << PB3);	// Clock
	DDRB |= (1 << PB2);	// Strobo
	DDRD |= (1 << PD5); //Spalte 5
	
	PORTB |= (1 << PB2);

	int i;

	for(i = 0; i < n; i++){
		PORTB &= ~(1 << PB3);
		PORTB &= ~(1 << PB4);
		PORTB |= (1 << PB3);
	}

	for(i = 0; i < 12 - n; i++){
		PORTB &= ~(1 << PB3);
		PORTB |= (1 << PB4);
		PORTB |= (1 << PB3);
	}

	PORTD |= (1 << PD5);
	_delay_ms(1);
	PORTD &= ~(1 << PD5);
}

ISR (INT0_vect){
	counter = 0;
}

ISR (TIMER2_OVF_vect){
	ka++;
	if ( ka == 16){
		ka = 0;
		sekunde++;
		counter++;

		if(sekunde == 60){
			minute++;
			sekunde = 0;
		}

		if(minute == 60){
			stunde++;
			minute = 0;
		}

		if(stunde == 12){
			stunde = 0;
		}
	}
}


int main()
{
	DDRD &= ~(1 << DDD2); // Interrupt-Port auf Input
	PORTD |= (1 << PORTD2);//Freigeben interrupt schalter
	
	MCUCR |= (1<<ISC01) | (1 << ISC00); //setzen des Interrupts auf Flanken;
	GICR |= (1 << INT0); // aktivieren des Interrupts

	ASSR |= (1<<AS2);
	TCCR2 |= (1 << CS21); //prescaler timer 2
	TCNT2 = 0x00;
	TIFR |= (1 << TOV2);
	TIMSK |= (1 << TOIE2); //overflow interrupt erlauben
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	sei();

	
	
	while(1){
		if(counter >= 30){

			sleep_mode();
		}else{
			setSek(sekunde);
			setMin(minute);
			setHour(stunde);
		}
	}
	
	return 0;
}

