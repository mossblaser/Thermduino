#include <WProgram.h>
#include <EEPROM.h>

#include <Clock.h>
#include <CalibratedServo.h>
#include <Programme.h>
#include <Persistant.h>


PersistantSettings::PersistantSettings(void)
	: numWrites(0)
{
	// Do nothing
}


PersistantSettings::~PersistantSettings(void) {}


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
	clock.setStartTime(getStartTime());
}


void
PersistantSettings::saveClock(Clock &clock)
{
	setDayNumber(clock.getDayNumber());
	setDayOfWeek(clock.getDayOfWeek());
	setStartTime(clock.getStartTime());
}


void
PersistantSettings::writeULong(int address, unsigned long value)
{
	int i;
	for (i = 3; i >= 0; i--) {
		numWrites++;
		EEPROM.write(address + i, (char)value);
		value >>= 8;
	}
}


unsigned long
PersistantSettings::readULong(int address)
{
	unsigned long value = 0;
	int i;
	for (i = 0; i < 4; i++) {
		value <<= 8;
		value |= EEPROM.read(address + i);
	}
	
	return value;
}


int
PersistantSettings::getNumWrites(void)
{
	return numWrites;
}


void
PersistantSettings::loadProgrammes(uint8_t &numProgrammes, Programme *programmes)
{
	numProgrammes = getNumProgrammes();
	
	// Size of a data entry in the eeprom
	const int dataSize = 5;
	
	int i;
	for (i = 0; i < numProgrammes; i++) {
		int address = PROGRAMME_DATA + (i*dataSize);
		
		uint8_t f0, f1, f2, f3, f4;
		f0 = EEPROM.read(address + 0);
		f1 = EEPROM.read(address + 1);
		f2 = EEPROM.read(address + 2);
		f3 = EEPROM.read(address + 3);
		f4 = EEPROM.read(address + 4);
		programmes[i].setFromData(f0, f1, f2, f3, f4);
	}
}


void
PersistantSettings::saveProgrammes(uint8_t &numProgrammes, Programme *programmes)
{
	setNumProgrammes(numProgrammes);
	
	// Size of a data entry in the eeprom
	const int dataSize = 5;
	
	int i;
	for (i = 0; i < numProgrammes; i++) {
		int address = PROGRAMME_DATA + (i*dataSize);
		
		uint8_t f0, f1, f2, f3, f4;
		programmes[i].getData(f0, f1, f2, f3, f4);
		numWrites += 5;
		EEPROM.write(address + 0, f0);
		EEPROM.write(address + 1, f1);
		EEPROM.write(address + 2, f2);
		EEPROM.write(address + 3, f3);
		EEPROM.write(address + 4, f4);
	}
}
