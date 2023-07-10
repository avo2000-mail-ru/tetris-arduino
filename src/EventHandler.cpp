/*
* EventHandler.cc
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-11-01 13:04:36
*/

#include "EventHandler.h"
#include "Tetris.h"

#ifndef USE_VIRTUAL_METHODS
void EventHandler::handle_event( KEYCODE code) {
    _owner->key_handler( _owner, code);
}

KEYCODE EventHandler::check_event() {
    return (KEYCODE) _con.check_event();
}
#endif
