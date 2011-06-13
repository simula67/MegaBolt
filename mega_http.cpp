
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

HttpDownload::HttpDownload(QUrl down_url,int id,QFileInfo dest_file,
			   QDateTime start_time,QDateTime end_time,
			   QString type,int num_threads,QUrl mega_proxy,
			   QString uname, QString passwd, 
			   QFileInfo thread_status,QFileInfo timing_info,
			   int size) {
  QUrl no_value("");
  this->dest_file = NULL;
  this->thread_status = NULL;
  this->timing_info = NULL;
  this->threads = NULL;
  this->mega_proxy = NULL;
  
  
  this->down_url = down_url;
  this->num_threads = num_threads;
  
  this->dest_file = new QFile(dest_file.absoluteFilePath());
  if( this->dest_file == NULL ) {
  	throw NOMEM;
  }
  if(!this->dest_file->open(QIODevice::ReadWrite|QIODevice::Unbuffered)) {  
    throw EOPEN;
  }
  
  this->thread_status = new QFile(thread_status.absoluteFilePath());
  if( this->thread_status == NULL ) {
    throw NOMEM;
  }
  if(!this->thread_status->open(QIODevice::ReadWrite)) {   
    throw EOPEN;
  }
  this->timing_info = new QFile(timing_info.absoluteFilePath());
  if( this->timing_info == NULL ) {
    throw NOMEM;
  }
  if(!this->timing_info->open(QIODevice::ReadWrite)) {   
    throw EOPEN;
  }
  
  
 
  if( mega_proxy != no_value ) {
    /* Add code for other type of proxies */
    this->mega_proxy = new QNetworkProxy(QNetworkProxy::HttpCachingProxy,
					 mega_proxy.host(),mega_proxy.port(),
					 uname,passwd);
    if( this->mega_proxy == NULL ) {
  	throw NOMEM;
    }
  }
  else {
    this->mega_proxy = new QNetworkProxy();
    if( this->mega_proxy == NULL ) {
      throw NOMEM;
    }
    this->mega_proxy->setType(QNetworkProxy::NoProxy);
  }
  
  if( size == 0 ) {
    QAbstractSocket *head_request = 
      new QAbstractSocket(QAbstractSocket::TcpSocket,this);
    if( head_request == NULL ) {
      throw NOMEM;
    }
    head_response = NULL;
    head_request->setProxy(( *(this->mega_proxy) ));
    QString response_string;  
    int head_status = 0;
    int follow = 1;
    while(follow) {
      do {
	QString request_string = "HEAD ";
	if( this->down_url.encodedPath() == "" ) 
	  request_string.append("/");
	else
	  request_string.append(this->down_url.encodedPath());
	request_string.append(" HTTP/1.1\r\nHost: ");
	request_string.append(down_url.host());
	request_string.append("\r\n\r\n");
	head_request->close();
	if( this->down_url.port() < 0 )
	  this->down_url.setPort(80);
	head_request->connectToHost(this->down_url.host(),
				    (quint16) this->down_url.port());
	if (head_request->waitForConnected(18000)) {
	   qDebug("Connected!");
	}
	else {
	   qDebug() << "Could not connect";
	   throw ECONNECT;
	}
	char buff[LINE_LEN];
	if( (head_request->state() == QAbstractSocket::ConnectedState) && 
	    (head_request->isValid()) ) {
	  head_request->write(request_string.toAscii(),request_string.size());
	  head_request->flush();
	  if(head_request->waitForBytesWritten(-1)) {
	    qDebug() << "All bytes written";
	  }
	  else {
	    qDebug() << "Waiting...";
	    QThread::sleep(REQUEST_PASS_SLEEP);
	  }
	  response_string.resize(0);
	  while( head_request->readLine(buff,LINE_LEN) > 0 ) {
	    response_string.append(buff);
	  }
	  head_response = new QHttpResponseHeader(response_string);
	  if(head_response == NULL) {
	    delete head_request;
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
	if( (head_response->value("Location") == "") || 
	    (head_response->value("Location") == this->down_url.toString()) ) {
	  follow = 0;
	  qDebug() << "Size is : "<<head_response->value("Content-Length");
          this->size = (head_response->value("Content-Length")).toInt();
          if(this->size == 0) {
	    qDebug() << "Size not found.";
	    qDebug() << "Cant download without knowing file size";
	    throw NOFILESIZE;
           }
          this->dest_file->resize(this->size);
	}
	else {
	  qDebug() << "Following here : " << head_response->value("Location");
	  this->down_url.setUrl(head_response->value("Location"));  
	}
	delete head_response;
	head_response = NULL;
      }
    }
    delete head_request;
  }
  else {
    this->size = size;
  }
  /*
  
  TODO: Check  if the thread_status file is populated
  If yes, load the threads array from there.
  
  */
  this->threads = new ThreadStatus[this->num_threads];
  if( this->threads == NULL ) {
    throw NOMEM;
  }
  int i;
  for(i=0;i<num_threads;i++) {
    this->threads[i].abs_start = ( i * (this->size/num_threads) ); 
    this->threads[i].abs_pos = ( i * (this->size/num_threads) ); 
    this->threads[i].abs_end =  (i+1) * (this->size/num_threads);
  }
  this->threads[i-1].abs_end = this->size;
  this->thread_status->write((char *)threads,
			     num_threads * sizeof(ThreadStatus));
  this->thread_status->flush();  
  
  this->bytes_download = 0;
  this->resumable = 0;
  this->mainThread = NULL; 
  this->status = DOWN_PAUSED;
  this->start_time= start_time;
  this->end_time = end_time;
  this->type = type;
  this->uname = uname;
  this->passwd = passwd;
  this->id = id;
}
HttpDownload::~HttpDownload()
{
  if(dest_file) {
    dest_file->close();
    delete dest_file;
  }
  if(thread_status) {
    thread_status->close();
    delete thread_status;
  }
  if(timing_info) {
    timing_info->close();
    delete timing_info;
  }
if(mega_proxy) {
  delete mega_proxy;
 }
 if( threads == NULL ) {
   delete threads;
 }
}
void HttpDownload::getHttp()
{
  if( bytes_download != size ) {
  	mainThread = new HttpMainThread(&down_url,dest_file,&size,
					&num_threads,&bytes_download,
					thread_status,threads,mega_proxy,
					&status);
  	if(mainThread == NULL) {
  		qDebug() << "Thread could not be allocated";
  	}
  	this->status = DOWN_ONGOING;
  	mainThread->start();
  }
}    
void HttpDownload::pause(void) {
  if(mainThread) {
  	mainThread->paused = 1;
  	while( !mainThread->isFinished() ) {
  	  QThread::msleep(1);
  	}
  	delete mainThread;
  	mainThread = NULL;
  }
  else {
  	qDebug() << "Called pause when no download going on";
  }
}
