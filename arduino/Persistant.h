#ifndef PERSISTANT_H
#define PERSISTANT_H

#include <WProgram.h>
#include <EEPROM.h>

#include <Clock.h>
#include <CalibratedServo.h>
#include <Programme.h>


#define PERSISTANT_GETTER_SETTER(addr, gname, sname) \
	uint8_t gname(void         ) { return EEPROM.read(addr); }; \
	void    sname(uint8_t value) { numWrites++;EEPROM.write(addr, value); }


#define PERSISTANT_ULONG_GETTER_SETTER(addr, gname, sname) \
	unsigned long gname(void               ) { return readULong(addr); }; \
	void          sname(unsigned long value) { writeULong(addr, value); }


class PersistantSettings {
	public:
		PersistantSettings(void);
		~PersistantSettings(void);
		
		PERSISTANT_GETTER_SETTER(CAL_MIN_ANGLE,
		                         getCalMinAngle,
		                         setCalMinAngle);
		PERSISTANT_GETTER_SETTER(CAL_MAX_ANGLE,
		                         getCalMaxAngle,
		                         setCalMaxAngle);
		PERSISTANT_GETTER_SETTER(CAL_MIN_TEMPERATURE,
		                         getCalMinTemperature,
		                         setCalMinTemperature);
		PERSISTANT_GETTER_SETTER(CAL_MAX_TEMPERATURE,
		                         getCalMaxTemperature,
		                         setCalMaxTemperature);
		
		PERSISTANT_GETTER_SETTER(DEFAULT_TEMPERATURE,
		                         getDefaultTemperature,
		                         setDefaultTemperature);
		
		PERSISTANT_GETTER_SETTER(CLOCK_DAY_NUMBER,
		                         getDayNumber,
		                         setDayNumber);
		PERSISTANT_GETTER_SETTER(CLOCK_DAY_OF_WEEK,
		                         getDayOfWeek,
		                         setDayOfWeek);
		
		PERSISTANT_ULONG_GETTER_SETTER(CLOCK_START_TIME,
		                               getStartTime,
		                               setStartTime);
		
		PERSISTANT_GETTER_SETTER(NUM_PROGRAMMES,
		                         getNumProgrammes,
		                         setNumProgrammes);
		
		void calibrateServo(CalibratedServo &servo);
		void saveServoCalibration(CalibratedServo &servo);
		
		void loadClock(Clock &clock);
		void saveClock(Clock &clock);
		
		void loadProgrammes(uint8_t &numProgrammes, Programme *programmes);
		void saveProgrammes(uint8_t &numProgrammes, Programme *programmes);
		
		void writeULong(int address, unsigned long);
		unsigned long readULong(int address);
		
		
		int getNumWrites(void);
		
	
	private:
		int numWrites;
		
		enum {
			CAL_MIN_ANGLE,
			CAL_MIN_TEMPERATURE,
			CAL_MAX_ANGLE,
			CAL_MAX_TEMPERATURE,
			DEFAULT_TEMPERATURE,
			CLOCK_DAY_NUMBER,
			CLOCK_DAY_OF_WEEK,
			CLOCK_START_TIME,
			NEXT = CLOCK_START_TIME + 4,
			
			NUM_PROGRAMMES = 64,
			PROGRAMME_DATA,
		};
	
};


#endif PERSISTANT_H
