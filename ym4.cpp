/*
	Arduino Library
	for YM4 Mobile Robot Platform
	YunFei Robotics Laboratory
	http://www.yfworld.com
	Version 0.1
	March 22, 2015
*/

#include <Arduino.h>
#include "ym4.h"

typedef void (*CALLBACK)(void);

void YM4Class::init(void) {

	// init Motor Control pin
	pinMode(LCTRL1_PIN, OUTPUT);
	pinMode(LCTRL2_PIN, OUTPUT);
	pinMode(LCTRL3_PIN, OUTPUT);

	pinMode(RCTRL1_PIN, OUTPUT);
	pinMode(RCTRL2_PIN, OUTPUT);
	pinMode(RCTRL3_PIN, OUTPUT);

	// init LED
	pinMode(LED_PIN, OUTPUT);
  
	// hook pulse sampling interrupt
	//attachInterrupt(LPULSE_INT, (CALLBACK)&YM4Class::lPulseSample, FALLING);
	//attachInterrupt(RPULSE_INT, (CALLBACK)&YM4Class::rPulseSapmle, FALLING);
	
}


void YM4Class::move(float speed) {
	// trim the input
	if (speed > 1) {
		speed = 1;
	} else if (speed < -1) {
		speed = -1;
	} else {
		;
	}
	
	if (speed > 0) {
		setMotorMode(M_LEFT, M_FORWARD);
		setMotorMode(M_RIGHT, M_FORWARD);
	} else if (speed < 0) {
		setMotorMode(M_LEFT, M_BACKWARD);
		setMotorMode(M_RIGHT, M_BACKWARD);		
	}

	int pmwVal;
	
	setMotorPWM(M_LEFT, 255);
	setMotorPWM(M_RIGHT, 255);
	
}


void YM4Class::turn(float speed) {
	if (speed > 0) {
		setMotorMode(M_LEFT, M_BACKWARD);
		setMotorPWM(M_LEFT, 255);
		setMotorMode(M_RIGHT, M_FORWARD);
		setMotorPWM(M_RIGHT, 255);
	} else {
		setMotorMode(M_LEFT, M_FORWARD);
		setMotorPWM(M_LEFT, 255);
		setMotorMode(M_RIGHT, M_BACKWARD);
		setMotorPWM(M_RIGHT, 255);		
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
    {
      digitalWrite(Ctrl1, LOW);     // OFF H Bridge
      digitalWrite(Ctrl2, HIGH);
      digitalWrite(Ctrl3, LOW);
      break;
    }

    case M_FORWARD:
    {
      digitalWrite(Ctrl1, LOW);     // OFF H Bridge
      digitalWrite(Ctrl2, LOW);
      digitalWrite(Ctrl3, HIGH);
      break;
    }
    
    case M_BRAKE:
    {
      digitalWrite(Ctrl1, LOW);     // OFF H Bridge
      digitalWrite(Ctrl2, LOW);
      digitalWrite(Ctrl3, LOW);
      break;
    }

    case M_FLOAT:
    {
      digitalWrite(Ctrl1, LOW);     // OFF H Bridge
      digitalWrite(Ctrl2, HIGH);
      digitalWrite(Ctrl3, HIGH);
      break;
    }
    
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


void YM4Class::lPulseSample(void) {
	;
}

void YM4Class::rPulseSapmle(void) {
	;
}