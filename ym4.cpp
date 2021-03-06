/** 
* @defgroup group1 YM4_Class
*			The Arduino Library of YM4 Mobile Robot Platform
* @author	Xiaotian Dai \n
* 			Yunfei Robotics Laboratory \n
* 			http://www.yfworld.com/
* @version	0.1.4a
* @date		April 22, 2015
* @{
*/

#include <Arduino.h>
#include "../MsTimer2/MsTimer2.h"
#include "ym4.h"

/** 
* @defgroup group2 Global_Variables
* Global variables for inter-process communication
* @{
*/
int gnLPulseCnt;			/**< pulse counter of the left motor encoder */ 
int gnRPulseCnt; 			/**< pulse counter of the right motor encoder */

int gnLTickLast;			/**< time instance of the last left pulse */
int gnRTickLast;			/**< time instance of the last right pulse */

int gnLTimePerPulse = 100; 	/**< time interval between two left pulses */
int gnRTimePerPulse = 100; 	/**< time interval between two right pulses */

int gnTick;					/**< global timer counter, increases every \n
							// PERIOD_MS in periodTask() */

int gnLPWM;					/**< the PWM value of the left motor: 0-255 */
int gnRPWM;					/**< the PWM value of the right motor: 0-255 */	
							
							
bool gbLEDStatus = false;	/**< controls the status of the LED */
/** @}*/


/** 
* Init the YM4 hardware, including configurations of pins, LED & interrupts
*/
void YM4_init(void) {
	
	/* this statement will change the PWM output frequency to 7.8 KHZ */
	//PWM_FREQUENCY_INIT();
	
	/* init Motor Control pin */
	pinMode(LCTRL1_PIN, OUTPUT);
	pinMode(LCTRL2_PIN, OUTPUT);
	pinMode(LCTRL3_PIN, OUTPUT);

	pinMode(RCTRL1_PIN, OUTPUT);
	pinMode(RCTRL2_PIN, OUTPUT);
	pinMode(RCTRL3_PIN, OUTPUT);

	/* init LED */
	pinMode(LED_PIN, OUTPUT);
	gbLEDStatus = false;
	
	/* hook the encoder pulse sampling interrupt */
	attachInterrupt(LPULSE_INT, &YM4_lPulseSample, FALLING);
	attachInterrupt(RPULSE_INT, &YM4_rPulseSapmle, FALLING);
	
	/* enable the timer interrupt */
	MsTimer2::set(PERIOD_MS, YM4_periodicHandle); // periodic interrupt
	MsTimer2::start();
}


/** 
* Control the linear speed of the YM4.
* @param[in] speed: = [-1,+1]
* @see YM4_turn()
*/
void YM4_move(float speed) {
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
		YM4_setMotorMode(M_LEFT, M_FORWARD);
		YM4_setMotorMode(M_RIGHT, M_FORWARD);
	} else if (speed < 0) {
		YM4_setMotorMode(M_LEFT, M_BACKWARD);
		YM4_setMotorMode(M_RIGHT, M_BACKWARD);
		speed = -1 * speed;
	}

	/* set speed */
	nPWMVal = (int)(speed * 255);
	YM4_setMotorPWM(M_LEFT, nPWMVal);
	YM4_setMotorPWM(M_RIGHT, nPWMVal);
	
}


/**
* Control the angular speed of the YM4.
* This is done by changing the difference between two wheels.
* @param[in] speed: = [-1,+1]
* @see YM4Class::move()
*/
void YM4_turn(float speed) {
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
		YM4_setMotorMode(M_LEFT, M_BACKWARD);
		YM4_setMotorMode(M_RIGHT, M_FORWARD);
	} else {
		YM4_setMotorMode(M_LEFT, M_FORWARD);
		YM4_setMotorMode(M_RIGHT, M_BACKWARD);
		speed = -1 * speed;
	}
	
	/* set speed */
	nPWMVal = (int)(speed * 255);
	YM4_setMotorPWM(M_LEFT, nPWMVal);
	YM4_setMotorPWM(M_RIGHT, nPWMVal);	
}


/**
* Set the working mode of the motor
* @param[in] motor: = {M_LEFT, M_RIGHT}
* @param[in] mode: = {M_BACKWARD, M_FORWARD, M_BRAKE, M_FLOAT}
* @see YM4Class::move()
*/
void YM4_setMotorMode(int motor, int mode) {

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
void YM4_setMotorPWM(int motor, unsigned char ucPwmVal) {
	if (motor == M_LEFT) {
		analogWrite(LCTRL1_PIN, ucPwmVal);
	} else {
		analogWrite(RCTRL1_PIN, ucPwmVal);
	}
}


/**
*
*/
void YM4_led(int ledmode) {
	switch (ledmode) {
		case LED_OFF:
			gbLEDStatus = false;
			digitalWrite(LED_PIN, HIGH);
			break;
			
		case LED_ON:
			gbLEDStatus = true;
			digitalWrite(LED_PIN, LOW);
			break;
		
		case LED_FLASH:
			gbLEDStatus = !gbLEDStatus;
			gbLEDStatus? digitalWrite(LED_PIN, LOW) 
					  : digitalWrite(LED_PIN, HIGH);
			break;
	
	}
}


/**
*
*/
void YM4_lPulseSample(void) {
	gnLPulseCnt++;
	if (gnLTickLast != 0) {
		gnLTimePerPulse = (gnTick - gnLTickLast) * PERIOD_MS;
	}
	gnLTickLast = gnTick;
}


/**
*
*/
void YM4_rPulseSapmle(void) {
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
void YM4_getSpeed(int *lSpd, int *rSpd) {
	*lSpd = gnLTimePerPulse;
	*rSpd = gnRTimePerPulse;
}


/**
* CLosed-loop speed PID controller
*/
void YM4_spdController(void) {
	int LRef = 15;
	int RRef = 15;
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

	if (RErr > 0) {
		if (gnRPWM > 0) {
			gnRPWM--;
		}
	} else {
		if (gnRPWM < 255) {
			gnRPWM++;
		}
	}
	
	/* set direction */
	YM4_setMotorMode(M_LEFT, M_FORWARD);
	YM4_setMotorMode(M_RIGHT, M_FORWARD);

	/* set speed */
	YM4_setMotorPWM(M_LEFT, gnLPWM);
	YM4_setMotorPWM(M_RIGHT, gnRPWM);
}


/**
* (this function is optional, only valid with the IMU module)
* estimate the current state (spd_x, spd_y, acc_x, acc_y, ang_z) from the IMU
*/
void YM4_stateEstimation(void) {
	;
}


/**
* (this function is optional, only valid with the IMU module)
* estimate the current position based on the IMU measurements 
*/
void YM4_posEstimation(void) {
	;
}


/**
* this handle will be executed every PERIOD_MS 
*/  
void YM4_periodicHandle(void) {
	
	gnTick++;
	
	// control the system every 20ms (50Hz)
	if (gnTick % (20/PERIOD_MS) == 0) {
		YM4_spdController();
	}	
	
	// flash the LED every 500ms
 	if (gnTick % (500/PERIOD_MS) == 0) {
		gbLEDStatus = !gbLEDStatus;
		gbLEDStatus? digitalWrite(LED_PIN, LOW) : digitalWrite(LED_PIN, HIGH);
	}
}
/** @}*/