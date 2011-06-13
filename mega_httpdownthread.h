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
#ifndef MEGA_HTTPDOWNTHREAD_H
#define MEGA_HTTPDOWNTHREAD_H
#include <QThread>
#include <QUrl>
#include <QNetworkProxy>

#define LINE_LEN 1024
#define DOWNLOAD_LEN 50
#define BUFFSIZE 51200 // 500KB Buffer Size

#ifndef REQUEST_PASS_SLEEP
#define REQUEST_PASS_SLEEP 2
#endif

enum ThreadSignal {INIT,DOWNLOAD,SUSPEND,DONE,PAUSE};

class HttpDownThread : public QThread {
  Q_OBJECT
 protected:
   QString *response_string;
   QAbstractSocket *get_request;
 public:
  void run();
  int volatile ready,suspended,paused;
  int bytes_received;
  ThreadSignal nextJob;
  char buffer[BUFFSIZE];
  QUrl down_url;
  int range_start;
  int range_end;
  HttpDownThread() {
    ready = 0;
    nextJob = SUSPEND;
    bytes_received = 0;
    paused = 0;
  }
  QNetworkProxy *mega_proxy;
};
#endif
