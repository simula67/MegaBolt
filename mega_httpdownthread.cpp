#include "mega_httpdownthread.h"

void HttpDownThread::run() {
  qDebug() <<  "This  thread will download from "<<range_start<<" to "<<range_end;
  QString *response_string = new QString("");
  QString request_string = "GET "; 
  QAbstractSocket *get_request = new QAbstractSocket(QAbstractSocket::TcpSocket,0);
  while( (nextJob != DONE) && (nextJob != PAUSE) ) {
    switch(nextJob) {
    case INIT:
      /* Create Socket, send request, receive and throw away headers */
      suspended = 0;
      
      get_request->setProxy((*mega_proxy));
      
      if( down_url.path() == "" ) 
	request_string.append("/");
      else
	request_string.append(down_url.path());
      request_string.append(" HTTP/1.1\r\nHost: ");
      request_string.append(down_url.host());
      request_string.append("\r\n");
      request_string.append("Range: bytes=");
      request_string.append(QString::number(range_start,10));
      request_string.append("-");
      request_string.append(QString::number(range_end,10));
      
      request_string.append("\r\n\r\n");
      qDebug() << "Request string : " << request_string;
      if( down_url.port() < 0 )
	down_url.setPort(80);
      
      while(1) {
	get_request->close();
	get_request->connectToHost(down_url.host(), (quint16) down_url.port());
	if (get_request->waitForConnected(18000)) {
	  qDebug("Connected!");    
	  break;
	}
      }
      /* Send out request and wait */
      if(get_request->state() == QAbstractSocket::ConnectedState) {
	get_request->write(request_string.toAscii());
	get_request->flush();
	if(get_request->waitForBytesWritten(-1)) {
	  qDebug() << "All bytes written";
	}
	else {
	  qDebug() << "Some bytes were not written. Error : ";
	  qDebug() << get_request->errorString();
	  qDebug() << "Working around QT bug with just sleeping QT_BUG_SLEEP seconds";
	  QThread::sleep(QT_BUG_SLEEP);
	}
	/* Throw away the headers */
	response_string->resize(0);
	while( 1 ) {
	  if( get_request->readLine(buffer,LINE_LEN) == 0 )
	    break;
	  response_string->append(buffer);
	  qDebug() << "Discarding line : " << (*response_string);
	  if( (*response_string) == "\r\n" ) {
	    
	    break;
	 }
	response_string->resize(0);
	}
	nextJob = SUSPEND;
        ready = 1;
      }
      break;
    case DOWNLOAD:
      suspended = 0;
      if( bytes_received > BUFFSIZE - DOWNLOAD_LEN ) {
	nextJob = SUSPEND;
	break;
      }
      /* Download the stuff onto the buffer. use "bytes_received". */
      int received;
      received = get_request->read(buffer + bytes_received,DOWNLOAD_LEN);
      if( received > 0 )
	bytes_received += received;
      break;
    case SUSPEND:
      suspended = 1;
      break;
    case DONE:
      
      break;
    case PAUSE:
    default:
      qDebug() << "ThreadSignal could not be deciphered";
    }
  }
  suspended = 0;
  get_request->close();
  delete get_request;
  delete response_string;
  paused = 1;
}
void HttpDownThread::start_download()
{
  nextJob = DOWNLOAD;
}
void HttpDownThread::suspend_download()
{
  nextJob = SUSPEND;
}
