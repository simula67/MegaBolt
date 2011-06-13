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
#ifndef MEGA_HTTP_H
#define MEGA_HTTP_H

#include <QFile>
#include <QDateTime>
#include <QString>
#include <QUrl>
#include <QNetworkProxy>
#include <QFileInfo>
#include <QThread>
#include <QHttpResponseHeader>
#include <QDebug>
#include <QTcpSocket>
#include <QApplication>

#include "mega_common.h"
#include "mega_httpmainthread.h"

#define EOPEN -1
#define UNKNOWN -2
#define NOFILESIZE -3
#define NOMEM -4
#define ECONNECT -5

#ifndef LINE_LEN
#define LINE_LEN 1024
#endif

#ifndef REQUEST_PASS_SLEEP
#define REQUEST_PASS_SLEEP 2
#endif



class HttpDownload : public QThread{
  Q_OBJECT
private:
  QNetworkProxy *mega_proxy;
  HttpMainThread *mainThread;
 public:
  QUrl down_url;
  int id;
  QFile *dest_file;
  QDateTime start_time;
  QDateTime end_time;
  QString type;
  int size;
  int num_threads;
  int resumable;
  int status;
  QString uname;
  QString passwd;
  int bytes_download;
  QFile *thread_status;

  ThreadStatus *threads;
  QFile *timing_info;
  HttpDownload(QUrl down_url,int id,QFileInfo dest_file,QDateTime start_time,
	       QDateTime end_time,QString type,int num_threads,
	       QUrl mega_proxy,QString uname, QString passwd, 
	       QFileInfo thread_status,QFileInfo timing_info,int size = 0);
 void getHttp();
 ~HttpDownload();
 void pause();
protected:
  QHttpResponseHeader *head_response;
};
#endif
