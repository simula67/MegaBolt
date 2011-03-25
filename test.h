#include <QFile>
#include <QDateTime>
#include <QString>
#include <QUrl>
#include <QNetworkProxy>
#include <QFileInfo>
#include <QThread>
#define EOPEN -1
#define UNKNOWN -2
#define BUFFSIZE 51200 // 500KB Buffer Size




enum ThreadSignal {INIT,DOWNLOAD,SUSPEND,DONE};
class HttpDownThread : public QThread {
  Q_OBJECT
    
  private slots:
  void thread_init();
  void start_download();
  void suspend_download();
 public:
  void run();
  int ready;
  int bytes_recieved;
  ThreadSignal nextJob;
  char buffer[BUFFSIZE];
  QUrl down_url;
  int range_start;
  int range_end;
  HttpDownThread() {
    ready = 0;
    nextJob = SUSPEND;
    bytes_received = 0;
};
class ThreadStatus {
public:
  int abs_start;
  int abs_end;
  int abs_pos;
};
class Download : public QThread{
  Q_OBJECT
private:
  QNetworkProxy *mega_proxy;
public:
  QUrl dl_url;
  int id;
  QFile *dest_file;
  QDateTime start_time;
  QDateTime end_time;
  QString type;
  int size;
  int num_threads;
  int resumable;
  int status;
  QString uname;
  QString passwd;
  int bytes_download;
  QFile *thread_status;

  ThreadStatus *threads;
  QFile *timing_info;
  //  QFileInfo target_address;
  Download(QUrl down_url,int down_id,QFileInfo down_file,QDateTime down_start,QDateTime down_end,QString down_type,int down_threads,QUrl down_proxy,QString down_uname, QString down_passwd,QFileInfo down_threadfile,QFileInfo down_timing);
 void Output();
 void getHttp();
 signals:
 void start_download();
 void thread_init();
 void suspend_download();
}; 
