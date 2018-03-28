Name: QIUFENG DU
ID no.: 1439484
Section: EA2

Accessories:
* 560 Ohm (Grn Blu Brown) resistor
* 5 LED Lights
* Arduino Mega Board
* Pushbutton
* Wires

Wiring instructions:
Arduino Power 5V <--> Red Arduino Power Bus
Arduino Power GND <--> Blue Arduino GND Bus
Arduino Pin 9 <--> longer LED lead |LED| Shorter LED lead <-->    Resistor <--> Blue Arduino GND Bus
Arduino Pin 10 <--> longer LED lead |LED| Shorter LED lead <--> Resistor <--> Blue Arduino GND Bus
Arduino Pin 11 <--> longer LED lead |LED| Shorter LED lead <--> Resistor <--> Blue Arduino GND Bus
Arduino Pin 12 <--> longer LED lead |LED| Shorter LED lead <--> Resistor <--> Blue Arduino GND Bus
Arduino Pin 13 <--> longer LED lead |LED| Shorter LED lead <--> Resistor <--> Blue Arduino GND Bus
Arduino pin 4 <--> Pushbutton <--> Blue Arduino GND Bus
Arduino pin 5 <--> Pushbutton <--> Blue Arduino GND Bus

Code running instruction:
In this lab, the basic idea of Decimal to Binary number conversion is applied. 5 LED lights are connected, and the resistors are also connected with LED lights. If the LED is on, it means 1. If it is off, then it means 0. We use the pushbutton to change the lights' conditions. One pushbutton performs the increment of binary operation, which means the number will be increased by 1 if the button is pressed.Another button performs the decrement of binary operation.

Inside the code, instead of using many simple codes, I used arrays to simplify my code.Also, I used both for and while loop to accomplish the repetition. In side the for loop, the decimal number is devided by 2 firstly, then the remainder is the first binary digit. Divide the quotient from the previous step by 2, the new remainder is the second digit. Do the previous step repetitively until it reaches zero.
