/*
* tetris.cc
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-22 02:38:26
*/

#include <stdlib.h>

#include "Tetris.h"
#include "Console.h"
#include <arduino/nano/io.h>

#define LEVEL_PERIOD_FACTOR 750 // means .75
#define GAME_PERIOD         1000
#define EXPLOSION_PERIOD    400

enum {
    TS_INIT,

    TS_NEW_FIGURE,
    TS_GAMING, // -> TS_NEW_FIGURE or TS_DETONATION

    TS_DETONATION,
    TS_ANNIHILATION,
    TS_DOWNFALL, // -> NEW_FIGURE
    TS_PAUSE
};

/* -------------------------------------------------------------------------- */
Tetris::Tetris(Reactor& r, Console& c)
    : _reactor(r), _con(c), _score(c), _game(c), _next(c)
#ifndef USE_VIRTUAL_METHODS
     , _handler(this,c) 
#endif
{
}

/* -------------------------------------------------------------------------- */
Tetris::~Tetris() {
    free();
}

/* -------------------------------------------------------------------------- */
void Tetris::init( int left, int top, int options) {
    _left = left;
    _top = top;

    _stage = TS_INIT;
    _period = 0;
#ifdef USE_VIRTUAL_METHODS
    _reactor.register_handler( this);
#else
    _reactor.register_handler( &_handler);
#endif
    _paused = 0;
    _options = options;
    _con.cls(); 
    _con.show_cursor(0);
    _con.colors_enable(1);
    
    new_game();
}

/* -------------------------------------------------------------------------- */
void Tetris::free() {
    _game.free();
    _next.free();
    _con.xy(1, 25);
    _con.uncol();
    _con.show_cursor(1);
    _con.put_str("                \n");          // Push characters to tty hw FIFO
}

/* -------------------------------------------------------------------------- */
void Tetris::start() {
    tick();
}

/* -------------------------------------------------------------------------- */
void Tetris::new_game() {
    _con.cls(); 
    _con.xy(1,22);
    _con.put_str("Enter your level (0..9) ");
    int c;
    for(;;) {
        c = _con.get_char();
        if ( '0' <= c && c <= '9' )
            break;
    }

    static bool rand_initialized = false;
    if ( !rand_initialized ) {
        int n = (int)io_now();
        srand(n);
        rand_initialized = true;
    }

    _score.init(_left + 5, _top + 4, c - '0' );
    _game.init(_left + 27, _top + 2, 10, 20, " .");
    _next.init(_left + 10, _top + 11, 4, 4, "  ");
}

/* -------------------------------------------------------------------------- */
void Tetris::gameover()
{
    _con.xy(1, 20);
    _con.put_str("Game Over!");
}

/* -------------------------------------------------------------------------- */
void Tetris::tick() {
    if (_paused)
        return;

    long next_timeout = GAME_PERIOD;
    switch (_stage) {
    case TS_INIT:
        refresh();
        _next.figure_new();
        _score.start();
        ++_stage;
        next_timeout = 1;
        break;

    case TS_NEW_FIGURE: {
        int fig = _next._figure._id;
        _next.figure_new();
        _next.put();
        _score.draw();
        int level = _score.level();
        long period = GAME_PERIOD;
        while (--level > 0) {
            period = (period * LEVEL_PERIOD_FACTOR) / 1000;
        }
        _period = period;

        _game.figure_set(fig);
        if (_game.figure_test()) {
            _game.put();
            gameover();
            if ( request_new_game() ) {
                new_game();               
                _stage = TS_INIT;
                next_timeout = 1;
                break;
            }
            else {
                exit(0);
                return;
            }
        }

        _game.figure_draw(1);
        _game.put();
        ++_stage;
        next_timeout = period;
        break; }

    case TS_GAMING: { // -> TS_NEW_FIGURE or TS_DETONATION
        if (!_game.figure_move(0, 1, 0)) {
            next_timeout = _period;
            break;
        }
        _score._figures += 1;

        int c = _game.check_detonations();
        if (c) {
            _score._lines += c;
            _score._score += c * c * 10;
            _score.draw();
        }
        _stage = c ? _stage + 1 : TS_NEW_FIGURE;
        next_timeout = 1;
        break; }

    case TS_DETONATION:
    case TS_ANNIHILATION:
        _game.detonate(_stage == TS_DETONATION);
        ++_stage;
        next_timeout = EXPLOSION_PERIOD;
        break;

    case TS_DOWNFALL: // -> TS_NEW_FIGURE
        _game.downfall();
        _stage = TS_NEW_FIGURE;
        next_timeout = EXPLOSION_PERIOD;
        break;
    }
    
    _reactor.set_timeout(next_timeout);
}

/* -------------------------------------------------------------------------- */
void Tetris::refresh() {
    static char const text[] = "\
  cursor keys\n\
       or\n\
\n\
     rotate\n\
       |\n\
      [w]\n\
<-[a] [s] [d]->\n\
\n\
    [space]\n\
       |\n\
       V\n\
\n\
  [p] - pause\n\
  [q] - quit\
";

    _con.cls();
    _con.box(55, 3, cyan, text);
    _game._field.walls_put();

    _score.refresh();
    _next.refresh();
    _game.refresh();
    _con.dock();
}

/* -------------------------------------------------------------------------- */
void Tetris::timer_handler()
{
    tick();
    _con.dock();
}

/* -------------------------------------------------------------------------- */
void Tetris::handle_key( KEYCODE code)
{
	if (_stage != TS_GAMING)
		return;

	if (_paused) {
		if (code == KEY_PAUSE)
			pause();
		return;
	}

	switch (code) {
	case KEY_LEFT:   _game.figure_move(-1, 0,  0); break;
	case KEY_RIGHT:  _game.figure_move( 1, 0,  0); break;
	case KEY_UP:     _game.figure_move( 0, 0,  1); break;
	case KEY_DOWN:   _game.figure_move( 0, 0, -1); break;
	case KEY_DROP:
		_score._score += _game.figure_drop();
        _reactor.set_timeout(1);
		break;
	case KEY_PAUSE:  pause(); break;
	case KEY_QUIT:
        _game.put();
        gameover();
        exit(0);
	}
    _con.dock();
	_con.flush();
}

/* -------------------------------------------------------------------------- */
void Tetris::pause()
{
    _reactor.set_timeout((_paused = !_paused) ? -1 : 1000);
	if (_paused) {
		if (_options & TO_BLACK_SCREEN_PAUSE)
			_con.cls();
	} else {
		refresh();
	}
}

/* -------------------------------------------------------------------------- */
bool Tetris::request_new_game() {
    _con.xy(1,22);
    _con.put_str("New game? y/n");
    for(;;) {
        int c = _con.get_char();
        if ( (c == 'Y') || (c == 'y') )
            return true;
        if ( (c == 'N') || (c == 'n') )
            return false;
    }
}

#ifdef USE_VIRTUAL_METHODS
/* -------------------------------------------------------------------------- */
void Tetris::handle_event( KEYCODE code) {
    handle_key(code);
}

/* -------------------------------------------------------------------------- */
KEYCODE Tetris::check_event() {
    return (KEYCODE) _con.check_event();
}
/* -------------------------------------------------------------------------- */
#else // USE_VIRTUAL_METHODS
void Tetris::key_handler(void *self, KEYCODE code)
{
    (static_cast<Tetris *>(self))->handle_key(code);
}
#endif