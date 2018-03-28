//Name: Qiufeng Du
//ID: 1439484
//Section: EA2
#include <Arduino.h>

//Make two arrays to represent the 5 LED lights
//all of them are turned off initially
int LEDPins[5] = {9, 10, 11, 12, 13};
int LED[5] = {LOW, LOW, LOW, LOW, LOW};

//The pin number for buttons
int ButtonPin1 = 4;
int ButtonPin2 = 5;

//set up the input and outputs
void setup(){
  int i = 0;//i is used for selecting lights
  //use while loop
  while (i<5){
    pinMode(LEDPins[i],OUTPUT);
    digitalWrite(LEDPins[i],LED[i]);//turn all the lights off initially
    i = i++;
  }
  //set up the buttons
  pinMode(ButtonPin1,INPUT);
  pinMode(ButtonPin2,INPUT);
  digitalWrite(ButtonPin1, HIGH);
  digitalWrite(ButtonPin2, HIGH);
}
//use for loop to convert decimal number to binary code
void lights_on_off(int press){
  for (int i = 0; i < 5; i++) {
    if (press % 2) {
      LED[i] = HIGH;
      digitalWrite(LEDPins[i], LED[i]);}
    else {
      LED[i] = LOW;
      digitalWrite(LEDPins[i], LED[i]);}
    press = press/2;
  }
}
int PressCount = 0;//give the initial value for PressCount to
                  //calculate the increase and decrease of the binary codes
//Make a loop in order to make the previous code run
void loop(){
  //initial value of press time.

  if ( digitalRead(ButtonPin1) == LOW ) {
    PressCount++;//increament button
  }
  if ( digitalRead(ButtonPin2) == LOW ) {
    PressCount--;//decreament button
  }

  //use the lights_on_off function to control which light should be turned on/off
  lights_on_off(PressCount);
  delay(150);
}
