/*
* Board.h
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-22 03:10:59
*/

#ifndef _id_3a57ae16_77af_4284_97ab_3fd3632d6f17
#define _id_3a57ae16_77af_4284_97ab_3fd3632d6f17

#include "Figure.h"
#include "Console.h"

class Board {

    Console&    _con;

    static int rand_fig();
    int figure_submove(int dx, int dy, int dang);

  public:
	Figure  _figure;
	Field   _field;
    
    Board( Console& con) : _con(con), _field(con) {}

    void init(int left, int top, unsigned int width, unsigned int height, char const *space);
    void free();
 
    void figure_set(int id);
    void figure_new();
    int  figure_move(int dx, int dy, int dang);
    int  figure_drop();
    
    /* -------------------------------------------------------------------------- */
    void put() {
        _field.put();
    }

    /* -------------------------------------------------------------------------- */
    void refresh() {
        _field.refresh();
    }

    /* -------------------------------------------------------------------------- */
    void figure_draw(int show) {
        _figure.draw(&_field, show);
    }

    /* -------------------------------------------------------------------------- */
    int figure_test() {
        return _figure.test(&_field);
    }

    /* -------------------------------------------------------------------------- */
    int check_detonations() {
        return _field.check_detonations();
    }

    /* -------------------------------------------------------------------------- */
    void detonate(int phase) {
        _field.detonate(phase);
    }

    /* -------------------------------------------------------------------------- */
    void downfall() {
        _field.downfall();
    }

    
};

#endif /* _id_3a57ae16_77af_4284_97ab_3fd3632d6f17 */
