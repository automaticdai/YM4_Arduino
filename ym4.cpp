/* 
	The Arduino Library
	of YM4 Mobile Robot Platform
	
	Xiaotian Dai
	YunFei Robotics Laboratory
	http://www.yfworld.com/
	
	Version 0.1.1b
	April 16, 2015
*/

#include "../MsTimer2/MsTimer2.h"
#include <Arduino.h>
#include "ym4.h"

/* Global variables for inter-process communication */
int gnLPulseCnt = 0;		// pulse count of the left motor, write by YM4
int gnRPulseCnt = 0; 		// pulse count of the right motor, write by YM4
int gnLTimePerPulse = 0; 	// 
int gnRTimePerPulse = 0; 	// 
int gnTick = 0;				// global timer counter, write by periodTask()

bool gbLEDStatus = false;

void YM4Class::init(void) {
	
	/* this statement changes the PWM output frequency to 7.8 KHZ */
	TCCR2B = (TCCR2B & 0b11111000) | 0x02;
	
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
	MsTimer2::set(2, YM4Class::periodicHandle); // 2ms interrupt
	MsTimer2::start();
}


void YM4Class::move(float speed) {
	int nPWMVal;
	
	// trim the input
	if (speed > 1) {
		speed = 1;
	} else if (speed < -1) {
		speed = -1;
	} else {
		;
	}
	
	// set direction
	if (speed > 0) {
		setMotorMode(M_LEFT, M_FORWARD);
		setMotorMode(M_RIGHT, M_FORWARD);
	} else if (speed < 0) {
		setMotorMode(M_LEFT, M_BACKWARD);
		setMotorMode(M_RIGHT, M_BACKWARD);
		speed = -1 * speed;
	}

	// set speed
	nPWMVal = (int)(speed * 255);
	setMotorPWM(M_LEFT, nPWMVal);
	setMotorPWM(M_RIGHT, nPWMVal);
	
}


void YM4Class::turn(float speed) {
	int nPWMVal;
	
	// trim the input
	if (speed > 1) {
		speed = 1;
	} else if (speed < -1) {
		speed = -1;
	} else {
		;
	}
	
	nPWMVal = (int)(speed * 255);
	
	if (speed > 0) {
		setMotorMode(M_LEFT, M_BACKWARD);
		setMotorMode(M_RIGHT, M_FORWARD);
		setMotorPWM(M_LEFT, nPWMVal);
		setMotorPWM(M_RIGHT, nPWMVal);
	} else {
		setMotorMode(M_LEFT, M_FORWARD);
		setMotorMode(M_RIGHT, M_BACKWARD);
		setMotorPWM(M_LEFT, nPWMVal);
		setMotorPWM(M_RIGHT, nPWMVal);		
	}
}


void YM4Class::setMotorMode(int motor, int mode) {

	int Ctrl1,Ctrl2,Ctrl3;

	if (motor == M_LEFT) {
		Ctrl1 = LCTRL1_PIN;
		Ctrl2 = LCTRL2_PIN;
		Ctrl3 = LCTRL3_PIN;
	} else {
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


void YM4Class::setMotorPWM(int motor, unsigned char ucPwmVal) {
	if (motor == M_LEFT) {
		analogWrite(LCTRL1_PIN, ucPwmVal);
	} else {
		analogWrite(RCTRL1_PIN, ucPwmVal);
	}
}


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
			bLEDStatus? digitalWrite(LED_PIN, LOW) : digitalWrite(LED_PIN, HIGH);
			break;
	
	}
}


void YM4Class::lPulseSample(void) {
	gnLPulseCnt++;
}


void YM4Class::rPulseSapmle(void) {
	gnRPulseCnt++;
}


void YM4Class::getSpeed(int &lSpd, int &rSpd) {
	lSpd = gnLTimePerPulse;
	rSpd = gnRTimePerPulse;
}


/* For speed calculation */
int gnLPulseThis;
int gnLPulseLast;
int gnLTickLast;

int gnRPulseThis;
int gnRPulseLast;
int gnRTickLast;

// this function will be executed every 2ms  
void YM4Class::periodicHandle(void) {
	
	gnTick++;
	
	gnLPulseThis = gnLPulseCnt;
	gnRPulseThis = gnRPulseCnt;

	if (gnLPulseThis != gnLPulseLast) {
		if (gnLTickLast != 0) {
			gnLTimePerPulse = gnTick - gnLTickLast;
		}
		gnLTickLast = gnTick;
		gnLPulseLast = gnLPulseThis;
	}

	if (gnRPulseThis != gnRPulseLast) {
		if (gnRTickLast != 0) {
			gnRTimePerPulse = gnTick - gnRTickLast;
		}
		gnRTickLast = gnTick;
		gnRPulseLast = gnRPulseThis;
	}
	
	// flash the LED every 500ms
 	if (gnTick % 250 == 0) {
		gbLEDStatus = !gbLEDStatus;
		gbLEDStatus? digitalWrite(LED_PIN, LOW) : digitalWrite(LED_PIN, HIGH);
		
		//gnLTimePerPulse = gnLPulseCnt;
		//gnRTimePerPulse = gnRPulseCnt;
	}
	
	
}