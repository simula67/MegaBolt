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
#include "mega_httpdownthread.h"

void HttpDownThread::run() {
  response_string = new QString("");
  if( response_string == NULL ) {
  	qDebug() << "Out of memory";
  }
  QString request_string = "GET "; 
  get_request = new QAbstractSocket(QAbstractSocket::TcpSocket,0);
  if( get_request == NULL ) {
  	qDebug() << "Could not open socket";
  }
  while( (nextJob != DONE) && (nextJob != PAUSE) ) {
    switch(nextJob) {
    case INIT:
      /* Create Socket, send request, receive and throw away headers */
      
      suspended = 0;
      ready=0;
      paused=0;
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
	  //qDebug() << "Some bytes were not written. Error : ";
	  //qDebug() << get_request->errorString();
	  qDebug() << "Waiting...";
	  QThread::sleep(REQUEST_PASS_SLEEP);
	}
	/* Throw away the headers */
	response_string->resize(0);
	while( 1 ) {
	  if( get_request->readLine(buffer,LINE_LEN) <= 0 )
	    break;
	  response_string->append(buffer);
	  //qDebug() << "Discarding line : " << (*response_string);
	  if( (*response_string) == "\r\n" ) {  
	    break;
	 }
	response_string->resize(0);
	}
	delete response_string;
	nextJob = SUSPEND;
        ready = 1;
      }
      break;
    case DOWNLOAD:
      suspended = 0;
      paused=0;
      ready=1;
      if( bytes_received > BUFFSIZE - DOWNLOAD_LEN ) {
	nextJob = SUSPEND;
	break;
      }
      /* Download the stuff onto the buffer. use "bytes_received". */
      int received;
      received = get_request->read(buffer + bytes_received,DOWNLOAD_LEN);
      //qDebug() << "Received : " <<received;
      if( received > 0 ) {       
	bytes_received += received;
      }
      break;
    case SUSPEND:
      ready=1;
      suspended = 1;
      paused=0;
      break;
    case DONE:
      paused=suspended=ready=0;
      break;
    case PAUSE:
      paused = 1;
      suspended=0;
      break;
    default:
      qDebug() << "ThreadSignal could not be deciphered";
    }
  }
  suspended = 0;
  get_request->close();
  delete get_request;
  paused = 1;
}
