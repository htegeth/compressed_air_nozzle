/*
 (!) Benutze den Sketch in der Arduino IDE oder schreibe eine eigene platformio.ini dafür.

 Wird verwendet um die Fernbedienung zu programmieren.
 Wichtig ist es die Fernbedienung ganz nah an den Sender zu halten. 
 Die Fernbedienung darf den Sender sogar berühren, sonst funktioniert die Übertragung nicht.
 Nicht jede Fernbedienung kann jedes Protokoll oder jede Bitlänge der Nachricht.

 Die Fernbedienung sollte vorher in den Programmiermodus gebracht werden. Bei den meisten Modellen mit vier Knöpfen geht das so:
 1. Knopf 1 (oder A) und 2 (B) gleichzeitig drücken bis die LED blinkt und ausgeht (das ist der Reset!) -> Knopf 1 nicht loslassen
 2. Knopf 1 weiter gedrückt halten  und Knopf 2  drei mal hintereinander drücken. Die LED sollte dann flackkern
 3. Unter mySwitch.send(n,24) den neuen Code eintragen und das Sketch auf den Arduino übertragen
 4. Knopf 1-4 (A-D) drücken und halten bis die LED nicht mehr blinkt sondern flackert.
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  mySwitch.enableTransmit(3); //PIN 3
}

void loop() {
  mySwitch.setProtocol(1);
  mySwitch.send(770200, 24);
  delay(500);

}