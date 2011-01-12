#include <WProgram.h>

#include <Persistant.h>
#include <CalibratedServo.h>
#include <System.h>


System::System(void)
	: ledPin(13)
	, defaultTemperature(0)
	, status(STATUS_TIME_OUT_OF_SYNC)
	, thermostat()
	, lastDayNumber(0)
	, lastDayOfWeek(0)
	, clock()
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
	
	settings.loadClock(clock);
	lastDayNumber = clock.getDayNumber();
	lastDayOfWeek = clock.getDayOfWeek();
}


void
System::saveSettings(void)
{
	settings.setDefaultTemperature(defaultTemperature);
	settings.saveServoCalibration(thermostat);
	settings.saveClock(clock);
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
System::tick(void)
{
	clock.tick();
	
	if (lastDayNumber != clock.getDayNumber()) {
		settings.setDayNumber(clock.getDayNumber());
		lastDayNumber = clock.getDayNumber();
	}
	
	if (lastDayOfWeek != clock.getDayOfWeek()) {
		settings.setDayOfWeek(clock.getDayOfWeek());
		lastDayOfWeek = clock.getDayOfWeek();
	}
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
		char command = Serial.read();
		
		switch (command) {
			case ('c'): // Send calibration data
				Serial.write((char)thermostat.getMinAngle());
				Serial.write((char)thermostat.getMinTemperature());
				Serial.write((char)thermostat.getMaxAngle());
				Serial.write((char)thermostat.getMaxTemperature());
				break;
			
			case ('C'): // Recieve calibration data
				while(!Serial.available());
				thermostat.setMinAngle(      (uint8_t)Serial.read());
				while(!Serial.available());
				thermostat.setMinTemperature((uint8_t)Serial.read());
				while(!Serial.available());
				thermostat.setMaxAngle(      (uint8_t)Serial.read());
				while(!Serial.available());
				thermostat.setMaxTemperature((uint8_t)Serial.read());
				while(!Serial.available());
				if (Serial.read()) settings.saveServoCalibration(thermostat);
				Serial.write((char)SUCCESS);
				break;
			
			case ('s'): // Send status
				Serial.write((char)getStatus());
				break;
			
			case ('t'): // Send temperature
				Serial.write((char)getTemperature());
				break;
			
			case ('d'): // Send default temperature
				Serial.write((char)getDefaultTemperature());
				break;
			
			case ('D'): // Set default temperature
				while(!Serial.available());
				setDefaultTemperature(Serial.read());
				Serial.write((char)SUCCESS);
				break;
			
			case ('n'): // Send the day number & day of week
				Serial.write((char)clock.getDayNumber());
				Serial.write((char)clock.getDayOfWeek());
				break;
			
			case ('N'): // Set the day number & day of week
				while(!Serial.available());
				clock.setDayNumber(Serial.read());
				while(!Serial.available());
				clock.setDayOfWeek(Serial.read());
				settings.saveClock(clock);
				Serial.write((char)SUCCESS);
				break;
		}
	}
}
