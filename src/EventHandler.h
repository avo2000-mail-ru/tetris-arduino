/*
* EventHandler.h
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-23 23:30:34
*/

#ifndef _id_03cd360e_3e00_4487_91a5_ca6ac2fe42e6
#define _id_03cd360e_3e00_4487_91a5_ca6ac2fe42e6

#include "Console.h"

class Tetris;

class EventHandler {
#ifdef USE_VIRTUAL_METHODS
  public:
    virtual void handle_event( KEYCODE code) = 0;
    virtual KEYCODE check_event() = 0;
#else
    Tetris*     _owner;
    Console&    _con;
  public:
    EventHandler( Tetris* owner, Console& con) : _owner(owner), _con(con) {}
    virtual void handle_event( KEYCODE code);
    virtual KEYCODE check_event();
#endif
};

#endif /* _id_03cd360e_3e00_4487_91a5_ca6ac2fe42e6 */
