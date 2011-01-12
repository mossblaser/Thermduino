#include <WProgram.h>
#include <EEPROM.h>

#include <Persistant.h>
#include <Clock.h>
#include <CalibratedServo.h>

PersistantSettings::PersistantSettings(void) {};
PersistantSettings::~PersistantSettings(void) {};


void
PersistantSettings::calibrateServo(CalibratedServo &servo)
{
	servo.setMinAngle(      getCalMinAngle());
	servo.setMaxAngle(      getCalMaxAngle());
	servo.setMinTemperature(getCalMinTemperature());
	servo.setMaxTemperature(getCalMaxTemperature());
}


void
PersistantSettings::saveServoCalibration(CalibratedServo &servo)
{
	setCalMinAngle(      servo.getMinAngle());
	setCalMaxAngle(      servo.getMaxAngle());
	setCalMinTemperature(servo.getMinTemperature());
	setCalMaxTemperature(servo.getMaxTemperature());
}


void
PersistantSettings::loadClock(Clock &clock)
{
	clock.setDayNumber(getDayNumber());
	clock.setDayOfWeek(getDayOfWeek());
}


void
PersistantSettings::saveClock(Clock &clock)
{
	setDayNumber(clock.getDayNumber());
	setDayOfWeek(clock.getDayOfWeek());
}
