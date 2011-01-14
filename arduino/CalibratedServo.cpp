#include <WProgram.h>
#include <Servo.h> 
#include <CalibratedServo.h>


CalibratedServo::CalibratedServo(void)
	: Servo::Servo()
	, minAngle(0)
	, minTemperature(0)
	, maxAngle(0)
	, maxTemperature(0)
	, lastTemperature(0)
{
	// Do nothing...
}


CalibratedServo::~CalibratedServo(void)
{
	// Do nothing...
}


void
CalibratedServo::setTemperature(uint8_t temperature)
{
	if (temperature > getMaxTemperature()) temperature = getMaxTemperature();
	if (temperature < getMinTemperature()) temperature = getMinTemperature();
	lastTemperature = temperature;
	int deltaTemp = getMaxTemperature() - getMinTemperature();
	int deltaAngle = getMaxAngle() - getMinAngle();
	if (deltaTemp == 0) return;
	int angle = ((((int)temperature - (int)getMinTemperature()) * (int)deltaAngle)
	             / (int)deltaTemp)
	            + getMinAngle();
	if (angle <= getMaxAngle() && angle >= getMinAngle())
		write(angle);
}


uint8_t
CalibratedServo::getTemperature(void)
{
	return lastTemperature;
}


uint8_t CalibratedServo::getMinAngle(void)       { return minAngle;       }
uint8_t CalibratedServo::getMinTemperature(void) { return minTemperature; }
uint8_t CalibratedServo::getMaxAngle(void)       { return maxAngle;       }
uint8_t CalibratedServo::getMaxTemperature(void) { return maxTemperature; }

void CalibratedServo::setMinAngle(uint8_t value)       { minAngle = value;       }
void CalibratedServo::setMinTemperature(uint8_t value) { minTemperature = value; }
void CalibratedServo::setMaxAngle(uint8_t value)       { maxAngle = value;       }
void CalibratedServo::setMaxTemperature(uint8_t value) { maxTemperature = value; }
