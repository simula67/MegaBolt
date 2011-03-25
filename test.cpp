#include "test.h"
#include <QDebug>
#include <cstring>
#include <QTcpSocket>
#include <QHttpResponseHeader>
#include <QApplication>


#define QT_BUG_SLEEP 2

void Download::Output() {
  qDebug()<< "Final Download URL is : ";
  qDebug()<<  dl_url.toString();
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
    fprintf(stderr,"ThreadFile\n");
    throw EOPEN;
  }
  timing_info = new QFile(down_timing.absoluteFilePath());  
  if(!timing_info->open(QIODevice::ReadWrite)) {   
    fprintf(stderr,"TimingFile\n");
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

  QAbstractSocket *head_request = new QAbstractSocket(QAbstractSocket::TcpSocket,this);
  QHttpResponseHeader *head_response = NULL;
  head_request->setProxy((*mega_proxy));
  QString response_string;  
  int head_status = 0;
  int follow = 1;
  while(follow) {
    do {
      QString request_string = "HEAD ";
      if( dl_url.path() == "" ) 
	request_string.append("/");
      else
	request_string.append(dl_url.path());
      request_string.append(" HTTP/1.1\r\nHost: ");
      request_string.append(dl_url.host());
      request_string.append("\r\n\r\n");
      head_request->close();
      //      qDebug() << request_string;
      if( dl_url.port() < 0 )
	dl_url.setPort(80);
      head_request->connectToHost(dl_url.host(), (quint16) dl_url.port());
      if (head_request->waitForConnected(18000)) {
	qDebug("Connected!");    
	;
      }
      char buff[1024];

      if(head_request->state() == QAbstractSocket::ConnectedState) {
	head_request->write(request_string.toAscii());
	//head_request->flush();
	if(head_request->waitForBytesWritten(-1)) {
	  qDebug() << "All bytes written";
	}
	else {
	  qDebug() << "Some bytes were not written. Error : ";
	  qDebug() << head_request->errorString();
	  qDebug() << "Working around QT bug with just sleeping QT_BUG_SLEEP seconds";
	  QThread::sleep(QT_BUG_SLEEP);
	}
	response_string.resize(0);
	while( head_request->readLine(buff,sizeof(buff)) ) {
	  response_string.append(buff);
	}
	head_response = new QHttpResponseHeader(response_string);
	//qDebug() << response_string;
	head_status = head_response->statusCode();
	if( (head_status != 302) && (head_status != 200) ) {
	  delete head_response;
	  head_response = NULL;
	}
      }
    }while( (head_status != 302) && (head_status != 200) );
    //qDebug() << head_response->toString();
    if(head_response) {
      //qDebug() << head_response->value("Location");
      if( (head_response->value("Location") == "") || ( head_response->value("Location") == dl_url.toString() ) ) {
	qDebug() << "Stop following";
	follow = 0;
      }
      else {
	qDebug() << "Following here : ";
	qDebug() << head_response->value("Location");
	dl_url.setUrl(head_response->value("Location"));
	delete head_response;
	head_response = NULL;
      }
    }
  }
  if(!head_response) {
    qDebug() << "Head Response is NULL?!?!?";
    throw UNKNOWN;
  }
  qDebug() << "Size is : ";
  qDebug() <<head_response->value("Content-Length");
  size = (head_response->value("Content-Length")).toInt();
  if(size == 0) {
    qDebug() << "Size not found. Reverting to one thread";
    num_threads = 1;
  }
  delete head_response;
  head_response = NULL;
  /*
  thread_status = new QFile(down_threadfile.absoluteFilePath());
  if(!thread_status->open(QIODevice::ReadWrite)) {   
    fprintf(stderr,"ThreadFile\n");
    throw EOPEN;
  }
  */
  threads = new ThreadStatus[num_threads];
  memset(threads,0,num_threads*sizeof(ThreadStatus));
  int i;
  for(i=0;i<num_threads;i++) {
    threads[i].abs_start = threads[i].abs_pos = ( i * (size/num_threads) );
    threads[i].abs_end =  (i+1) * (size/num_threads);
  }
  threads[i-1].abs_end = size;
  bytes_download = 0;
  resumable = 0;
  thread_status->write((char *)threads,num_threads * sizeof(ThreadStatus));
  thread_status->flush();
  sleep(5);
}
void HttpDownThread::run() {
  while(nextJob != DONE) {
    switch(nextJob) {
    case INIT:// Create Socket, send request and receive and throw away headers
      ready = 1;
      nextjob = SUSPEND;
      break;
    case DOWNLOAD:// Download the stuff onto the buffer. use "bytes_downloaded". 
      break;
    case SUSPEND:
      break;
    case IDLE:
      ready = 0;
    case DONE:
      break;
    default:
      qDebug() << "ThreadSignal could not be deciphered";
    }
  }
}
void Download::getHttp()
{
  /* Create the threads, initialize the URL and ranges by hand.
     Now, set the nextJob to INIT. Once all threads have ready == 1 emit start_download()
     Every 300ms, emit suspend_download() and  
     save all the buffers to disk. If any thread downloaded past its abs_end, discard the extra data. Make bytes_downloaded of each thread 0
     If any thread finished downloading, find longest undivided block and re-arrage the ranges
     

     Update the thread abs_pos and write out thread_file
  */

}
/*
Test
*/
int main(int argc,char *argv[])
{
  QApplication *app=new QApplication(argc,argv);
  QUrl down_url("http://localhost/sim/poll/ajax-poller.html");
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
    xilinx.Output();
  }
  catch(int e) {
    qDebug()<<"Could not open file";
  }
  delete app;
  return 0;
}
