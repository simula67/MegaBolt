#ifndef MEGA_HTTPMAINTHREAD_H
#define MEGA_HTTPMAINTHREAD_H
#include <QThread>
#include <QTcpSocket>
#include "mega_common.h"
#include <QFile>
#include <QNetworkProxy>
#include "mega_httpdownthread.h"


class HttpMainThread : public QThread {
	QUrl *dl_url;
	QFile *dest_file;
	int *size;
	int *num_threads;
	int *bytes_download;
	QFile *thread_status;
	ThreadStatus *threads;
	QNetworkProxy *mega_proxy;
	int *status;
public:
	int volatile pause_complete;
	int paused;
	void run();
	HttpMainThread(QUrl *down_url,QFile *inp_dest_file,int *inp_size,int *inp_num_threads,int *inp_num_bytes,QFile *inp_thread_status,ThreadStatus *threads,QNetworkProxy *inp_mega_proxy,int *inp_status);
};


#endif
