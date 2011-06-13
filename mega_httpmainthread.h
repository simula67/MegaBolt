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
#ifndef MEGA_HTTPMAINTHREAD_H
#define MEGA_HTTPMAINTHREAD_H
#include <QThread>
#include <QTcpSocket>
#include "mega_common.h"
#include <QFile>
#include <QNetworkProxy>
#include "mega_httpdownthread.h"


class HttpMainThread : public QThread {
	QUrl *down_url;
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
