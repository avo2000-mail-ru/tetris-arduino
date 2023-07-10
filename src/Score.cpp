/*
* Score.cc
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-22 03:11:39
*/

#include "Score.h"
#include <arduino/nano/io.h>
#include "console.h"

#define LEVEL_UP              20 // 

/* ------------------------------------------------------------------------ */
void Score::init(int x, int y, int level)
{
	_left = x;
	_top = y;
	_visible = 
	_score = 
	_lines = 
	_figures = 0;
	_level = level;
	_start_time = io_now();
}

/* ------------------------------------------------------------------------ */
void Score::start() {
	_score = 
	_lines = 
	_figures; 
	_start_time = io_now();
}

/* ------------------------------------------------------------------------ */
int Score::level() {
	//return _level = _figures / LEVEL_UP;
	return _level + _figures / LEVEL_UP;
}

/* ------------------------------------------------------------------------ */
void Score::draw() {
	_con.color(green);
	if (!_visible) {
		_con.box(_left, _top, green, "\
Lines:\n\
Figures:\n\
Level:\n\
Score:\n");
		_visible = 1;
	}

	_con.xy(_left + 10, _top);
	_con.bold();
	_con.put("%4d", _lines); _con.lf();
	_con.put("%4d", _figures); _con.lf();
	_con.put("%4d", _level + 1); _con.lf();
	_con.put("%4d", _score);
	_con.uncol();
}

/* ------------------------------------------------------------------------ */
void Score::refresh() {
	_visible = 0;
	draw();
}
