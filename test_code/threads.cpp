#include <unistd.h>
#include "threads.h"
void waiter_threads::download() {
  while(!stop) {
    qDebug() << "Downloading...";
    QThread::msleep(500);
  }
  qDebug() << "Oops... Interrupted";
  stop = False;
}
void waiter_threads::run()
{
  while(True) {
    download();
  }
}
#define NUM_THREADS 5
int main()
{
  Caller my_caller;
  waiter_threads **some_threads= new waiter_threads *[NUM_THREADS];
  for(int i=0;i<NUM_THREADS;i++) {
    some_threads[i] = new waiter_threads(&my_caller);
    some_threads[i]->start();
  }
  sleep(5);
  my_caller.interrupt_thread();
  sleep(5);
  return 0;
}
