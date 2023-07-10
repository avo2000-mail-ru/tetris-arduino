/*
* Field.h
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-22 03:26:58
*/

#ifndef _id_7256d2bb_5e82_4406_b83f_5ec4e2654946
#define _id_7256d2bb_5e82_4406_b83f_5ec4e2654946

#include "Console.h"

class Field {
    typedef
    unsigned char cell_value_t;
    
	int _left, _top;
	cell_value_t *_data, *_back;
	char const *_space;
    Console&    _con;
    
    void cell_out(unsigned char e);
    unsigned int cell_offset(int x, int y) {
        return (unsigned)y * _width + (unsigned)x;
    }
    
  public:
	unsigned int _width, _height;

    Field(Console& con);
    ~Field();

    void init(int left, int top, unsigned int width, unsigned int height, char const *space);
    void free();

    void walls_put();
    void put();
    void refresh();

    void row_fill(int y, int value);
    void fill(int value);
    void row_move(int dst, int src);
    int row_wiegh(int y);
    int row_is_empty(int y);
    int row_is_full(int y);
    unsigned int check_coords(int x, int y);

    void set_cell(int x, int y, int value);
    unsigned int get_cell(int x, int y);

    int check_detonations();
    void detonate(int phase);
    void downfall();
};

#endif /* _id_7256d2bb_5e82_4406_b83f_5ec4e2654946 */
