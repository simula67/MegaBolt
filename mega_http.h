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

#ifndef LINE_LEN
#define LINE_LEN 1024
#endif

#ifndef QT_BUG_SLEEP
#define QT_BUG_SLEEP 2
#endif



class HttpDownload : public QThread{
  Q_OBJECT
private:
  QNetworkProxy *mega_proxy;
  HttpMainThread *mainThread;
 public:
  QUrl dl_url;
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
  HttpDownload(QUrl down_url,int down_id,QFileInfo down_file,QDateTime down_start,QDateTime down_end,QString down_type,int down_threads,QUrl down_proxy,QString down_uname, QString down_passwd,QFileInfo down_threadfile,QFileInfo down_timing,int size = 0);
 void getHttp();
 //~HttpDownload();
 void pause();
protected:
  QHttpResponseHeader *head_response;
};
#endif
