/* Name: Qiufeng Du, Hai Huang
Student number: 1439484, 1500228
Section: A2

What you type on one Arduino shows up in the screen
attached to the other one and vice versa.
*/
#include <Arduino.h>
// set the global variable
long timeout = 1000; // times to wait
uint8_t nbytes = 1; // bytes need to receive
const char CR = 'C';
const char ACK = 'A';

/* Calculates and returns (a * b) mod m
Parameters:
a: a: base, nonnegative integer
b: multiplier, nonnegative integer
m: positive integer
*this function calculates each multiplication in side the pow_mod function
*/
uint32_t mul_mod(uint32_t a, uint32_t b, uint32_t m){
  int p = a%m;
  int r = 0%m;
  for(int i=0; i<32; ++i){
    if (b & (1ul<<i)!=0){
      r = (r+p)%m;
    }
    p = (p*2)%m;
  }
  return(r);
}
/* Calculates and returns (a**b) mod m.
Parameters:
a: base, nonnegative integer
b: exponent, nonnegative integer, a=b=0 not allowed
m: positive integer
*/
uint32_t pow_mod( uint32_t a, uint32_t b, uint32_t m ) {
  uint32_t r = 1 % m; // will store the result at the end
  uint32_t p = a % m; // will store a^{2^i} % m for i=0,1,...
  for (int i=0; i<32; ++i) {
    if ( (b & (1ul<<i))!=0 ) { // check i-th bit of b
      r = mul_mod(r, p, m);
    }
    p = mul_mod(p, p, m);//since p is always in 16 bits, p*p should not over
    //32 bits, overflow avoide.
  }
  return r;
}


//Read the nunber in analogPin1 while analogPin1 is not connected with anything
uint32_t get_private_key(){
  int analogPin = 1;
  uint32_t val=0; //use this variable to read the analogpin number
  int times;
  unsigned int result = 0;
  int i = 0;// use i to get the least significant bit of the number
  uint32_t finalvalue = 0;
  for (times = 31; times>=0; times--){
    val = analogRead(analogPin);
    //get the least significant bit of the value
    i = val%2;
    //reform the 16 bit random number using the value
    finalvalue = finalvalue+(i<<times);
    //wait for some time to allow the voltage on the pin fluctuate
    delay(50);
  }
  //Serial.print(finalvalue);
  return (finalvalue);
}
/** Waits for a certain number of bytes on Serial3 or timeout
 * @param nbytes: the number of bytes we want
 * @param timeout: timeout period (ms); specifying a negative number
 *                turns off timeouts (the function waits indefinitely
 *                if timeouts are turned off).
 * @return True if the required number of bytes have arrived.
 */
bool wait_on_serial3( uint8_t nbytes, long timeout ) {
  unsigned long deadline = millis() + timeout;//wraparound not a problem
  while (Serial3.available()<nbytes && (timeout<0 || millis()<deadline))
  {
    delay(1); // be nice, no busy loop
  }
  return Serial3.available()>=nbytes;
}

/** Writes an uint32_t to Serial3, starting from the least-significant
* and finishing with the most significant byte.
*/
void uint32_to_serial3(uint32_t num) {
  Serial3.write((char) (num >> 0));
  Serial3.write((char) (num >> 8));
  Serial3.write((char) (num >> 16));
  Serial3.write((char) (num >> 24));
}

/** Reads an uint32_t from Serial3, starting from the least-significant
* and finishing with the most significant byte.
*/
uint32_t uint32_from_serial3() {
  uint32_t num = 0;
  num = num | ((uint32_t) Serial3.read()) << 0;
  num = num | ((uint32_t) Serial3.read()) << 8;
  num = num | ((uint32_t) Serial3.read()) << 16;
  num = num | ((uint32_t) Serial3.read()) << 24;
  return num;
}
/** State machine for client, returns server's public key
* Parameters:
skey: public key received from partner
ckey: client's public key
*/
uint32_t client(uint32_t skey, uint32_t ckey){
  enum State {Start, WaitingForAct, DataExchage};
  State curr_state = Start;
  while(true){ //wait for actions
    if (curr_state == Start){
      Serial3.print(CR); //send CR
      uint32_to_serial3(ckey); // send ckey
      Serial.println("sending CR(ckey)");
      curr_state = WaitingForAct;
    }
    else if((curr_state == WaitingForAct) && (wait_on_serial3(nbytes, timeout) == 1) && (Serial3.read() == ACK)){
      skey = uint32_from_serial3(); //receive and store another Arduino's key
      Serial3.print(ACK); //send ACK
      Serial.println("skey received");
      curr_state = DataExchage;
    }
    else if ( ((curr_state == WaitingForAct) && (wait_on_serial3(nbytes, timeout) == 0)) && (Serial3.read() != ACK) ){
      curr_state = Start; //when timeout,go back
    }
    else if (curr_state == DataExchage){
      Serial.println("Good to go!");
      Serial3.println("Your partner is ready! ");
      break;
    }
  }
  return(skey);
}

/** State machine for server, returns client's public key
* Parameters:
ckey: public key received from partner
skey: server's public key
*/
uint32_t server(uint32_t skey, uint32_t ckey){
  enum State{Listen, WaitingForKey, WaitingForAck, DataExchage};
  State curr_state = Listen;
  while(true){
    if ((curr_state == Listen) ){
      if ((wait_on_serial3 (nbytes, timeout)) && (Serial3.read() == CR)){
        curr_state = WaitingForKey;  // when CR is received go to the next stage
      }
      else {
        ; //do nothing
      }
    }
    else if (curr_state == WaitingForKey){
      ckey = uint32_from_serial3(); //receive and store ckey from client Arduino
      Serial.println("ckey received");
      Serial3.print(ACK); //send ACK
      uint32_to_serial3(skey);// send skey
      curr_state = WaitingForAck;
    }
    else if((curr_state == WaitingForAck)){
      if (wait_on_serial3(nbytes, timeout) && (Serial3.read() == ACK)){
        curr_state = DataExchage;
        Serial.println("Good to go!");
        Serial3.println("Your partner is ready! ");
        break;
      }
      else if (wait_on_serial3(nbytes,timeout) && Serial3.read() == CR){
        curr_state = WaitingForKey; // if still receving CR, go to the previous stage
      }

    }
    else {
      curr_state = Listen;
    }
  }
  return(ckey);
}

/** Implements the Park-Miller algorithm with 32 bit integer arithmetic
* @return ((current_key * 48271)) mod (2^31 - 1);
* This is linear congruential generator, based on the multiplicative
* group of integers modulo m = 2^31 - 1.
* The generator has a long period and it is relatively efficient.
* Most importantly, the generator's modulus is not a power of two
* (as is for the built-in rng),
* hence the keys mod 2^{s} cannot be obtained
* by using a key with s bits.
* Based on:
* http://www.firstpr.com.au/dsp/rand31/rand31-park-miller-carta.cc.txt
*/
uint32_t next_key(uint32_t current_key) {
  const uint32_t modulus = 0x7FFFFFFF; // 2^31-1
  const uint32_t consta = 48271;  // we use that consta<2^16
  uint32_t lo = consta*(current_key & 0xFFFF);
  uint32_t hi = consta*(current_key >> 16);
  lo += (hi & 0x7FFF)<<16;
  lo += hi>>15;
  if (lo > modulus) lo -= modulus;
  return lo;
}


int main() {
  init();
  while (true) {
    uint32_t prime = 2147483647;
    uint32_t generator = 16807;
    uint16_t private_key = get_private_key();
    // uint32_t public_key = pow_mod( generator, uint32_t(private_key), prime );
    //get shared_secret_key
    uint32_t current_key;
    //char secret_key = shared_secret;
    // lowest 8 bits of the shared_secret
    uint32_t skey = 0;
    uint32_t ckey = 0;
    Serial.begin(9600);
    Serial3.begin(9600);
    int analogPin = 13;
    pinMode(analogPin,INPUT);
    /* if else statement to recognize which Arduino is server or client
    analogPin = 13
    */
    if (digitalRead(analogPin) == HIGH){
      Serial.println("I'm server");
      skey = pow_mod(generator, uint32_t(private_key), prime);
      Serial.print("Public_key is: ");
      Serial.println(skey);
      server(skey,ckey); // get the ckey
      // compute the secret_key for server
      current_key = pow_mod( ckey, private_key, prime );
    }
    else {
      Serial.println("I'm client");
      ckey = pow_mod(generator, uint32_t(private_key), prime);
      Serial.print("Public_key is: ");
      Serial.println(ckey);
      client(skey,ckey);// get the skey
      // compute the secret_key for client
      current_key = pow_mod( skey, private_key, prime );
    }

    while (true) {
      char secret_key = current_key;
      // PC keystroke -> other Arduino
      if (Serial.available()>0) {
        char c = Serial.read();
        // Serial.write(c); // echo back to screen
        Serial.print("ASCII code of character to be sent: ");
        Serial.println((int)c);
        // encrypt data:
        c = c ^ secret_key;
        Serial.print("Encrypted information being sent: ");
        Serial.println((int)c);
        Serial3.write(c); // sending byte to the "other" Arduino
      }
      // Other Arduino to PC screen
      if (Serial3.available()>0) {
        char c = Serial3.read();
        // decrypt data:
        c = c ^ secret_key;
        Serial.write(c); // show byte on screen as character
      }
      // update the key each time
      uint32_t current_key = next_key(current_key);
    }
    Serial3.end();
    Serial.end();
    return 0;
  }
}
