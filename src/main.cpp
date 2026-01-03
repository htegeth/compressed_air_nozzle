#include <Arduino.h>
#include <atRcRwitch.h>
#include <EEPROM.h>

// ATTiny 85 PINS
#define INTERRUPT_PIN   PCINT1     // Interrupt = PB1
#define VENTIL_PIN      PB3        // Relaisausgang
#define CONTROL_LED     PB0        // Status LED
#define PCINT_VECTOR    PCINT0_vect

// Befehlspräfixe
#define PREFIX_FLEX     77
#define PREFIX_CONFIG   99

// Konfiguration (über platformio.ini gesetzt)
const unsigned int druckluftDauerConf = DRUCKLUFTDAUER;
const unsigned long ventilCode = VENTIL_CODE;
const unsigned int druckluftDauerMin = DRUCKLUFTDAUER_MIN;
const unsigned int druckluftDauerMax = DRUCKLUFTDAUER_MAX;

RCSwitch mySwitch = RCSwitch();

static unsigned int druckluftDauer;
unsigned long currentCodeMain = 0;

void setup() {
  pinMode(CONTROL_LED, OUTPUT);
  pinMode(VENTIL_PIN, OUTPUT);

  cli();
  PCMSK |= (1 << INTERRUPT_PIN);
  GIMSK |= (1 << PCIE);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  mySwitch.enableReceive(0);
  sei();

  EEPROM.get(0, druckluftDauer);
  if (druckluftDauer == 0xFFFF || druckluftDauer < druckluftDauerMin || druckluftDauer > druckluftDauerMax) {
    druckluftDauer = druckluftDauerConf;
    EEPROM.put(0, druckluftDauerConf);
  }
}

ISR(PCINT_VECTOR) {
  // Muss so, da die Library Interrupt-Bearbeitung intern erfordert
  mySwitch.handleInterrupt();
}

void ventilOffen(unsigned int dauer) {
  if (dauer >= druckluftDauerMin && dauer <= druckluftDauerMax) {
    // HIGH aktiviert das Relais nur dann, wenn es sich um ein High-Level-Trigger-Relaismodul handelt;
    digitalWrite(CONTROL_LED, HIGH); 
    digitalWrite(VENTIL_PIN, HIGH); 
    delay(dauer);
    digitalWrite(VENTIL_PIN, LOW);
    digitalWrite(CONTROL_LED, LOW);
  }
}

void feedbackLed() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(CONTROL_LED, HIGH);
    delay(50);
    digitalWrite(CONTROL_LED, LOW);
    delay(50);
  }
}

void loop() {
  if (mySwitch.available()) {
    currentCodeMain = mySwitch.getReceivedValue();
    mySwitch.resetAvailable();
  }

  unsigned long codePrefix = currentCodeMain / 10000;
  unsigned long codeSuffix = currentCodeMain % 10000;

  if (currentCodeMain == ventilCode) {
    ventilOffen(druckluftDauer);
  } 
  else if (codePrefix == PREFIX_FLEX) {
    ventilOffen(codeSuffix);
  } 
  else if (codePrefix == PREFIX_CONFIG) {
    feedbackLed();
    if (codeSuffix >= druckluftDauerMin && codeSuffix <= druckluftDauerMax) {
      if (codeSuffix != druckluftDauer) {
        druckluftDauer = codeSuffix;
        EEPROM.put(0, druckluftDauer);
      }
    }
  }

  currentCodeMain = 0; // Reset nach jedem Befehl
}
