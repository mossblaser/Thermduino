#include <WProgram.h>
#include <EEPROM.h>

#include <Persistant.h>
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
