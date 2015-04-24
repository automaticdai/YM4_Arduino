# YM4_Arduino
This project holds the Arduino library of the YM4 Mobile Robot platform. 

## What is YM4?
The YM4 is a mobile robot platform designed for robot amateurs and now is at its 4th generation. The YM4 is a cube with a size of 7.5cm * 7.5cm * 7.5 cm, which is the standard of the FIRA robot football competition. The robot uses a Arduino-compatiable microcontroller, the AVR(R) ATMega 328, and it can be programmed with the Arduino IDE.

## How to install the library?
Download all the project files and extract the folder into $(Arduino_dir)\libraries, where Arduino_dir is the directory where your Arduino is installed.

## How to use the library?
The examples of the usage are included in the library. Once installed the library, you can find them in the Arduino IDE from File->Examples->YM4_Arduino.

## Resource usage of the library.
This library uses about half of the Arduino hardware resources.

Here is a list of what you can use in your own program: 

1. SPI (or uses as digital pins D10, D11, D12, D13)

2. IIC (or uses as analog A4 and A5)

3. A2 and A3.

This library also uses the MsTimer library, so it cannot be used in the user's program. The library provides a time base, increasing per 1ms, which could be used as a timer.

## More about YM4 and this library
More information of the YM4 and the library could be found on my blog: http://www.yfworld.com.

## Release Note
v0.1.5a, April 24, 2015: changed the style of some comments. add voltage and current sampling (dummy) function.

v0.1.5, April 23, 2015: converted the code back to c (for compatitability of MsTimer) 

v0.1.4, April 22, 2015: converted the code from c++ into c

v0.1.3, April 21, 2015: tested on the real hardware and corrected some compile errors

v0.1.2b, April 21, 2015: added experimental code of closed-loop speed control

v0.1.2a, April 17, 2015: changed the comment style to support doxygen

v0.1.1b, April 16, 2015: added the code to obtain the motor speed

v0.1.1, April 15, 2015: optimized the speed control code

v0.1, March 22, 2015: init commit of the code
