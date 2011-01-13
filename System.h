#ifndef SYSTEM_H
#define SYSTEM_H

#include <WProgram.h>

#include <Persistant.h>
#include <Clock.h>
#include <CalibratedServo.h>
#include <Programme.h>

#define MAX_PROGRAMMES 10


class System {
	private:
		typedef enum status {
			STATUS_TIME_OUT_OF_SYNC = 0,
			STATUS_NO_PROGRAMS      = 1,
			STATUS_PROGRAMED        = 2,
			STATUS_PROGRAM_RUNNING  = 3,
		} status_t;
		
		typedef enum response {
			FALIURE = 0,
			SUCCESS = 1,
		} response_t;
	
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
		
		void tick(void);
		
		void setLED(void);
		
		void handleCommands(void);
	
	private:
		bool ledState;
		int  ledPin;
		
		int defaultTemperature;
		
		status_t status;
		
		CalibratedServo thermostat;
		
		uint8_t lastDayNumber;
		uint8_t lastDayOfWeek;
		Clock clock;
		
		void write(char value);
		void uLongWrite(unsigned long value);
		
		char blockingRead(void);
		unsigned long blockingULongRead(void);
		
		uint8_t numProgrammes;
		Programme programmes[MAX_PROGRAMMES];
};


#endif
