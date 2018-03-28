/*Name: QIUFENG DU
ID no.: 1439484
Section: EA2*/


#include <Arduino.h>
// count the length of the string
// use i to count the while loop's running times
int getStringLength(char *str){
  int i = 0;
  while (true){
    // use if statement to check where the
    //termial 0 is
    if(str[i] != 0){
      ++i;
    }
    else{
      break;
    }
  }
  return(i);
}

//Print the string backward
void printBackward(char *str){
  Serial.print("The string is: ");
  int number;// integer number is used to store the location
  //of the last checked character
  //use for loop to print the characters backward
  for(number = getStringLength(str);number>=0;number--){
    Serial.print(str[number]);
  }
}

int main(){
  init();
  Serial.begin(9600);
  // build a random string
  char str1[] = "Hello World!";
  //use character pointer to locate the string
  char* str;
  str = str1;
  Serial.print("The string number is: ");
  // use string pointer as parameters
  Serial.println(getStringLength(str));
  printBackward(str);
  Serial.end();
}
