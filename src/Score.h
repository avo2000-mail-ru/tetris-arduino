/*
* Score.h
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-22 03:11:32
*/

#ifndef _id_15611989_f1b1_4943_8986_d8cf12d2eab1
#define _id_15611989_f1b1_4943_8986_d8cf12d2eab1

#include "Console.h"

class Score {
	int             _left, _top;
	unsigned long   _start_time;
	int             _visible;
    Console &       _con;
    int             _level;
    
  public:
	int             _lines, _figures, _score;

    Score( Console& con) : _con(con) {}

    void init(int x, int y, int level);
    void draw();
    void refresh(); 

    void start();
    int  level();
};

#endif /* _id_15611989_f1b1_4943_8986_d8cf12d2eab1 */
