#include <QApplication>
#include <QUrl>
#include <QDateTime>
#include <QFileInfo>
#include "mega_http.h"

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
    xilinx.getHttp();
  }
  catch(int e) {
    qDebug()<<"Some error in creating the object";
  }
  delete app;
  return 0;
}

