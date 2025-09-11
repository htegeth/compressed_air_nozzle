// ATTiny 85 PINS
//
//                  +-\/-+
// Ain0 (D 5) PB5  1|    |8  Vcc
// Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1
// Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1
//            GND  4|    |5  PB0 (D 0) pwm0
//

#include <Arduino.h>
#include <atRcRwitch.h>
#include <EEPROM.h>

#define INTERRUPT_PIN PCINT1 // Interupt ist PB1 gemäß dem Schaltplan
#define VENTIL_PIN PB3       //Anschluss Relais um das Ventil zu steuern
#define CONTROL_LED PB0      // LED um anzuzeigen ob das ventil geöffnet werden sollte
#define PCINT_VECTOR PCINT0_vect // This step is not necessary - it's a naming thing for clarit

#define DATA_PIN 4

// Anzhal der zulässigen maxmialmodes
#define MAX_MODES 4

boolean lightsOn = false;

unsigned long currentCodeMain = 0;

// Fernbedienungscodes. Ausgelesen über RCSwitch.getReceivedValue()
//    _________ 
//  |     =     |
//  | (1)   (2) |
//  |           | 
//  | (3)   (4) |
//  |           |
//  |           |
//    _________   

//Diese Werte werden durch die Build Paramter von platfomio.ini gesetzt.
const unsigned int druckluftDauerConf = DRUCKLUFTDAUER;
const unsigned long ventilCode = VENTIL_CODE;


RCSwitch mySwitch = RCSwitch();

static unsigned int druckluftDauer;


void setup()
{  
  pinMode(PB0, OUTPUT);
  pinMode(VENTIL_PIN, OUTPUT);

  cli();
  PCMSK |= (1 << INTERRUPT_PIN);
  GIMSK |= (1 << PCIE);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  mySwitch.enableReceive(0);
  sei();

  EEPROM.get(0, druckluftDauer);
  if (druckluftDauer == 0xFFFF || druckluftDauer < 10 || druckluftDauer > 2000) {  // EEPROM leer (alle Bits 1) oder auf einem unsinnigen wert
    druckluftDauer = druckluftDauerConf;
    EEPROM.put(0, druckluftDauerConf);  // schreibt 2 Bytes ab Adresse 0
  }
}

ISR(PCINT_VECTOR)
{
  mySwitch.handleInterrupt();
}

void ventilOffen()
{
  digitalWrite(VENTIL_PIN, HIGH); 
  delay(druckluftDauer);                   
  digitalWrite(VENTIL_PIN, LOW);     
}

void feedbackLed()
{
  for(int i=0;  i<5; i++){
    digitalWrite(CONTROL_LED, HIGH); 
    delay(50);                   
    digitalWrite(CONTROL_LED, LOW); 
    delay(50);                   
  }
}

void loop()
{
  
  if (mySwitch.available())
  {    
    currentCodeMain = mySwitch.getReceivedValue();           
    mySwitch.resetAvailable();
  }
  int codePrefix = currentCodeMain / 10000;
  int codeSuffix = currentCodeMain % 10000;


  if (currentCodeMain == ventilCode) //ventil öffnen
  { 
     ventilOffen();
  }else if (codePrefix == 99 ) //zeit zum Ventilöffnen neu festlegen
  {
    feedbackLed();    
    if (codeSuffix>50 && codeSuffix < 4000)
    {
      druckluftDauer=codeSuffix; 
      EEPROM.put(0, druckluftDauer);
    }
  }
   currentCodeMain=0;   

}
