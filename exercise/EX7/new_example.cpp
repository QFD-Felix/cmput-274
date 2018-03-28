#include <iostream>
#include <stdlib.h>
using namespace std;
/*in this function
a -------- input string
n -------- length of the input string
b -------- convert the input strinng to an character array
val_a ------ integer value of string
c --------- new array
*/
int main(){
  //input the string
  string a;
  cin>>a;
  int n = a.length();
  int check = true;
  // when it is not pure digit, return false
  for (int i = 0;i<n; i++){
    if (!(isdigit(a[i]))){
      check = false;
    }
  }
  // convert to an character array
  char b[n];
  for (int j = 0; j<n; j++){
    b[j] = a[j];
  }

  // use atoi to convert the character array into an integer
  int val_a = atoi(b);

  // if the string is 0; return false
  if (val_a == 0){
    check = false;
  }

  // output the result
  if (check){
    cout<<"Test Pass"<<endl;
  }
  else {
    cout<<"Test fail"<<endl;
    return 0;
  }

  // creat an new array. the size of the array equal to the input string
  // assign an pointer to that array
  int *c = new int [val_a];
  for (int g = 0; g<val_a; g++){
    c[g] = g+1; // since g = 0 initially, so we need to plus one every time
    cout<<c[g]<<endl;
  }
  //delet the memories of the new arrays
  delete[] c;
  return 0;
}
