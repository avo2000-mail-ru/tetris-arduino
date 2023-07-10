/*
* Reactor.cc
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-23 23:26:03
*/

#include "Reactor.h"
#include <arduino/nano/io.h>
#include <arch.h>

/* ------------------------------------------------------------------------ */
Reactor::Reactor( Console& con) : _paused(false), _con(con) {}

/* ------------------------------------------------------------------------ */
void Reactor::register_handler( EventHandler *handler) {
    _list.push_back(handler);
}

/* ------------------------------------------------------------------------ */
void Reactor::unregister_handler( EventHandler *handler) {
    _list.remove(handler);
}

/* ------------------------------------------------------------------------ */
bool Reactor::handle_events() {
    bool rc = false;
    while (!rc) {
        if ( !_paused ) {
            unsigned long now = io_now();
            if ( GT_ULONG(now,_end) ) {
                break;
            }
        }
        for ( List::Node *node = _list.head(); node != nullptr; node = node->next ) {
            KEYCODE code = node->item->check_event();
            if (  code != KEY_NONE ) {
                node->item->handle_event(code);
                rc = true; 
            }
        }
    }
    return rc;
}
  
/* ------------------------------------------------------------------------ */
void Reactor::set_timeout(long timeout_msec) {
    if ( timeout_msec == -1 ) {
        if ( !_paused ) {
            _paused = true;
        }
    }
    else {
        _timeout = timeout_msec;
        _start = io_now();
        _end = _start + _timeout;
        _paused = false;
    }
}

