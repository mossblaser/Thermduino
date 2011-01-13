#include <WProgram.h>

#include <Clock.h>

#define SECS_PER_DAY 86400


Clock::Clock(void)
	: secsToday(0)
	, dayNumber(0)
	, dayOfWeek(0)
	, lastMillis(0)
	, startTime(0)
{
	// Do nothing...
}


void Clock::setDayNumber(uint8_t value) { dayNumber = value; }
void Clock::setDayOfWeek(uint8_t value) { dayOfWeek = value; }
void Clock::setStartTime(unsigned long value) { startTime = value; }
void Clock::setSecsToday(unsigned long value) { secsToday = value; }

uint8_t Clock::getDayNumber(void) { return dayNumber; }
uint8_t Clock::getDayOfWeek(void) { return dayOfWeek; }
unsigned long Clock::getStartTime(void) { return startTime; }
unsigned long Clock::getSecsToday(void) { return secsToday; }


void
Clock::tick(void)
{
	unsigned long delta_millis = millis() - lastMillis;
	unsigned long secs_elapsed = delta_millis / 1000;
	lastMillis += secs_elapsed * 1000;
	
	secsToday += secs_elapsed;
	
	if (secsToday >= SECS_PER_DAY) {
		// Roll the seccond counter
		secsToday = secsToday - SECS_PER_DAY;
		
		// Increment the day counters
		setDayOfWeek((getDayOfWeek() + 1) % 7);
		
		// Don't let the day number roll over
		if (dayNumber < 0xFF) {
			setDayNumber(getDayNumber() + 1);
		}
	}
}
