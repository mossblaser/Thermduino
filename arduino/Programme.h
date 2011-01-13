#ifndef PROGRAMME_H
#define PROGRAMME_H

#include <WProgram.h>
#include <Clock.h>


class Programme {
	public:
		Programme(void);
		
		bool isActive(Clock &clock);
		bool isExpired(Clock &clock);
		
		void getData(uint8_t &f0, uint8_t &f1, uint8_t &f2, uint8_t &f3, uint8_t &f4);
		void setFromData(uint8_t f0, uint8_t f1, uint8_t f2, uint8_t f3, uint8_t f4);
		
		bool    isWeekday(void);
		uint8_t getStartDayNumber(void);
		uint8_t getWeekdays(void);
		uint8_t getEndDayNumber(void);
		uint8_t getStartTime(void);
		uint8_t getEndTime(void);
		uint8_t getTemperature(void);
		
		void setWeekday(bool weekday);
		void setStartDayNumber(uint8_t startDayNumber);
		void setWeekdays(uint8_t weekdays);
		void setEndDayNumber(uint8_t endDayNumber);
		void setStartTime(uint8_t startTime);
		void setEndTime(uint8_t endTime);
		void setTemperature(uint8_t temperature);
	
	private:
		// Is this a weekday program?
		bool weekday;
		
		union {
			// Day number to start program (non-weekday programme)
			uint8_t startDayNumber;
			
			// Bitfield with days-of-week to be active (weekday programme)
			uint8_t weekdays;
		};
		
		// Day number to end the programme (non-weekday programme)
		uint8_t endDayNumber;
		
		// Times it is active (weekday programme) or time to begin on start day and
		// end on end day.
		uint8_t startTime;
		uint8_t endTime;
		
		// Temperature to switch to
		uint8_t temperature;
};


#endif
