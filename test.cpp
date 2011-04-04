#include <QApplication>
#include <QUrl>
#include <QDateTime>
#include <QFileInfo>
#include "mega_http.h"
#include <QThread>
#include <unistd.h>

class myThread : public QThread {
private:  
  HttpDownload *down_obj;
public:
  void run();
  myThread(HttpDownload *inp_down_obj) {down_obj = inp_down_obj; done = 0;}
  int done;
};
void myThread::run()
{
    down_obj->getHttp();
    //qDebug() << "Setting done bit NOW";
    done = 1;
}
int main(int argc,char *argv[])
{
  QApplication *app=new QApplication(argc,argv);
  QUrl down_url("http://localhost/a.abc");
  QFileInfo down_file("/home/simula67/Repos/MegaBolt/down_file");
  QDateTime down_start = QDateTime::currentDateTime();
  QDateTime down_end = QDateTime::currentDateTime();
  QString down_type = "http";
  QFileInfo down_threads("/home/simula67/Repos/MegaBolt/thread_file");
  QFileInfo down_timing("/home/simula67/Repos/MegaBolt/timing_file");
  QUrl down_proxy("");
  try { 
    HttpDownload xilinx(down_url,1,down_file,down_start,down_end,down_type,2,down_proxy,"","",down_threads,down_timing);
    myThread *download_thread = new myThread(&xilinx);
    download_thread->start();
    sleep(5);
    xilinx.pause();
    /*
    while(download_thread->done != 1) {
      qDebug() << "Waiting for thread to get over";
    }
    
    delete download_thread;
    */
    sleep(5);
    xilinx.getHttp();
  }
  catch(int e) {
    qDebug()<<"Some error in creating the object";
  }
  delete app;
  return 0;
}

