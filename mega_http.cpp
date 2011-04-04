#include "mega_http.h"
#include "mega_httpdownthread.h"
#include <QDebug>
#include <QTcpSocket>
#include <QHttpResponseHeader>
#include <QApplication>

HttpDownload::HttpDownload(QUrl down_url,int down_id,QFileInfo down_file,QDateTime down_start,QDateTime down_end,QString down_type,int down_threads,QUrl down_proxy,QString down_uname, QString down_passwd,QFileInfo down_threadfile,QFileInfo down_timing) {
  QUrl no_value("");
  dl_url = down_url;
  id=down_id;
  dest_file = new QFile(down_file.absoluteFilePath());
  if(!dest_file->open(QIODevice::ReadWrite|QIODevice::Unbuffered)) {   
    
    throw EOPEN;
  }
  thread_status = new QFile(down_threadfile.absoluteFilePath());
  if(!thread_status->open(QIODevice::ReadWrite)) {   
    throw EOPEN;
  }
  timing_info = new QFile(down_timing.absoluteFilePath());  
  if(!timing_info->open(QIODevice::ReadWrite)) {   
    throw EOPEN;
  }
  start_time=down_start;
  end_time = down_end;
  type = down_type;
  num_threads = down_threads;
  
  if( down_proxy != no_value ) {
    /* Add code for other type of proxies */
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
      if( dl_url.port() < 0 )
	dl_url.setPort(80);
      head_request->connectToHost(dl_url.host(), (quint16) dl_url.port());
      if (head_request->waitForConnected(18000)) {
	qDebug("Connected!");
      }
      char buff[LINE_LEN];
      
      if(head_request->state() == QAbstractSocket::ConnectedState) {
	head_request->write(request_string.toAscii());
	head_request->flush();
	if(head_request->waitForBytesWritten(-1)) {
	  qDebug() << "All bytes written";
	}
	else {
	  //qDebug() << "Some bytes were not written. Error : ";
	  //qDebug() << head_request->errorString();
	  qDebug() << "Working around QT bug with just sleeping QT_BUG_SLEEP seconds";
	  QThread::sleep(QT_BUG_SLEEP);
	}
	response_string.resize(0);
	while( head_request->readLine(buff,LINE_LEN) ) {
	  response_string.append(buff);
	}
	head_response = new QHttpResponseHeader(response_string);
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
  qDebug() << "Size is : "<<head_response->value("Content-Length");
  size = (head_response->value("Content-Length")).toInt();
  if(size == 0) {
    qDebug() << "Size not found. Cant download without knowing file size";
    throw NOFILESIZE;
  }
  dest_file->resize(size);
  delete head_response;
  head_response = NULL;
  
  threads = new ThreadStatus[num_threads];
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
  paused = 0;
  sleep(5);
}

void HttpDownload::getHttp()
{
  /* If already paused we are resuming */
  if(paused == 1) {
    paused = 0;
  }
  /* 
     Create the threads, initialize the URL and ranges by hand.
     Connect the start_download signal of the "this" to start_download slot of each thread
     |||y for suspend_download

     Now, set the nextJob to INIT. Once all threads have ready == 1 emit start_download()
     Every 300ms, emit suspend_download() and  
     save all the buffers to disk. If any thread downloaded past its abs_end, discard the extra data.
     Make bytes_downloaded of each thread 0
     
     If any thread finished downloading, find longest undivided block and re-arrage the ranges
     

     Update the thread abs_pos and write out thread_file
  */
  
  HttpDownThread *worker = new HttpDownThread[num_threads];
  int i,all_start;
  for(i=0;i<num_threads;i++) {
    worker[i].down_url = dl_url;
    worker[i].range_start = threads[i].abs_start;
    worker[i].range_end = threads[i].abs_end;
    worker[i].mega_proxy = mega_proxy;
    worker[i].nextJob = INIT;
    QObject::connect(this,SIGNAL(start_download()),(&worker[i]),SLOT(start_download()));
    QObject::connect(this,SIGNAL(suspend_download()),(&worker[i]),SLOT(suspend_download()));
    worker[i].start();
  }
  
  /* Wait till all threads initialize */
  all_start = 0;
  while(!all_start) {
    all_start = 1;
    for(i = 0; i < num_threads; i++ ) {
      if( worker[i].ready == 0 ) {
	all_start = 0;
      }
    }
  }
  
  /* Now start the download loop */
  while( (bytes_download < size) && (!paused) ) {
    emit start_download();
    /*
      After this the nextJob of each thread MUST be DOWNLOAD
      Sometimes WIERDLY it does not happen.
      This is a workaround to set it to DOWNLOAD

    */
    for(i=0;i<num_threads;i++) {
      if( worker[i].nextJob != DOWNLOAD ) {
	worker[i].nextJob = DOWNLOAD;
      }
    }
    
    QThread::msleep(300);
    emit suspend_download();
    all_start = 0;
    while(!all_start) {
      all_start = 1;
      for(i = 0; i < num_threads; i++ ) {
	if( worker[i].suspended == 0 ) {
	  all_start = 0;
	}
      }
    }
    /* All threads suspended. Save buffers to disk */
    for( i = 0;i < num_threads; i++ ) {
      int to_write;
      if( threads[i].abs_pos + worker[i].bytes_received > threads[i].abs_end ) {
	/* Downloaded past the end of the range */
	to_write = (threads[i].abs_end) - (threads[i].abs_pos);
	worker[i].nextJob = DONE;
      }
      else
	to_write = worker[i].bytes_received;

      if( !dest_file->seek(threads[i].abs_pos) ) {
	qDebug()<<"Cant seek!!!";
	exit(1);
      }  
      int written=0;
      /* Repeatedly write to file */
      while( (written += dest_file->write( (worker[i].buffer) + written,(to_write - written) )) < to_write );
      dest_file->flush();
      if(dest_file->waitForBytesWritten(-1)) {
	qDebug() << "All bytes written";
      }
      else {
	;
	//qDebug() << "Could not write. This is sometimes pretty bad";
      }
      bytes_download += to_write;
      worker[i].bytes_received = 0;
      threads[i].abs_pos += to_write;
      /* We are just change the abs_pos to abs_start so that we just have to write and delete on pause */
      threads[i].abs_start = threads[i].abs_pos;
    }
    
    /* Now take the DONE threads and reposition the range_start and range end and put it in INIT state 
    
    (a) First look for one DONE thread.
    (b) If found look throughout the "threads" array to find a (abs_end - abs_pos) > MIN_SPLIT_LEN
    (c) Now make range_start of DONE thread to be (abs_end - abs_pos)/2 and range_end be abs_end of unfinished thread
    (d) Similarly make changes for unfinished thread.
    (e) Change abs_start and abs_end in "threads" array for finished and unfinished thread
   
    */
    thread_status->seek(0);
    thread_status->write((char *)threads,num_threads * sizeof(ThreadStatus));
    qDebug() << "Downloaded "<<bytes_download<<" bytes";
  }
  if(paused) {
    int j,all_start=1;
    for(int j=0;j<num_threads;j++) {
      worker[j].nextJob = PAUSE;
    }
    QThread::msleep(200);
    while(all_start) {
      all_start=0;
      for(j=0;j<num_threads;j++) {
	if( worker[j].paused == 0 ) {
	  all_start = 1;
	}
      }
    }
    delete[] worker;
  }
}    
void HttpDownload::pause(void) {
  paused = 1;
}
