#ifndef MEGA_COMMON_H
#define MEGA_COMMON_H

enum DowloadStatus {DOWNLOADING,PAUSED,FIN};

class ThreadStatus {
public:
  int abs_start;
  int abs_end;
  int abs_pos;
  ThreadStatus(){
    abs_start=abs_end=abs_pos=0;
  }
};
#endif
