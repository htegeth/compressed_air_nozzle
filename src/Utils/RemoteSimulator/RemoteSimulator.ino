/*
Das Programm wird auf einem Arduino ausgeführt und kann über Serial die Ventilsteuerung anfunken.
Wenn man 1 eingibt dann wird das Ventil geöffnet. Mit der Eingabe von 99XXXX wird die Dauer eingestellt in dem das Ventil geöffnet wird
*/

#include <RCSwitch.h>
#define CONTROL_LED 4
#define REMOTE_PIN 3

RCSwitch mySwitch = RCSwitch();

void setup() {
  pinMode(CONTROL_LED, OUTPUT);
  mySwitch.enableTransmit(REMOTE_PIN); 
  mySwitch.setProtocol(1);
  mySwitch.setRepeatTransmit(5);
  
  Serial.begin(9600);
  Serial.println("Gib einen Code ein. Eingabe '1' sendet 454531:");
}

void loop() {
  // Prüfen, ob etwas über Serial eingegeben wurde
  if (Serial.available() > 0) {
    unsigned long input = Serial.parseInt();

    unsigned long sendCode = 0;

    if (input == 1) {
      sendCode = 454531;
      Serial.println("Code 454531 wird gesendet.");
    } else if (input > 1) {
      sendCode = input;
      Serial.print("Code ");
      Serial.print(sendCode);
      Serial.println(" wird gesendet.");
    }

    // Wenn ein gültiger Code gesetzt wurde, senden
    if (sendCode > 0) {
      digitalWrite(CONTROL_LED, HIGH);   
      mySwitch.send(sendCode, 24);   
      delay(100);                    
      digitalWrite(CONTROL_LED, LOW);    
      Serial.println("Signal gesendet.");
    }

    // Seriellen Eingang leeren
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
}
