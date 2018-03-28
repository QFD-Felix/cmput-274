/*Name Qiufeng Du
ID: 1439484
Section: EA2*/

#include <Arduino.h>

//get a input number
uint32_t get_number() {
  const uint32_t MAX_CHARACTERS = 100;
  char line[MAX_CHARACTERS+1];
  //int get;
  // wait for data
  uint32_t i = 0;
  while (true) {
    while (Serial.available()==0) {
    }
    // read the incoming byte:
    uint32_t incomingByte = Serial.read();
    if (incomingByte==13) {
      Serial.print("");
      line[i] = 0;
      break;
    }
    else {
      line[i] = incomingByte;
    }
    ++i;
  }
  Serial.println(line);
  //use strtoul function to
  return strtoul(line,'\0',10);
}

int num_bits_unsigned (unsigned long x){
  long Counting[32];//make an array which have 32 bits.
  int number;
  /*use for to check where the first bit is, and use if to break the loop
  if get the first bit's position.*/
  for (int i=31; i>=0; --i){
    Counting[31-i] = ((x>>i)&1);}
    //use for loop to get how many bits are needed.
    for (int j = 0; j<32; j++){
      if (Counting[j]==1){
        number = 32-j;
        break;
      }
    }
    Serial.print(number);
    Serial.println("");
  }

  //a function to get the value for octet.
  uint8_t get_noctet() {
    const uint32_t MAX_CHARACTERS = 100;
    char select[MAX_CHARACTERS+1];
    // wait for data
    uint32_t i = 0;
    while (true) {
      while (Serial.available()==0) {
      }
      // read the incoming byte:
      uint32_t incomingByte = Serial.read();
      if (incomingByte==13) {
        Serial.print("");
        select[i] = 0;
        break;
      }
      else {
        select[i] = incomingByte;
      }
      ++i;
    }
    // print which selection is chosen.
    Serial.println(select);
    Serial.print("Its octet ");
    Serial.print(select);
    Serial.print(" is: ");
    return atoi(select);
  }

  int8_t get_octet(uint32_t ip_address,uint8_t octet){
    int32_t separate[32];
    uint8_t result;
    for (int i=31; i>=0; --i) {
      //put the ip address in to a 32 bits array
      separate[i] = ((ip_address>> i) & 1 );
    }
    int p;
    for (p=0;(p<=7);p=p+1){
      result = result+separate[octet*8+p]*pow(2,p);
      /*here is where error happens.
      I tried printed out [octet*8+p], and it is correct
      and I think the calcualtion is correct, so the problem should
      relate to the integer type, but I could not find it.*/
    }
    Serial.println(result);
    return (result);
  }

  // this function is to verify whether the customer selected a or b.
  int chose (){
    while (true){
      while (Serial.available()==0){}
      //waiting for the input
      int input = Serial.read();
      //if chose a
      if(input==(97)){
        Serial.println("a");
        Serial.print("Please enter an unsigned long: ");
        long x = get_number();
        Serial.print("The number of bits required is:");
        num_bits_unsigned (x);
      }
      // if chose b
      if (input == (98)){
        Serial.println("b");
        Serial.print("Please enter an uint32_t: ");
        uint32_t ip_address = get_number();
        Serial.print("Please enter an octet (0,1,2,or3): ");
        uint8_t octet = get_noctet();
        get_octet (ip_address,octet);
      }
    }
  }
  //recall all the functions and let them work togather
  int main(){
    init();
    Serial.begin(9600);
    Serial.print ("Which part to test (a or b)?");
    chose();
    Serial.end();
    return 0;
  }
