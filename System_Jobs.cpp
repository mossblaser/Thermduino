#include <WProgram.h>

#include <Persistant.h>
#include <CalibratedServo.h>
#include <System.h>
#include <Programme.h>


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
		case(STATUS_PROGRAM_RUNNING):  pattern = 0xFE; break; // Slow flashing
		case(STATUS_TIME_OUT_OF_SYNC): pattern = 0x55; break; // Fast flashing
		default:                       pattern = 0x00; break;
	}
	
	uint8_t step = (millis()/100) % 8;
	bool state = (pattern >> step) & 0x01;
	// This appears to prevent the servo from twitching by reducing the number of
	// writes here...
	if (state != ledState) {
		digitalWrite(ledPin, state);
		ledState = state;
	}
}


void
System::setTemperature(void)
{
	int temperature = defaultTemperature;
	
	bool isProgramRunning = false;
	bool isProgramNotExpired = false;
	
	int i;
	for (i = 0; i < numProgrammes; i++) {
		if (programmes[i].isActive(clock)) {
			temperature = programmes[i].getTemperature();
			if (!programmes[i].isWeekday())
				isProgramRunning = true;
		} else {
			if (!programmes[i].isWeekday() && !programmes[i].isExpired(clock))
				isProgramNotExpired = true;
		}
	}
	
	// If the time is out of sync, don't change the status
	if (getStatus() != STATUS_TIME_OUT_OF_SYNC) {
		if (isProgramRunning)
			setStatus(STATUS_PROGRAM_RUNNING);
		else if (isProgramNotExpired)
			setStatus(STATUS_PROGRAMED);
		else
			setStatus(STATUS_NO_PROGRAMS);
	}
	
	thermostat.setTemperature(temperature);
}
