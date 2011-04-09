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
#ifndef MEGA_COMMON_H
#define MEGA_COMMON_H

enum DowloadStatus {DOWNLOADING,PAUSED,FIN};

class ThreadStatus {
public:
  int abs_start;
  int abs_end;
  int abs_pos;
  ThreadStatus(){
    abs_start=abs_end=abs_pos=0;
  }
};
#endif
