#include <WProgram.h>

#include <Persistant.h>
#include <CalibratedServo.h>
#include <System.h>


System::System(void)
	: ledPin(13)
	, defaultTemperature(0)
	, status(STATUS_TIME_OUT_OF_SYNC)
	, thermostat()
{
	loadSettings();
}


void
System::setThermostatPin(int pin)
{
	thermostat.attach(pin);
}


void
System::setLEDPin(int pin)
{
	ledPin = pin;
	pinMode(ledPin, OUTPUT);
}


void
System::loadSettings(void)
{
	defaultTemperature = settings.getDefaultTemperature();
	settings.calibrateServo(thermostat);
}


void
System::saveSettings(void)
{
	settings.setDefaultTemperature(defaultTemperature);
	settings.saveServoCalibration(thermostat);
}


int
System::getDefaultTemperature(void)
{
	return defaultTemperature;
}


void
System::setDefaultTemperature(int temperature)
{
	defaultTemperature = temperature;
	setTemperature();
}


void
System::setTemperature(void)
{
	int temperature = defaultTemperature;
	
	// TODO: Check against all rules
	
	thermostat.setTemperature(temperature);
}


int
System::getTemperature(void)
{
	return thermostat.getTemperature();
}


void
System::setStatus(enum status state)
{
	status = state;
}


System::status_t
System::getStatus(void)
{
	return status;
}


void
System::setLED(void)
{
	uint8_t pattern = 0;
	
	switch (getStatus()) {
		case(STATUS_NO_PROGRAMS):      pattern = 0x00; break; // Off
		case(STATUS_PROGRAMED):        pattern = 0xFF; break; // Solid
		case(STATUS_PROGRAM_RUNNING):  pattern = 0x0F; break; // Slow flashing
		case(STATUS_TIME_OUT_OF_SYNC): pattern = 0x55; break; // Fast flashing
		default:                       pattern = 0x00; break;
	}
	
	int step = (millis()/100) % 8;
	digitalWrite(ledPin, (pattern >> step) & 0x01);
}


void
System::handleCommands(void)
{
	if (Serial.available()) {
		// Do something
	}
}
