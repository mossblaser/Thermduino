#ifndef PERSISTANT_H
#define PERSISTANT_H

#include <WProgram.h>
#include <EEPROM.h>

#include <CalibratedServo.h>


#define PERSISTANT_GETTER_SETTER(addr, gname, sname) \
	uint8_t gname(void         ) { return EEPROM.read(addr); }; \
	void    sname(uint8_t value) { EEPROM.write(addr, value); }


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
		
		void calibrateServo(CalibratedServo &servo);
		void saveServoCalibration(CalibratedServo &servo);
	
	private:
		enum {
			CAL_MIN_ANGLE,
			CAL_MIN_TEMPERATURE,
			CAL_MAX_ANGLE,
			CAL_MAX_TEMPERATURE,
			DEFAULT_TEMPERATURE,
		};
	
};


#endif PERSISTANT_H
