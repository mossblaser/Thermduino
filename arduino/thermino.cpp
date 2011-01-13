#include <WProgram.h>
#include <System.h>
#include <EEPROM.h>

#define LED_PIN   13
#define SERVO_PIN 9


System system = System();


void setup() 
{
	Serial.begin(9600);
	system.setLEDPin(LED_PIN);
	system.setThermostatPin(SERVO_PIN);
	system.tick();
}


void
loop()
{
	system.handleCommands();
	system.tick();
	system.setTemperature();
	system.setLED();
}
