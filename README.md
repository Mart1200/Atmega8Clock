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
