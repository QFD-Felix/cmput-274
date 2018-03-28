#ifndef QUEUE_H
#define QUEUE_H
#include <cassert>

struct QueueItem {
  QueueItem *next;
  int v;

  QueueItem(int v) {
    this->v = v;
    this->next = 0;
  }
};

struct Queue {
  QueueItem *tail;

  Queue() {
    tail = 0;
  }

  ~Queue() {
    while (!isEmpty())
      pop();
  }

  bool isEmpty() {
    return tail == 0;
  }

  int front() {
    assert(!isEmpty());
    return tail->next->v;
  }

  void push(int item) {
    QueueItem *tmp = new QueueItem(item);
    assert(tmp != 0);
    if (isEmpty()) {
      tmp->next = tmp;
    }
    else {
      tmp->next = tail->next;
      tail->next = tmp;
    }
    tail = tmp;
  }

  void pop() {
    assert(!isEmpty());
    QueueItem *tmp = tail->next;
    tail->next = tmp->next;
    delete tmp;
    if (tail == tmp) {
      tail = 0;
    }
  }
};

#endif
