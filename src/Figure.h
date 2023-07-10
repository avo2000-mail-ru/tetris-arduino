/*
* Figure.h
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-22 03:25:32
*/

#ifndef _id_cb4437ac_a705_4a2c_87dc_48d8fa5b7417
#define _id_cb4437ac_a705_4a2c_87dc_48d8fa5b7417

#include "Field.h"

class Figure {

    static unsigned int get_figure(int fig, int angle);
    
  public:
	int _id;
	int _x, _y, _angle;
    
    Figure(){}

    void init(int id);
    void draw(Field *field, int show);
    int  test(Field *field);
    
    static int top(int id);
};

#endif /* _id_cb4437ac_a705_4a2c_87dc_48d8fa5b7417 */
