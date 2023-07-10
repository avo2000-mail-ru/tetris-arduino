/*
* Reactor.h
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-23 23:25:50
*/

#ifndef _id_607a5591_8e69_43cb_99e6_50987bc84a79
#define _id_607a5591_8e69_43cb_99e6_50987bc84a79

#include "EventHandler.h"
#include "List.h"
#include "Console.h"

class Reactor {
    
    typedef LinkedList<EventHandler*> List;
    List                    _list;
    unsigned long           _timeout;
    unsigned long           _end;
    unsigned long           _start;
    int                     _paused;
    Console &               _con;
    
  public:
    Reactor( Console& con);
    void register_handler( EventHandler *handler);
    void unregister_handler( EventHandler *handler);
    
    void set_timeout(long timeout_msec);
    bool handle_events();
};

#endif /* _id_607a5591_8e69_43cb_99e6_50987bc84a79 */
