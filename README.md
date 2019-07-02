# Atmega8Clock
energie-effeziente Uhr mit dem Atmega8

## Die 3 Probleme
* Anzeige der Zeit
* Deaktivieren der Anzeige/ Aufwecken mit Tastendruck
* Schalten der Zeit mit externen Quarz

## Anzeige der Zeit
Zum Anzeigen der Zeit wird eine LED-Matrix genutzt, wo bei die Spalten direkt mit dem Atmega8 kontrolliert werden und Zeilen mit zwei 8-Bit Shiftregister 4094D.
