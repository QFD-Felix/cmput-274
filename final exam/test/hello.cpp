#include <iostream>
#include <cstdint>

using namespace std;

struct Square{int x; int y; int side;};

int main(){
Square s1 = {0,0,2};
Square *s2 = &s1;
s2->side = 2;
cout << "The square s2 starts at position<";
cout <<s2->x<<", "<<s2->y;
cout << "> and has an area of " << (s2->side)*(s2->side)<<endl;
}
