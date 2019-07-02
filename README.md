# Atmega8Clock
energie-effeziente Uhr mit dem Atmega8

## Die 3 Probleme
* Anzeige der Zeit
* Deaktivieren der Anzeige/ Aufwecken mit Tastendruck
* Schalten der Zeit mit externen Quarz

## Anzeige der Zeit
Zum Anzeigen der Zeit wird eine LED-Matrix (12x10) genutzt, wo bei die Spalten direkt mit dem Atmega8 kontrolliert werden und Zeilen mit zwei 8-Bit Shiftregister 4094D, wobei die zu vielen Bits in den zweien Shitregister geschoben werden. Da bei wird in diesem Lösungsansatz (von links nach recht) erst Sekunden, dann Minuten, dann Studen jeweils in 2 Spalten dargestellt. Der Code für die Darstellung von Sekunden, Minuten und Stunden sind nahe zu identisch.

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
