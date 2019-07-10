# Atmega8Clock
energie-effeziente Uhr mit dem Atmega8 auf Basis von [FRANZIS Ping-Pong](https://www.franzis.de/maker/bausaetze/das-retro-spiel-zum-selberbauen-bausatz)

## Die 3 Probleme
* Anzeige der Zeit
* Deaktivieren der Anzeige/ Aufwecken mit Tastendruck
* Schalten der Zeit mit externen Quarz

## Anzeige der Zeit
Zum Anzeigen der Zeit wird eine LED-Matrix (12x10) genutzt, wo bei die Spalten direkt mit dem Atmega8 kontrolliert werden und Zeilen mit zwei 8-Bit Shiftregister 4094D, wobei die zu vielen Bits in den zweiten Shiftregister geschoben werden. Da bei wird in diesem Lösungsansatz (von links nach recht) erst Sekunden, dann Minuten, dann Studen jeweils in 2 Spalten dargestellt. Der Code für die Darstellung von Sekunden, Minuten und Stunden sind nahe zu identisch.

Hier erklärt anhand der Funktion für Sekunden:
```
DDRB |= (1 << PB4); // D für IC2
DDRB |= (1 << PB3); // Clock
DDRB |= (1 << PB2); // Strobo
DDRB |= (1 << PB1); //Spalte 1
DDRB |= (1 << PB0); //Spalte 2
  
PORTB |= (1 << PB2);
```
Am Anfang werden die entsprechen Pins am Atmega auf Ausgang gesetzs und der Strobo-Pin am IC auf 1, für die korrekte Konfiguration.
```
uint8_t n1, n2;

n1 = n%10;
n2 = n/10;

int i;
```
Deklaration der verschiedene Variabeln für die Darstellung, Sek = n2 * 10 + n1. Die Variabeln werden auf eine Länge von 8bit gesetzt auf Grund der 8bit Architektur des Atmega8s. int i dient als Zählervariabel in den Kommenden Schleifen.
```
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
```
In diesem Abschnitt werden Register belegt für die erste Spalte belegt, 0 stehen für leuchtende LEDs, 1 für deaktivierte.
```
PORTB |= (1 << PB1);
_delay_ms(1); //evtl noch durch anderen Timmer zu ersetzen
PORTB &= ~(1 << PB1);
```
Aktivieren der ersten Spalte und dadurch aktivieren der LEDs, nach kurzer Zeit deaktievieren der LEDs.
```
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
```
Selbes Vorgehen wie bei Spalte 1.
## Deaktivieren der Anzeige/ Aufwecken mit Tastendruck
```
#include <avr/interrupt.h>
#include <avr/sleep.h>

volatile uint8_t counter = 0;
```
Includen der benötigten Funktionen, und setzen einer globalen Variabel zum Zählen der Sekunden bis zur Deaktivierung.
In der main():
```
DDRD &= ~(1 << DDD2); // Interrupt-Port auf Input
PORTD |= (1 << PORTD2);//Freigeben interrupt schalter

MCUCR |= (1<<ISC01) | (1 << ISC00); //setzen des Interrupts auf Flanken;
GICR |= (1 << INT0); // aktivieren des Interrupts
```
Freischalten des Interrupt-Pins am Atmega8 und Konfiguration des Interruppts. Der Interrupt soll bei steigender Flanke ausgelöst.
```
set_sleep_mode(SLEEP_MODE_PWR_SAVE);
sei();
```
Konfiguration des Sleep-Modes auf einen Modi, der externe Interrupts zu lässt und Ermöglichung von Interrupts durch sei().
```
while(1){
  if(counter >= 30){
    sleep_mode();
  }else{
    setSek(sekunde);
    setMin(minute);
    setHour(stunde);
  }
}
```
Wenn der Counter auf 30 geht, Aktivierung Sleep-Mode, sonst darstellung der Zeit durch oben erläuterte Funktionen.
die Interrupt-Funktion:
```
ISR (INT0_vect){
	counter = 0;
}
```
Falls der Schalter gedrückt wird resetet der counter und fängt von vorne an zu zählen.
## Schalten der Zeit mit externen Quarz
Für die Zeit brauchen wir 3 Verschiedene Variabel, die jeweils die aktuelle Stunde, Minute und Sekunde darstellen. Diese Variabeln müssen global sein und volatile, um Hardwareeingriffe zu ermöglichen. Der Startzeitpunkt wird hier fest gelegt.
```
volatile uint8_t sekunde = 50;
volatile uint8_t minute = 59;
volatile uint8_t stunde = 11;
```
Konfiguration des Quarzes und freischalten des Overflow-Interrupts:
```
ASSR |= (1<<AS2);
TCCR2 |= (1 << CS20) | (1 << CS22); //prescaler timer 2 (128) t(zw interrupt)= (2^BitTimer * prescaler)/Frequenz
TCNT2 = 0x00;
TIFR |= (1 << TOV2);
TIMSK |= (1 << TOIE2); //overflow interrupt erlauben
```
Der Taktgeber für Timer 2 wird auf den externen Quarzoszillator geändert. Der Prescaler skaliert um 128.  Im folgenden wird der Timer auf 0 gesetzt. Timer/Counter Interrupt Flag Register wird gesetzt, so dass Timer 2 overflowen kann. Timer/Counter Interrupt Mask Register wird gesetzt so das es beim Overflow von Timer 2 zum Interrupt kommen kann.

Interrupt-Routine für den Overflow-Interrupt des Timer 2 (Quarz):
```
ISR (TIMER2_OVF_vect){
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
```
In der Zeit wo eine Sekunde vergeht wird 16 mal der Overflow-Interrupt ausgelöst, auf Grund des Prescalers. Also erst nach 16 mal aufrufen des Interrupts soll die Variabel für die Sekunde hochgezählt werden. Falls die Sekunde 60 ereichen würde, wird sie auf 0 gesetzt und die Minute um 1 erhöht. Selbiges geschieht für Minute und Stunde.

