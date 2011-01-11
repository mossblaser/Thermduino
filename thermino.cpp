#include <WProgram.h>
#include <System.h>

#define LED_PIN   13
#define SERVO_PIN 9


System system = System();


void setup() 
{
	Serial.begin(9600);
	system.setLEDPin(LED_PIN);
	system.setThermostatPin(SERVO_PIN);
}


void
loop()
{
	system.handleCommands();
	system.setTemperature();
	system.setLED();
}
