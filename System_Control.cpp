#include <WProgram.h>

#include <Persistant.h>
#include <CalibratedServo.h>
#include <System.h>


void
System::write(char value)
{
	Serial.write(value);
}


void
System::uLongWrite(unsigned long value)
{
	int i;
	for (i = 3; i >= 0; i--) {
		write((char) ((value >> (i*8)) & 0xFF));
	}
}


char
System::blockingRead(void)
{
	while (!Serial.available());
	return Serial.read();
}


unsigned long
System::blockingULongRead(void)
{
	unsigned long value = 0;
	int i;
	for (i = 0; i < 4; i++) {
		value <<= 8;
		value |= blockingRead() & 0xFF;
	}
	return value;
}


void
System::handleCommands(void)
{
	if (Serial.available()) {
		char command = Serial.read();
		
		switch (command) {
			case ('c'): // Send calibration data
				write((char)thermostat.getMinAngle());
				write((char)thermostat.getMinTemperature());
				write((char)thermostat.getMaxAngle());
				write((char)thermostat.getMaxTemperature());
				break;
			
			case ('C'): // Recieve calibration data
				thermostat.setMinAngle(      (uint8_t)blockingRead());
				thermostat.setMinTemperature((uint8_t)blockingRead());
				thermostat.setMaxAngle(      (uint8_t)blockingRead());
				thermostat.setMaxTemperature((uint8_t)blockingRead());
				if (blockingRead()) settings.saveServoCalibration(thermostat);
				write((char)SUCCESS);
				break;
			
			case ('s'): // Send status
				write((char)getStatus());
				break;
			
			case ('t'): // Send temperature
				write((char)getTemperature());
				break;
			
			case ('d'): // Send default temperature
				write((char)getDefaultTemperature());
				break;
			
			case ('D'): // Set default temperature
				setDefaultTemperature(blockingRead());
				write((char)SUCCESS);
				break;
			
			case ('n'): // Send time data
				write((char)clock.getDayNumber());
				write((char)clock.getDayOfWeek());
				uLongWrite(clock.getSecsToday());
				uLongWrite(clock.getStartTime());
				break;
			
			case ('N'): // Set time data
				clock.setDayNumber(blockingRead());
				clock.setDayOfWeek(blockingRead());
				clock.setSecsToday(blockingULongRead());
				clock.setStartTime(blockingULongRead());
				write((char)SUCCESS);
				setStatus(STATUS_NO_PROGRAMS); // Clear the status!
				break;
			
			case ('e'): // Send num EEPROM writes
				uLongWrite(settings.getNumWrites());
				break;
			
			case ('p'): // Send programme data
				write(numProgrammes);
				for (int i = 0; i < numProgrammes; i++) {
					uint8_t f0, f1, f2, f3, f4;
					programmes[i].getData(f0, f1, f2, f3, f4);
					write(f0); write(f1); write(f2); write(f3); write(f4);
				}
				break;
			
			case ('P'): // Recieve programme data
				int reqNumProgrammes = blockingRead();
				for (int i = 0; i < reqNumProgrammes; i++) {
					uint8_t f0, f1, f2, f3, f4;
					f0 = blockingRead();
					f1 = blockingRead();
					f2 = blockingRead();
					f3 = blockingRead();
					f4 = blockingRead();
					if (reqNumProgrammes < MAX_PROGRAMMES)
						programmes[i].setFromData(f0, f1, f2, f3, f4);
				}
				if (reqNumProgrammes < MAX_PROGRAMMES) {
					numProgrammes = reqNumProgrammes;
					settings.saveProgrammes(numProgrammes, programmes);
					write((char)SUCCESS);
				} else {
					write((char)FALIURE);
				}
				break;
		}
	}
}
