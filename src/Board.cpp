/*
* Board.cc
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-22 03:11:05
*/

#include <stdlib.h>

#include "Board.h"

/* -------------------------------------------------------------------------- */
void Board::init(int left, int top, unsigned int width, unsigned int height, char const *space) {
	_field.init(left, top, width, height, space);
	_figure.init(-1);
}

/* -------------------------------------------------------------------------- */
void Board::free() {
	_field.free();
	//figure_free(&self->figure);
}


/* -------------------------------------------------------------------------- */
void Board::figure_set(int id) {
	_figure._id = id;
	_figure._angle = 0;
	_figure._x = (signed)(_field._width - 4) >> 1;
	_figure._y = -_figure.top(id);
}

/* -------------------------------------------------------------------------- */
void Board::figure_new() {
	figure_draw(0);
	figure_set(rand_fig());
	figure_draw(1);
}

/* -------------------------------------------------------------------------- */
int Board::rand_fig() {
	static int last = -1;
	int next;
	do {
		next = (rand() & 0x7FFF) % 7;   // strip sign before get remainder
	//} while (next == last);
	} while (0);
	return last = next;
}

/* -------------------------------------------------------------------------- */
int Board::figure_move(int dx, int dy, int dang) {
	_figure.draw(&_field, 0);
	int res = figure_submove(dx, dy, dang);
	_figure.draw(&_field, 1);
	if (res >= 0) // the figure has moved
		_field.put();
	return res;
}

/* -------------------------------------------------------------------------- */
int Board::figure_submove(int dx, int dy, int dang) {
	_figure._x += dx;
	_figure._y += dy;
	_figure._angle += dang;
	if (_figure.test(&_field)) {
		if (!dx && !dy && dang && _figure._y < 0) { // patch for "line" fig. rotation at top position
			_figure._y = 0;
			if (!_figure.test(&_field))
				return 0;
		} else
			_figure._y -= dy; 
		_figure._x -= dx;
		_figure._angle -= dang;
		return -1; // no place to move
	}
	return 0;
}

/* -------------------------------------------------------------------------- */
int Board::figure_drop()
{
	_figure.draw(&_field, 0);
	int c = 0;
	for (; !figure_submove(0, 1, 0); ++c);
	_figure.draw(&_field, 1);
	return c;
}



