#ifndef CALIBRATEDSERVO_H
#define CALIBRATEDSERVO_H

#include <WProgram.h>
#include <Servo.h>


class CalibratedServo : public Servo {
	public:
		CalibratedServo(void);
		~CalibratedServo(void);
		
		uint8_t getTemperature(void);
		void    setTemperature(uint8_t temperature);
		
		uint8_t getMinAngle(void);
		uint8_t getMinTemperature(void);
		uint8_t getMaxAngle(void);
		uint8_t getMaxTemperature(void);
		
		void setMinAngle(uint8_t angle);
		void setMinTemperature(uint8_t temperature);
		void setMaxAngle(uint8_t angle);
		void setMaxTemperature(uint8_t temperature);
	
	private:
		// Calibration data
		uint8_t minAngle;
		uint8_t minTemperature;
		uint8_t maxAngle;
		uint8_t maxTemperature;
	
};


#endif
