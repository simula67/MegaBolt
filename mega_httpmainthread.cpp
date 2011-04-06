#include "mega_httpmainthread.h"

HttpMainThread::HttpMainThread(QUrl *down_url,QFile *inp_dest_file,int *inp_size,int *inp_num_threads,int *inp_num_bytes,QFile *inp_thread_status,ThreadStatus *inp_threads,QNetworkProxy *inp_mega_proxy,int *inp_status)
{
dl_url = down_url;
dest_file = inp_dest_file;
size = inp_size;
num_threads = inp_num_threads;
bytes_download = inp_num_bytes;
thread_status = inp_thread_status;
threads = inp_threads;
mega_proxy = inp_mega_proxy;
pause_complete = 0;
paused = 0;
status = inp_status;
}
void HttpMainThread::run(){

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
  HttpDownThread *worker = new HttpDownThread[(*num_threads)];
  if( worker == NULL ) {
  	qDebug() << "No memory";
  }
  int i,all_start;
  for(i=0;i<(*num_threads);i++) {
    worker[i].down_url = (*dl_url);
    worker[i].range_start = threads[i].abs_start;
    worker[i].range_end = threads[i].abs_end;
    worker[i].mega_proxy = mega_proxy;
    worker[i].nextJob = INIT;
    worker[i].start();
  }
  /* Wait till all worker threads initialize */
  all_start = 0;
  while(!all_start) {
    all_start = 1;
    for(i = 0; i < (*num_threads); i++ ) {
      if( worker[i].ready == 0 ) {
	all_start = 0;
      }
    }
  }
  /* Now start the download loop */
  while( ((*bytes_download) < (*size)) && (!paused) ) {
    
    /* Start download */
    for(i=0;i<(*num_threads);i++) {
      if( worker[i].nextJob != DOWNLOAD ) {
	worker[i].nextJob = DOWNLOAD;
      }
    }
    
    /* Sleep */
    
    QThread::msleep(300);
    
    /* Suspend download */
    for(i=0;i<(*num_threads);i++) {
      if( worker[i].nextJob != SUSPEND ) {
	worker[i].nextJob = SUSPEND;
      }
    }
    all_start = 0;
    while(!all_start) {
      all_start = 1;
      for(i = 0; i < (*num_threads); i++ ) {
	if( worker[i].suspended == 0 ) {
	  all_start = 0;
	}
      }
    }
    /* All threads suspended. Save buffers to disk */
    for( i = 0;i < (*num_threads); i++ ) {
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
      (*bytes_download) += to_write;
      worker[i].bytes_received = 0;
      threads[i].abs_pos += to_write;
      /* We are just changing the abs_pos to abs_start so that we just have to write and delete on pause */
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
    thread_status->write((char *)threads,(*num_threads) * sizeof(ThreadStatus));
    qDebug() << "Downloaded "<< (int)(((float)(*bytes_download)/(float)(*size)) * 100) <<"%";
  }
  if(paused) {
    int j,all_start=1;
    for(int j=0;j<(*num_threads);j++) {
      worker[j].nextJob = PAUSE;
    }
    QThread::msleep(200);
    while(all_start) {
      all_start=0;
      for(j=0;j<(*num_threads);j++) {
	if( worker[j].paused == 0 ) {	
	  all_start = 1;
	}
      }
    }
    delete[] worker;
    (*status) = PAUSED;
  }
  if( (*bytes_download) == (*size) ) {
      (*status) = FIN;
  }
  pause_complete = 1;
}


