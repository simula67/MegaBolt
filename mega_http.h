#ifndef MEGA_HTTP_H
#define MEGA_HTTP_H

#include <QFile>
#include <QDateTime>
#include <QString>
#include <QUrl>
#include <QNetworkProxy>
#include <QFileInfo>
#include <QThread>

#define EOPEN -1
#define UNKNOWN -2
#define NOFILESIZE -3

#ifndef LINE_LEN
#define LINE_LEN 1024
#endif

#ifndef QT_BUG_SLEEP
#define QT_BUG_SLEEP 2
#endif


class ThreadStatus {
public:
  int abs_start;
  int abs_end;
  int abs_pos;
  ThreadStatus(){
    abs_start=abs_end=abs_pos=0;

}
};
class HttpDownload : public QThread{
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
  HttpDownload(QUrl down_url,int down_id,QFileInfo down_file,QDateTime down_start,QDateTime down_end,QString down_type,int down_threads,QUrl down_proxy,QString down_uname, QString down_passwd,QFileInfo down_threadfile,QFileInfo down_timing);
 void getHttp();
 signals:
 void start_download();
 void suspend_download();
};
#endif
