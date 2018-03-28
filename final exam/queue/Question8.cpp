#include <iostream>
#include <cassert>
#include "queue.h"

using namespace std;

int countValue(Queue* qptr, int v){
  int count = 0;
  if (qptr->tail == 0){
    return 0;
  }
  QueueItem *tmp = qptr->tail;
  while (true){
    if (tmp->v == v){
    count++;
    tmp = tmp->next;
  if ((tmp = (qptr->tail))){
    break;
  }
}
  }
  return count;
}
void testCountValue(){

    Queue q;

    q.push(5);
    q.push(20);
    q.push(30);
    q.push(30);
    q.push(5);
    q.push(40);
    q.push(10);
    q.push(5);
    q.push(30);
    q.push(5);
    q.push(5);
    q.pop();
    int counter=countValue(&q,20);
    int counter1=countValue(&q,5);
    int counter2=countValue(&q,30);
    int counter3=countValue(&q,0);

    cout<<counter<<" "<<counter1<<" "<<counter2<<" "<<counter3<<endl;
}
int main(){
    testCountValue();
    return 0;
}
