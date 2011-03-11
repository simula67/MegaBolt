#include "test.h"
#include <QDebug>
#include <cstring>
#include <QTcpSocket>
#include <QHttpResponseHeader>

#include <QApplication>

void Download::Output() {
  QString URL = dl_url.toString();
  qDebug()<<  URL.toAscii();
  
}
Download::Download(QUrl down_url,int down_id,QFileInfo down_file,QDateTime down_start,QDateTime down_end,QString down_type,int down_threads,QUrl down_proxy,QString down_uname, QString down_passwd,QFileInfo down_threadfile,QFileInfo down_timing) {
  //int i;
  QUrl no_value("");
  dl_url = down_url;
  id=down_id;
  dest_file = new QFile(down_file.absoluteFilePath());
  if(!dest_file->open(QIODevice::ReadWrite|QIODevice::Unbuffered)) {   
    fprintf(stderr,"DestFile\n");
    throw EOPEN;
  }
  thread_status = new QFile(down_threadfile.absoluteFilePath());
  if(!thread_status->open(QIODevice::ReadWrite)) {   
    fprintf(stderr,"DestFile\n");
    throw EOPEN;
  }
  timing_info = new QFile(down_timing.absoluteFilePath());  
  if(!timing_info->open(QIODevice::ReadWrite)) {   
    fprintf(stderr,"DestFile\n");
    throw EOPEN;
  }
  start_time=down_start;
  end_time = down_end;
  type = down_type;
  num_threads = down_threads;
  if( down_proxy != no_value ) {
    //Add code for other type of proxies
    mega_proxy = new QNetworkProxy(QNetworkProxy::HttpCachingProxy,down_proxy.host(),down_proxy.port(),uname,passwd);
  }
  else {
    mega_proxy = new QNetworkProxy();
    mega_proxy->setType(QNetworkProxy::NoProxy);
  }
  uname = down_uname;
  passwd = down_passwd;

  threads = new ThreadStatus[num_threads];
  memset(threads,0,num_threads*sizeof(ThreadStatus));
  
  QString request_string = "HEAD ";
  if( down_url.path() == "" ) 
    request_string.append("/");
  else
    request_string.append(down_url.path());
  request_string.append(" HTTP/1.1\r\nHost: ");
  request_string.append(down_url.host());
  request_string.append("\r\n\r\n");
  qDebug() << request_string;
  QAbstractSocket *head_request = new QAbstractSocket(QAbstractSocket::TcpSocket,this);
  head_request->setProxy((*mega_proxy));  
  head_request->connectToHost(down_url.host(), (quint16) 80);
  if (head_request->waitForConnected(9000))
    qDebug("Connected!");    
  QString htt_res_str;
  char buff[1024];
  if(head_request->state() == QAbstractSocket::ConnectedState) {
    head_request->write(request_string.toAscii());
    head_request->flush();
    while( head_request->readLine(buff,sizeof(buff)) ) {
      qDebug() << buff;
  }
    
  }
}
/*
Test
*/
int main(int argc,char *argv[])
{
  QApplication *app=new QApplication(argc,argv);
  QUrl down_url("http://localhost/");
  QFileInfo down_file("/home/simula67/Repos/MegaBolt/down_file");
  //  QFileInfo down_file("/etc/shadow");
  QDateTime down_start = QDateTime::currentDateTime();
  QDateTime down_end = QDateTime::currentDateTime();
  QString down_type = "http";
  QFileInfo down_threads("/home/simula67/Repos/MegaBolt/thread_file");
  QFileInfo down_timing("/home/simula67/Repos/MegaBolt/timing_file");
  QUrl down_proxy("");
  try { 
    Download xilinx(down_url,1,down_file,down_start,down_end,down_type,5,down_proxy,"","",down_threads,down_timing);
    //xilinx.Output();
  }
  catch(int e) {
    qDebug()<<"Could not open file";
  }
  delete app;
  return 0;
}
