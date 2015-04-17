#ifndef _YM4_H_INCLUDED
#define _YM4_H_INCLUDED

/* Pin Table */
/* Control pins for the left motor */
#define LCTRL1_PIN		(9)
#define LCTRL2_PIN		(8)
#define LCTRL3_PIN		(7)

/* Control pins for the right motor */
#define RCTRL1_PIN		(10)	/**< NOTE: for older version pcb, 
									this was Pin 6 */
#define RCTRL2_PIN		(5)
#define RCTRL3_PIN		(4)

/* LED Pin */
#define LED_PIN			(13)	/**< Pin number of the LED */

/* Analog Input Pins */
#define LMOTORI_PIN		(6)
#define LMOTORV_PIN		(1)

#define RMOTORI_PIN		(7)
#define RMOTORV_PIN		(0)

/* Interrupt Number */
#define LPULSE_INT 		(1)		/**< interrupt of the left-side spd encoder */
#define RPULSE_INT 		(0)		/**< interrupt of the right-side spd encoder */

/* Motor Modes */
#define M_LEFT     		(0)
#define M_RIGHT    		(1)

#define M_FORWARD  		(1)
#define M_BACKWARD 		(2)
#define M_BRAKE    		(3)
#define M_FLOAT    		(4)

/* LED Modes */
#define LED_OFF			(0)
#define LED_ON			(1)
#define LED_FLASH		(2)

/**
* This is the Class Wrapper of the YM4 Arduino Mobile Robot.
* This class handles all the hardware of the YM4 platform and provides
* easy-to-use APIs for different user applications.
*/
class YM4Class {
public:
	YM4Class() {;}
	void init(void);
	void move(float speed);		// speed = [-1,1]
	void turn(float speed);		// speed = [-1,1]
	void led(int ledmode);
	void getSpeed(int &lSpd, int &rSpd);
	
	static void lPulseSample(void);
	static void rPulseSapmle(void);
	static void periodicHandle(void);
	
private:
	void setMotorMode(int motor, int mode);
	void setMotorPWM(int motor, unsigned char ucPwmVal);
	
	void spdEstimation(void) {;}
	void posEstimation(void) {;}

};

#endif