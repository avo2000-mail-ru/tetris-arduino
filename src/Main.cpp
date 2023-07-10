/*
* main.cc
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-22 02:40:09
*/

#include "Tetris.h"

static Console  console;
static Reactor  reactor(console);
static Tetris   tetris(reactor,console);

void Main() {
    console.init(stdin,stdout);
    tetris.init(0,0,1);
    tetris.start();
    
    for (;;) {
        if ( !reactor.handle_events() ) {
            tetris.timer_handler();
        }
    }
}