/*
* Field.cc
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-22 03:27:03
*/

#include <stdlib.h>
#include <string.h>

#include "Field.h"

/* ------------------------------------------------------------------------ */
static const char BLOCK[] = "[]";
static const char CRACK[] = "**";
static const char LEFT_WALL[] = "<!";
static const char RIGHT_WALL[] = "!>";
static const char LEFT_CORNER[] = "<+";
static const char RIGHT_CORNER[] = "+>";
static const char BOTTOM_WALL[][3] = { "--", "\\/" };

/* ------------------------------------------------------------------------ */
Field::Field(Console& con) : _data(0), _back(0), _con(con) {
}

/* ------------------------------------------------------------------------ */
Field::~Field() {
    free();
}

/* ------------------------------------------------------------------------ */
void Field::init(int left, int top, unsigned int width, unsigned int height, char const *space) {
	size_t data_size = sizeof(_data[0]) * width * height;
	_data = new unsigned char [data_size];
	_back = new unsigned char [data_size];
	memset(_data,   0, data_size);
	memset(_back, 255, data_size);
	_left = left;
	_top = top;
	_width = width;
	_height = height;
	_space = space;
}

/* ------------------------------------------------------------------------ */
void Field::free() {
    if ( _back != NULL )
        delete [] _back;
    if ( _data != NULL )
        delete [] _data;
}

/* ------------------------------------------------------------------------ */
void Field::detonate(int phase) {
	for (int y = 0; y < _height; ++y)
		if (row_is_full(y))
			row_fill(y, phase ? 8 : 0);
	put();
}

/* ------------------------------------------------------------------------ */
void Field::put() {
	int pos = 0;
	for (int y = 0; y < _height; ++y) {
		int cx = -30;
		for (int x = 0; x < _width; ++x, ++pos) {
			if (_data[pos] != _back[pos]) {
				switch (x - cx) {
				default:
					_con.xy(x*2 + _left, y + _top);
					break;
				case 3:
				case 2:
				case 1:
					do {
						cell_out(_data[pos - x + cx]);
					} while (x != ++cx);
				case 0:;
				}
				cell_out(_back[pos] = _data[pos]);
				cx = x + 1;
			}
		}
	}
	_con.uncol();
}

/* ------------------------------------------------------------------------ */
void Field::cell_out(unsigned char e) {
	if (!e) {
		_con.uncol();
		_con.put_str(_space);
	} else {
		_con.bold();
		if (e < 8) {
			_con.color(e);
			_con.put_str(BLOCK);
		} else {
			_con.color(7);
			_con.put_str(CRACK);
		}
	}
}

/* ------------------------------------------------------------------------ */
void Field::row_fill(int y, int value)
{
	if (y >= 0 && y < _height)
		memset(_data + _width * (unsigned)y, value, sizeof(*_data)*_width);
}

/* ------------------------------------------------------------------------ */
int Field::row_is_empty(int y) {
	return !row_wiegh(y) ? 1 : 0;
}

/* ------------------------------------------------------------------------ */
int Field::row_is_full(int y) {
	return row_wiegh(y) == _width ? 1 : 0;
}

/* ------------------------------------------------------------------------ */
int Field::row_wiegh(int y) {
	int c = 0;
	for (unsigned char *cell = _data + _width * (unsigned)y, *end = cell + _width; cell < end; ++cell)
		c += *cell ? 1 : 0;
	return c;
}

/* ------------------------------------------------------------------------ */
int Field::check_detonations() {
	int c = 0;
	for (int y = 0; y < _height; ++y)
		c += row_is_full(y)? 1 : 0;
	return c;
}

/* ------------------------------------------------------------------------ */
void Field::walls_put() {
	_con.color(yellow);
	for (int i = 0; i < _height; ++i) {
		_con.xy(_left - 2, _top + i);
		_con.put_str(LEFT_WALL);
		_con.xy(_left + 2 * (signed)_width, _top + i);
		_con.put_str(RIGHT_WALL);
	}
	_con.xy(_left - 2, _top + (signed)_height);
	_con.put_str(LEFT_CORNER);
	for (int x = 0; x < _width; ++x)
		_con.put_str(BOTTOM_WALL[0]);
	_con.put_str(RIGHT_CORNER);
	_con.xy(_left, _top + (signed)_height + 1);
	for (int x = 0; x < _width; ++x)
		_con.put_str(BOTTOM_WALL[1]);
	_con.uncol();
}

/* ------------------------------------------------------------------------ */
void Field::refresh()
{
	memset(_back, 255, sizeof(_data[0]) * _width * _height);
	put();
}

/* ------------------------------------------------------------------------ */
void Field::downfall() {
	for (int up = (signed)_height - 2, down = up + 1; up >= 0; --up, --down)
		switch (row_is_empty(up) * 2 + row_is_empty(down)) {
		case 1:
			row_move(down, up); break;
		case 3:
			++down;
		}
	put();
}

/* ------------------------------------------------------------------------ */
void Field::row_move(int dst, int src)
{
	if (dst < 0 || src < 0 || dst >= _height || src >= _height)
		return;

	memcpy(_data + _width * (unsigned)dst, _data + _width * (unsigned)src, sizeof(*_data) * _width);
	row_fill(src, 0);
}

/* ------------------------------------------------------------------------ */
void Field::set_cell(int x, int y, int value) {
	if (check_coords(x, y))
		_data[cell_offset(x, y)] = (cell_value_t)value;
}

/* ------------------------------------------------------------------------ */
unsigned int Field::get_cell(int x, int y) {
	if (check_coords(x, y))
		return _data[cell_offset(x, y)];
	return (cell_value_t)~0;
}

/* ------------------------------------------------------------------------ */
unsigned int Field::check_coords(int x, int y) {
	return 0 <= x && x < _width &&
	       0 <= y && y < _height;
}


