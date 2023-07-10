/*
* tetris.h
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-22 02:38:17
*/

#ifndef _id_3591da8c_0ae0_4ae7_a9c4_4899ed47b7c2
#define _id_3591da8c_0ae0_4ae7_a9c4_4899ed47b7c2

#include "Console.h"
#include "Board.h"
#include "Score.h"
#include "Reactor.h"

class Tetris
#ifdef USE_VIRTUAL_METHODS
 : public EventHandler
#endif
{
    enum { TO_BLACK_SCREEN_PAUSE = 1 };
    
    Reactor&    _reactor;
    Console&    _con;
    Board       _game;
    Board       _next;
    Score       _score;
#ifndef USE_VIRTUAL_METHODS
    EventHandler _handler;
#endif
    
	int         _stage;
	long        _period; // timer period;
	int         _paused;
	int         _options;
    int         _left;
    int         _top;
    
    void	    tick();
    void        refresh();
    void        gameover();
    void        handle_key(KEYCODE code);
    void        pause();
    void        new_game();
    bool        request_new_game();
    
  public:
    Tetris(Reactor& r, Console& c);
    ~Tetris();
    void init( int x, int y, int options);
    void free();
    void start();

    void timer_handler();

#ifdef USE_VIRTUAL_METHODS
    virtual void handle_event( KEYCODE code);
    virtual KEYCODE check_event();
#else
    static void key_handler(void *self, KEYCODE code);
#endif
};

#endif /* _id_3591da8c_0ae0_4ae7_a9c4_4899ed47b7c2 */
