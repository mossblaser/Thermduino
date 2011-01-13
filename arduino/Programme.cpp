#include <WProgram.h>

#include <Clock.h>
#include <Programme.h>

#define TIME_CONVERSION_RATIO (900)


Programme::Programme(void)
	: weekday(false)
	, startDayNumber(0)
	, endDayNumber(0)
	, startTime(0)
	, endTime(0)
	, temperature(15)
{
	// Do nothing...
}


bool    Programme::isWeekday(void) {         return weekday; }
uint8_t Programme::getStartDayNumber(void) { return startDayNumber; }
uint8_t Programme::getWeekdays(void) {       return weekdays; }
uint8_t Programme::getEndDayNumber(void) {   return endDayNumber; }
uint8_t Programme::getStartTime(void) {      return startTime; }
uint8_t Programme::getEndTime(void) {        return endTime; }
uint8_t Programme::getTemperature(void) {    return temperature; }

void Programme::setWeekday(bool value) {           weekday = value; }
void Programme::setStartDayNumber(uint8_t value) { startDayNumber = value; }
void Programme::setWeekdays(uint8_t value) {       weekdays = value; }
void Programme::setEndDayNumber(uint8_t value) {   endDayNumber = value; }
void Programme::setStartTime(uint8_t value) {      startTime = value; }
void Programme::setEndTime(uint8_t value) {        endTime = value; }
void Programme::setTemperature(uint8_t value) {    temperature = value; }


bool
Programme::isActive(Clock &clock)
{
	// Calculate the time limits in seconds
	unsigned long startTimeSec = ((unsigned long)startTime)
	                             * TIME_CONVERSION_RATIO;
	unsigned long endTimeSec = ((unsigned long)endTime)
	                            * TIME_CONVERSION_RATIO;
	
	if (isWeekday()) {
		// Weekday programme
		return ((getWeekdays() & (0x01 << clock.getDayOfWeek())) != 0)
		       && clock.getSecsToday() >= startTimeSec
		       && clock.getSecsToday() < endTimeSec;
	} else {
		// Non-Weekday programme
		uint8_t dayNumber = clock.getDayNumber();
		unsigned long secsToday = clock.getSecsToday();
		
		return (dayNumber == startDayNumber    && startDayNumber == endDayNumber
		        && secsToday >= startTimeSec && secsToday < endTimeSec)
		       || (dayNumber == startDayNumber && startDayNumber != endDayNumber
		           && secsToday >= startTimeSec)
		       || (dayNumber == endDayNumber   && startDayNumber != endDayNumber
		           && secsToday < startTimeSec)
		       || (dayNumber > startDayNumber  && startDayNumber != endDayNumber
		           && dayNumber < endDayNumber);
	}
}


bool
Programme::isExpired(Clock &clock)
{
	if (isWeekday()) {
		return false;
	} else {
		uint8_t dayNumber = clock.getDayNumber();
		return (dayNumber > endDayNumber)
		       || (dayNumber == endDayNumber
		           && !isActive(clock));
	}
}

// Data encoding
//
//              +-------------+-----------+-----------+---------+-------------+
// Non-weekday: | StartDayNum | StartTime | EndDayNum | EndTime | Temperature |
//              +-------------+-----------+-----------+---------+-------------+
//
//              +------+-----------+--------------+---------+-------------+
// Weekday:     | 0xFF | StartTime | DaysInEffect | EndTime | Temperature |
//              +------+-----------+--------------+---------+-------------+


void
Programme::setFromData(uint8_t f0, uint8_t f1, uint8_t f2, uint8_t f3, uint8_t f4)
{
	
	if (f0 == 0xFF) {
		// Weekday programme
		setWeekday(true);
		setWeekdays(f2);
	} else {
		// Non-weekday programme
		setWeekday(false);
		setStartDayNumber(f0);
		setEndDayNumber(f2);
	}
	
	// Load start and end times
	setStartTime(f1);
	setEndTime(f3);
	
	// Load temperature
	setTemperature(f4);
}


void
Programme::getData(uint8_t &f0, uint8_t &f1, uint8_t &f2, uint8_t &f3, uint8_t &f4)
{
	
	if (isWeekday()) {
		f0 = 0xFF;
		f2 = getWeekdays();
	} else {
		f0 = getStartDayNumber();
		f2 = getEndDayNumber();
	}
	
	// Start and end times
	f1 = getStartTime();
	f3 = getEndTime();
	
	// Temperature
	f4 = getTemperature();
}
