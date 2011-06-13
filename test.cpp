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
#include <QApplication>
#include <QUrl>
#include <QDateTime>
#include <QFileInfo>
#include "mega_http.h"
#include <QThread>
#include <unistd.h>

int main(int argc,char *argv[])
{
  QApplication *app=new QApplication(argc,argv);
  QUrl down_url;
  down_url.setEncodedUrl("http://localhost/a.abc");
  QFileInfo down_file("/home/simula67/Repos/MegaBolt/down_file");
  QDateTime down_start = QDateTime::currentDateTime();
  QDateTime down_end = QDateTime::currentDateTime();
  QString down_type = "http";
  QFileInfo down_threads("/home/simula67/Repos/MegaBolt/thread_file");
  QFileInfo down_timing("/home/simula67/Repos/MegaBolt/timing_file");
  QUrl down_proxy("");
  try { 
    HttpDownload xilinx(down_url,1,down_file,down_start,down_end,down_type,2,down_proxy,"","",down_threads,down_timing);
    xilinx.getHttp();
    while( xilinx.status != DOWN_FIN ) {
    	sleep(1);
    }
  }
  catch(int e) {
    qDebug()<<"Some error in creating the object";
  }

  delete app;
  return 0;
}

