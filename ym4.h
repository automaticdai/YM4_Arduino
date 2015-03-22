#ifndef _YM4_H_INCLUDED
#define _YM4_H_INCLUDED

/* Digital Output */
// Control pins for the left motor
#define LCTRL1_PIN		(9)
#define LCTRL2_PIN		(8)
#define LCTRL3_PIN		(7)

// Control pins for the right motor
#define RCTRL1_PIN		(10)	// for the older pcb, this was Pin 6
#define RCTRL2_PIN		(5)
#define RCTRL3_PIN		(4)

#define LED_PIN			(13)

/* Analog Input */
#define LMOTORI_PIN		(6)
#define LMOTORV_PIN		(1)

#define RMOTORI_PIN		(7)
#define RMOTORV_PIN		(0)

/* Interrupt Number */
#define LPULSE_INT 		(1)
#define RPULSE_INT 		(0)

/* Motor Modes */
#define M_FORWARD  		(1)
#define M_BACKWARD 		(2)
#define M_BRAKE    		(3)
#define M_FLOAT    		(4)

#define M_LEFT     		(0)
#define M_RIGHT    		(1)

class YM4Class {
public:
	YM4Class() {;}
	void init(void);
	void move(float speed);
	void turn(float speed);
  
private:
	void setMotorMode(int motor, int mode);
	void setMotorPWM(int motor, unsigned char ucPwmVal);
	void lPulseSample(void);
	void rPulseSapmle(void);
};

#endif