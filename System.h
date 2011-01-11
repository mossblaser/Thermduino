#ifndef SYSTEM_H
#define SYSTEM_H

#include <WProgram.h>

#include <Persistant.h>
#include <CalibratedServo.h>


class System {
	private:
		typedef enum status {
			STATUS_NO_PROGRAMS,
			STATUS_PROGRAMED,
			STATUS_PROGRAM_RUNNING,
			STATUS_TIME_OUT_OF_SYNC,
		} status_t;
	
	public:
		PersistantSettings settings;
		
		System(void);
		
		void setThermostatPin(int pin);
		void setLEDPin(int pin);
		
		void loadSettings(void);
		void saveSettings(void);
		
		int  getDefaultTemperature(void);
		void setDefaultTemperature(int temperature);
		
		void setTemperature(void);
		int  getTemperature(void);
		
		void setStatus(status_t status);
		status_t getStatus(void);
		
		void setLED(void);
		
		void handleCommands(void);
	
	private:
		int ledPin;
		int defaultTemperature;
		
		status_t status;
		
		CalibratedServo thermostat;
};


#endif
