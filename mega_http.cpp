/*
 *
 *  Copyright (C) 2011, Joji Antony
 *  This file is part of MegaBolt.
 *   MegaBolt is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   MegaBolt is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with MegaBolt.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "mega_http.h"

HttpDownload::HttpDownload(QUrl dl_url,int down_id,QFileInfo down_file,QDateTime down_start,QDateTime down_end,QString down_type,int down_threads,QUrl down_proxy,QString down_uname, QString down_passwd,QFileInfo down_threadfile,QFileInfo down_timing,int size) {
  QUrl no_value("");
  
  this->dl_url = dl_url;
  
  dest_file = new QFile(down_file.absoluteFilePath());
  if( dest_file == NULL ) {
  	throw NOMEM;
  }
  if(!dest_file->open(QIODevice::ReadWrite|QIODevice::Unbuffered)) {   
    
    throw EOPEN;
  }
  thread_status = new QFile(down_threadfile.absoluteFilePath());
  if( thread_status == NULL ) {
    throw NOMEM;
  }
  if(!thread_status->open(QIODevice::ReadWrite)) {   
    throw EOPEN;
  }
  timing_info = new QFile(down_timing.absoluteFilePath());
  if( timing_info == NULL ) {
    throw NOMEM;
  }
  if(!timing_info->open(QIODevice::ReadWrite)) {   
    throw EOPEN;
  }
  
  
 
  if( down_proxy != no_value ) {
    /* Add code for other type of proxies */
    mega_proxy = new QNetworkProxy(QNetworkProxy::HttpCachingProxy,down_proxy.host(),down_proxy.port(),uname,passwd);
    if( mega_proxy == NULL ) {
  	throw NOMEM;
    }
  }
  else {
    mega_proxy = new QNetworkProxy();
    if( mega_proxy == NULL ) {
      throw NOMEM;
    }
    mega_proxy->setType(QNetworkProxy::NoProxy);
  }
  if( size == 0 ) {
    QAbstractSocket *head_request = new QAbstractSocket(QAbstractSocket::TcpSocket,this);
    if( head_request == NULL ) {
      throw NOMEM;
    }
    head_response = NULL;
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
	if( dl_url.port() < 0 )
	  dl_url.setPort(80);
	head_request->connectToHost(dl_url.host(), (quint16) dl_url.port());
	if (head_request->waitForConnected(18000)) {
	qDebug("Connected!");
	}
	char buff[LINE_LEN];
	
	if( (head_request->state() == QAbstractSocket::ConnectedState) && (head_request->isValid()) ) {
	  head_request->write(request_string.toAscii(),request_string.size());
	  head_request->flush();
	  if(head_request->waitForBytesWritten(-1)) {
	    qDebug() << "All bytes written";
	  }
	  else {
	    qDebug() << "Working around QT bug with just sleeping QT_BUG_SLEEP seconds";
	    QThread::sleep(QT_BUG_SLEEP);
	  }
	  response_string.resize(0);
	  while( head_request->readLine(buff,LINE_LEN) > 0 ) {
	    response_string.append(buff);
	  }
	  head_response = new QHttpResponseHeader(response_string);
	  if(head_response == NULL) {
	    throw NOMEM;
	  }
	  head_status = head_response->statusCode();
	  if( (head_status != 302) && (head_status != 200) ) {
	    delete head_response;
	    head_response = NULL;
	  }
	}
      }while( (head_status != 302) && (head_status != 200) );
      
      
      if(head_response) {
	if( (head_response->value("Location") == "") || ( head_response->value("Location") == dl_url.toString() ) ) {
	  follow = 0;
	  qDebug() << "Size is : "<<head_response->value("Content-Length");
          size = (head_response->value("Content-Length")).toInt();
          if(size == 0) {
             qDebug() << "Size not found. Cant download without knowing file size";
             throw NOFILESIZE;
           }
          dest_file->resize(size);
	}
	else {
	  qDebug() << "Following here : " << head_response->value("Location");
	  dl_url.setUrl(head_response->value("Location"));  
	}
	delete head_response;
	head_response = NULL;
      }
    }
  }
  else {
    this->size = size;
  }
  
  threads = new ThreadStatus[num_threads];
  if( threads == NULL ) {
    throw NOMEM;
  }
  int i;
  for(i=0;i<num_threads;i++) {
    threads[i].abs_start = threads[i].abs_pos = ( i * (size/num_threads) );
    threads[i].abs_end =  (i+1) * (size/num_threads);
  }
  threads[i-1].abs_end = size;
  
  thread_status->write((char *)threads,num_threads * sizeof(ThreadStatus));
  thread_status->flush();
  
  bytes_download = 0;
  resumable = 0;
  mainThread = NULL; 
  status = PAUSED;
  start_time=down_start;
  end_time = down_end;
  type = down_type;
  num_threads = down_threads;
  uname = down_uname;
  passwd = down_passwd;
  id=down_id;
}
/*
HttpDownload::~HttpDownload()
{
  qDebug() << "In destructor";	
  if(dest_file) {
    dest_file->close();
    delete dest_file;
  }
  if(thread_status) {
    dest_file->close();
    delete dest_file;
  }
  if(timing_info) {
    dest_file->close();
    delete dest_file;
  }
if(mega_proxy) {
  delete mega_proxy;
 }
 if( threads == NULL ) {
   delete threads;
 }
}
*/
void HttpDownload::getHttp()
{
  if( bytes_download != size ) {
  	mainThread = new HttpMainThread(&dl_url,dest_file,&size,&num_threads,&bytes_download,thread_status,threads,mega_proxy,&status);
  	if(mainThread == NULL) {
  		qDebug() << "Thread could not be allocated";
  	}
  	mainThread->start();
  }
}    
void HttpDownload::pause(void) {
  if(mainThread) {
  	mainThread->paused = 1;
  	while( mainThread->pause_complete == 0 ) {
  	  QThread::msleep(1);
  	}
  	delete mainThread;
  	mainThread = NULL;
  }
  else {
  	qDebug() << "Called pause when no download going on";
  }
}
