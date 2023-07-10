/*
* Figure.cc
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-22 03:25:39
*/

#include "Figure.h"

#define FIGURE_LENGTH 4

/*
	0 1 2 3      0 1         1
	4 5 6 7        5 6     4 5
	8 9 A B                8
	C D E F
*/

/* ------------------------------------------------------------------------ */
unsigned int Figure::get_figure(int fig, int angle) {
	static const unsigned short c_figures[] = {
		0x0156, 0x1458, 0x0156, 0x1458, // z
		0x1245, 0x0459, 0x1245, 0x0459, // s
		0x2456, 0x159A, 0x0124, 0x0159, // L
		0x0126, 0x1589, 0x0456, 0x1259, // j
		0x1459, 0x1456, 0x1569, 0x0125, // T
		0x4567, 0x159d, 0x4567, 0x159d, // |
		0x1256, 0x1256, 0x1256, 0x1256  // o
	};

	//assert(fig >= 0 && fig < 7);
	return c_figures[fig * 4 + (angle & 3)];
}

/* ------------------------------------------------------------------------ */
void Figure::init(int id) {
	_x = _y = _angle = 0;
	_id = id;
}

/* ------------------------------------------------------------------------ */
int Figure::top(int id) {
	unsigned int fig = get_figure(id, 0);
	unsigned int top = 10;
	for (int i = 0; i < FIGURE_LENGTH && top; ++i, fig >>= 4) {
		unsigned int y = ((fig >> 2) & 3);
		if (y < top)
			top = y;
	}
	return (signed)top;
}

/* ------------------------------------------------------------------------ */
void Figure::draw(Field *field, int show) {
	int id = _id;
	if (id < 0)
		return;
	unsigned char fill = (unsigned char)(id + 1);
	if (!show)
		fill = 0;
	unsigned int fig = get_figure(id, _angle);
	int x = _x, y = _y;
	for (int i = 0; i < FIGURE_LENGTH; ++i, fig >>= 4) {
		field->set_cell(x + (signed)(fig & 3), y + (signed)((fig >> 2) & 3), fill);
    }
}


/* ------------------------------------------------------------------------ */
int Figure::test(Field *field)
{
	int id = _id;
	if (id < 0)
		return 1;
	unsigned int fig = get_figure(id, _angle);
	int x = _x, y = _y;
	for (int i = 0; i < FIGURE_LENGTH; ++i, fig >>= 4) {
		if (field->get_cell(x + (signed)(fig & 3), y + (signed)((fig >> 2) & 3)))
			return 1;
    }
	return 0;
}

