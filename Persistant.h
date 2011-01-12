#ifndef PERSISTANT_H
#define PERSISTANT_H

#include <WProgram.h>
#include <EEPROM.h>

#include <Clock.h>
#include <CalibratedServo.h>


#define PERSISTANT_GETTER_SETTER(addr, gname, sname) \
	uint8_t gname(void         ) { return EEPROM.read(addr); }; \
	void    sname(uint8_t value) { /*EEPROM.write(addr, value);*/ }


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
		
		void calibrateServo(CalibratedServo &servo);
		void saveServoCalibration(CalibratedServo &servo);
		
		void loadClock(Clock &clock);
		void saveClock(Clock &clock);
	
	private:
		enum {
			CAL_MIN_ANGLE,
			CAL_MIN_TEMPERATURE,
			CAL_MAX_ANGLE,
			CAL_MAX_TEMPERATURE,
			DEFAULT_TEMPERATURE,
			CLOCK_DAY_NUMBER,
			CLOCK_DAY_OF_WEEK,
		};
	
};


#endif PERSISTANT_H
