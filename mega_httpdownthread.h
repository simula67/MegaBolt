#ifndef MEGA_HTTPDOWNTHREAD_H
#define MEGA_HTTPDOWNTHREAD_H
#include <QThread>
#include <QUrl>
#include <QNetworkProxy>

#define LINE_LEN 1024
#define DOWNLOAD_LEN 50
#define BUFFSIZE 51200 // 500KB Buffer Size

#define QT_BUG_SLEEP 2

enum ThreadSignal {INIT,DOWNLOAD,SUSPEND,DONE,PAUSE};

class HttpDownThread : public QThread {
  Q_OBJECT
    
  private slots:
  void start_download();
  void suspend_download();
 public:
  void run();
  int ready,suspended,paused;
  int bytes_received;
  ThreadSignal nextJob;
  char buffer[BUFFSIZE];
  QUrl down_url;
  int range_start;
  int range_end;
  HttpDownThread() {
    ready = 0;
    nextJob = SUSPEND;
    bytes_received = 0;
    paused = 0;
  }
  QNetworkProxy *mega_proxy;
};
#endif
