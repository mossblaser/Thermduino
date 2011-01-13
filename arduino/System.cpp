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
	, numProgrammes(0)
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
	
	settings.loadProgrammes(numProgrammes, programmes);
}


void
System::saveSettings(void)
{
	settings.setDefaultTemperature(defaultTemperature);
	settings.saveServoCalibration(thermostat);
	settings.saveClock(clock);
	settings.saveProgrammes(numProgrammes, programmes);
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
