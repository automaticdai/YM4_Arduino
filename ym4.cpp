/** 
* @defgroup group1 YM4_Class
*			The Arduino Library of YM4 Mobile Robot Platform
* @author	Xiaotian Dai \n
* 			Yunfei Robotics Laboratory \n
* 			http://www.yfworld.com/
* @version	0.1.2b
* @date		April 21, 2015
* @{
*/

#include <Arduino.h>
#include "../MsTimer2/MsTimer2.h"
#include "ym4.h"

/** Set the period of the timer interrupt */
#define PERIOD_MS				(2)

/** Set the PWM frequency */
#define PWM_FREQUENCY_INIT()	(TCCR2B = (TCCR2B & 0b11111000) | 0x02)		

/* Global variables for inter-process communication */
int gnLPulseCnt;			/**< pulse counter of the left motor encoder */ 
int gnRPulseCnt; 			/**< pulse counter of the right motor encoder */

int gnLTickLast;
int gnRTickLast;

int gnLTimePerPulse = 100; 	/**< time interval between two left pulses */
int gnRTimePerPulse = 100; 	/**< time interval between two right pulses */

int gnTick;					/**< global timer counter, adds every PERIOD_MS \n
							// write by periodTask() */

int gnLPWM;
int gnRPWM;						
							
							
bool gbLEDStatus = false;	///< controls the status of the LED


/** 
* Init the YM4 hardware, including configurations of pins, LED & interrupts
*/
void YM4Class::init(void) {
	
	/* this statement will change the PWM output frequency to 7.8 KHZ */
	PWM_FREQUENCY_INIT();
	
	/* init Motor Control pin */
	pinMode(LCTRL1_PIN, OUTPUT);
	pinMode(LCTRL2_PIN, OUTPUT);
	pinMode(LCTRL3_PIN, OUTPUT);

	pinMode(RCTRL1_PIN, OUTPUT);
	pinMode(RCTRL2_PIN, OUTPUT);
	pinMode(RCTRL3_PIN, OUTPUT);

	/* init LED */
	pinMode(LED_PIN, OUTPUT);
	bLEDStatus = false;
	
	/* hook the encoder pulse sampling interrupt */
	attachInterrupt(LPULSE_INT, &YM4Class::lPulseSample, FALLING);
	attachInterrupt(RPULSE_INT, &YM4Class::rPulseSapmle, FALLING);
	
	/* enable the timer interrupt */
	MsTimer2::set(PERIOD_MS, YM4Class::periodicHandle); // periodic interrupt
	MsTimer2::start();
}


/** 
* Control the linear speed of the YM4.
* @param[in] speed: = [-1,+1]
* @see YM4Class::turn()
*/
void YM4Class::move(float speed) {
	int nPWMVal;
	
	/* trim the input */
	if (speed > 1) {
		speed = 1;
	} else if (speed < -1) {
		speed = -1;
	} else {
		;
	}
	
	/* set direction */
	if (speed > 0) {
		setMotorMode(M_LEFT, M_FORWARD);
		setMotorMode(M_RIGHT, M_FORWARD);
	} else if (speed < 0) {
		setMotorMode(M_LEFT, M_BACKWARD);
		setMotorMode(M_RIGHT, M_BACKWARD);
		speed = -1 * speed;
	}

	/* set speed */
	nPWMVal = (int)(speed * 255);
	setMotorPWM(M_LEFT, nPWMVal);
	setMotorPWM(M_RIGHT, nPWMVal);
	
}


/**
* Control the angular speed of the YM4.
* This is done by changing the difference between two wheels.
* @param[in] speed: = [-1,+1]
* @see YM4Class::move()
*/
void YM4Class::turn(float speed) {
	int nPWMVal;
	
	/* trim the input */
	if (speed > 1) {
		speed = 1;
	} else if (speed < -1) {
		speed = -1;
	} else {
		;
	}
	
	/* set direction */
	if (speed > 0) {
		setMotorMode(M_LEFT, M_BACKWARD);
		setMotorMode(M_RIGHT, M_FORWARD);
	} else {
		setMotorMode(M_LEFT, M_FORWARD);
		setMotorMode(M_RIGHT, M_BACKWARD);
		speed = -1 * speed;
	}
	
	/* set speed */
	nPWMVal = (int)(speed * 255);
	setMotorPWM(M_LEFT, nPWMVal);
	setMotorPWM(M_RIGHT, nPWMVal);	
}


/**
* Set the working mode of the motor
* @param[in] motor: = {M_LEFT, M_RIGHT}
* @param[in] mode: = {M_BACKWARD, M_FORWARD, M_BRAKE, M_FLOAT}
* @see YM4Class::move()
*/
void YM4Class::setMotorMode(int motor, int mode) {

	int Ctrl1, Ctrl2, Ctrl3;

	if (motor == M_LEFT) {
		Ctrl1 = LCTRL1_PIN;
		Ctrl2 = LCTRL2_PIN;
		Ctrl3 = LCTRL3_PIN;
	} else {	// M_RIGHT
		Ctrl1 = RCTRL1_PIN;
		Ctrl2 = RCTRL2_PIN;
		Ctrl3 = RCTRL3_PIN;
	}

	switch (mode) {
		case M_BACKWARD:
			digitalWrite(Ctrl1, LOW);     // OFF H Bridge
			digitalWrite(Ctrl2, HIGH);
			digitalWrite(Ctrl3, LOW);
			break;

		case M_FORWARD:
			digitalWrite(Ctrl1, LOW);     // OFF H Bridge
			digitalWrite(Ctrl2, LOW);
			digitalWrite(Ctrl3, HIGH);
			break;

		case M_BRAKE:
			digitalWrite(Ctrl1, LOW);     // OFF H Bridge
			digitalWrite(Ctrl2, LOW);
			digitalWrite(Ctrl3, LOW);
			break;

		case M_FLOAT:
			digitalWrite(Ctrl1, LOW);     // OFF H Bridge
			digitalWrite(Ctrl2, HIGH);
			digitalWrite(Ctrl3, HIGH);
			break;

		default: 
			;
	} 
}


/**
* set the PWM output value
* @param[in] motor: {M_LEFT, M_RIGHT}
* @param[in] usPwmVal: 0 - 255
*/
void YM4Class::setMotorPWM(int motor, unsigned char ucPwmVal) {
	if (motor == M_LEFT) {
		analogWrite(LCTRL1_PIN, ucPwmVal);
	} else {
		analogWrite(RCTRL1_PIN, ucPwmVal);
	}
}


/**
*
*/
void YM4Class::led(int ledmode) {
	switch (ledmode) {
		case LED_OFF:
			bLEDStatus = false;
			digitalWrite(LED_PIN, HIGH);
			break;
			
		case LED_ON:
			bLEDStatus = true;
			digitalWrite(LED_PIN, LOW);
			break;
		
		case LED_FLASH:
			bLEDStatus = !bLEDStatus;
			bLEDStatus? digitalWrite(LED_PIN, LOW) 
					  : digitalWrite(LED_PIN, HIGH);
			break;
	
	}
}


/**
*
*/
void YM4Class::lPulseSample(void) {
	gnLPulseCnt++;
	if (gnLTickLast != 0) {
		gnLTimePerPulse = (gnTick - gnLTickLast) * PERIOD_MS;
	}
	gnLTickLast = gnTick;
}


/**
*
*/
void YM4Class::rPulseSapmle(void) {
	gnRPulseCnt++;
	if (gnRTickLast != 0) {
		gnRTimePerPulse = (gnTick - gnRTickLast) * PERIOD_MS;
	}
	gnRTickLast = gnTick;
}


/**
* get the current speed of the motors
* @param[in,out] lSpd: Left motor speed
* @param[in,out] rSpd: Right motor speed
*/
void YM4Class::getSpeed(int &lSpd, int &rSpd) {
	lSpd = gnLTimePerPulse;
	rSpd = gnRTimePerPulse;
}


/**
* CLosed-loop speed PID controller
*/
void YM4Class::spdController(void) {
	int LRef = 30;
	int RRef = 30;
	int LErr = LRef - gnLTimePerPulse;
	int RErr = RRef - gnRTimePerPulse;
	
	if (LErr > 0) {
		if (gnLPWM > 0) {
			gnLPWM--;
		}
	} else {
		if (gnLPWM < 255) {
			gnLPWM++;
		}
	}
	
	
	/* set direction */
	setMotorMode(M_LEFT, M_FORWARD);
	setMotorMode(M_RIGHT, M_FORWARD);

	/* set speed */
	setMotorPWM(M_LEFT, gnLPWM);
	setMotorPWM(M_RIGHT, gnRPWM);
}

/**
* this function will be executed every PERIOD_MS 
*/  
void YM4Class::periodicHandle(void) {
	
	gnTick++;
	
	// control the system every 20ms (50Hz)
	if (gnTick % (20/PERIOD_MS) == 0) {
		spdController();
	}	
	
	// flash the LED every 500ms
 	if (gnTick % (500/PERIOD_MS) == 0) {
		gbLEDStatus = !gbLEDStatus;
		gbLEDStatus? digitalWrite(LED_PIN, LOW) : digitalWrite(LED_PIN, HIGH);
	}
}

/** @}*