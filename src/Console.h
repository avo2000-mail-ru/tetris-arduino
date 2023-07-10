/*
* Console.h
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-11-12 14:59:10
*/

#ifndef _id_7a383413_064a_47cd_9028_ce81456aee31
#define _id_7a383413_064a_47cd_9028_ce81456aee31

#include <stdarg.h>
#include <stdio.h>

#ifndef KB_TIMEOUT
#define KB_TIMEOUT  50      // timeout (ms) for next character of ESC sequence 
#endif

enum { black, red, green, yellow, blue, fuchsia, cyan, white };

enum  KEYCODE { KEY_NONE = 0, KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_DROP, KEY_PAUSE, KEY_QUIT };

enum TTY_MODE {
    TTY_MODE_NOTCOMPATIBLE, TTY_MODE_UNKNOWN, TTY_MODE_VT52, TTY_MODE_ANSI, 
};

class Console {
    FILE *          _in;
    FILE *          _out;
    int             _mode;
    bool            _mode_detected;
    int             _x, _y, _colored;
    int             _color, _bgr, _bold;
    const char *    _terminal_name;
    
    int _vout(char const *fmt, va_list ap);
    int _get_ch( int *buf);
    int check_reply(const char *escapes[], int timeout = KB_TIMEOUT);
    void detect();
    void termid();
    void exit_not_compatible();   

  public:
    Console();
    
    void init( FILE * in, FILE * out);
    void free();
    
    void colors_enable(int yes);

    int put_char(char ch);
    int get_char();
    int put_str(char const *str);
    int put(char const *fmt, ...);
    int write( const char *buf, unsigned int size);
    int read( char *buf, unsigned int size, bool & overrun, int timeout = KB_TIMEOUT);
    void flush();

    int xy(int x, int y);
    int lf();
    int cls();
    int color(int c);
    int bold();
    int uncol();
    int bgr(int c);
    int unbold();
    int show_cursor(int yes);
    void box(int x, int y, int color, char const *str);

    int check_event();
    int dock();
};

#endif /* _id_7a383413_064a_47cd_9028_ce81456aee31 */
