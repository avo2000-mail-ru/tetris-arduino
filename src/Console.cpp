/*
* Console.cc
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-11-12 14:59:30
*/

#include "Console.h"

#include <arch.h>
#include <stdio.h>
#include <stdlib.h>
//#include <io.h>
#include <string.h>
#include <arduino/nano/io.h>
#include <errno.h>
#include <fcntl.h>
#include <debug.h>

inline static bool isdigit( char c) {
    return ('0' <= c) && (c <= '9') ? true : false;
}

inline static bool isodigit( char c) {
    return ('0' <= c) && (c <= '7') ? true : false;
}

#ifndef UINT_MAX
#define	UINT_MAX	((unsigned int)(~0))		/* 0xFFFF */
#endif

/*
 * Convert a string to an unsigned integer.
 *
 * Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
static unsigned int
strtonum(const char *nptr, char **endptr)
{
    int base = 10;
	const char *s = nptr;
	unsigned int acc;
	int c;
	unsigned int cutoff;
	int neg = 0, any, cutlim;

	/*
	 * See strtol for comments as to the logic used.
	 */
	c = *s++;
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	cutoff = (unsigned int)UINT_MAX / (unsigned int)base;
	cutlim = (unsigned int)UINT_MAX % (unsigned int)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = UINT_MAX;
		errno = ERANGE;
	} else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);
	return (acc);
}

/* ------------------------------------------------------------------------ */
Console::Console() : _mode_detected(false), _in(NULL), _out(NULL) {}

/* ------------------------------------------------------------------------ */
int Console::put_char(char ch) {
    return write(&ch,1);
}

/* ------------------------------------------------------------------------ */
int Console::put_str(char const *str) {
    return write(str,strlen(str));
}

/* ------------------------------------------------------------------------ */
int Console::put(char const *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int done = _vout(fmt, ap);
	va_end(ap);
	return done;
}

/* ------------------------------------------------------------------------ */
int Console::write(char const *data, unsigned int size)
{
//	return ::write(fileno(_out),data,size);
    return ::write(data,size);
}

/* ------------------------------------------------------------------------ */
int Console::_vout(char const *fmt, va_list ap)
{
#ifdef USE_SNPRINTF_FOR_CONSOLE
	char buf[2048];
	ssize_t len = vsnprintf(buf, sizeof(buf), fmt, ap);
	return write(buf, (size_t)len);
#else
	int wrtn = 0;
	do {
		char const *perc = strchr(fmt, '%');
		if (!perc) {
			wrtn += put_str(fmt);
			break;
		}
		if (fmt < perc) {
			wrtn += write(fmt, (size_t)(perc - fmt));
			fmt = perc;
		}
		++fmt;
		int width = 0;
		for (; *fmt >= '0' && *fmt <= '9'; ++fmt)
			width = width * 10 + (*fmt - '0');
		switch (*fmt) {
		case 'd': {
				char num[64], *p = num + sizeof(num);
				int arg = va_arg(ap, int),
				    minus = arg < 0;
				if (minus)
					arg = -arg;
				do {
					*--p = (char)('0' + arg % 10);
					--width;
					arg /= 10;
				} while (arg);
				if (minus)
					*--p = '-';
				for (; width > 0; --width)
					*--p = ' ';
				wrtn += write(p, (size_t)(num + (sizeof num) - p));
			} break;
		case 's': {
				char *arg = va_arg(ap, char *);
				wrtn += put_str(arg);
			} break;
		case 'c': {
				wrtn += put_char((char)va_arg(ap, int));
			} break;
		case '%':
			wrtn += put_char('%');
			break;
		default:
			wrtn += write(fmt - 1, 2);
		}
	} while (*++fmt);
	return wrtn;
#endif
}

/* ------------------------------------------------------------------------ */

int Console::_get_ch( int *buf) {
    //*buf = fgetc(_in);
    //int r = (ferror(_in) || feof(_in)) ? (errno == EAGAIN ? 0 : -1) : 1;
    //if ( r == 0 )
    //    clearerr(_in);
    //return r;
    if (available()) {
        *buf = getchr();
        return 1;
    }
    return 0;
}

int Console::get_char() {
    int c;
    int r;
    do {
        r = _get_ch(&c);
    } while ( !r );
    if ( r == -1 )
        exit(0);
    return c;
}

/* ------------------------------------------------------------------------ */
void Console::init( FILE * in, FILE * out) {
    //int fd = fileno(in);
    //int flags = fcntl(fd, F_GETFL);
    //flags |= O_NONBLOCK;
    //if ( fcntl(fd, F_SETFL, flags) < 0 ) {
    //    puts("fcntl failed");
    //    exit(1);
    //}
    _in = in;
    _out = out;
	_x = _y = -1;
	_colored = 1;
	_color = white;
	_bgr = _bold = 0;
    _mode = TTY_MODE_ANSI;
    _mode_detected = false;
    unsigned long t = io_now() + 2000;
    while ( GT_ULONG(t,io_now()) );             // delay for slow terminal emulators
}

/* ------------------------------------------------------------------------ */
void Console::colors_enable(int yes)
{
	_colored = yes ? 1 : 0;
}

/* ------------------------------------------------------------------------ */
void Console::free()
{
	uncol();
}


/* ------------------------------------------------------------------------ */
int Console::xy(int x, int y)
{
    int rc;
    if ( _mode == TTY_MODE_ANSI ) {
        rc = put("\033[%d;%dH", _y = y, _x = x);
    }
    else {
        rc = put("\033Y%c%c", ' ' - 1 + (_y = y), ' ' - 1 + (_x = x));
    }
    return rc;
}

/* ------------------------------------------------------------------------ */
int Console::lf()
{
	return xy(_x, _y + 1);
}

/* ------------------------------------------------------------------------ */
int Console::read( char *buf, unsigned int size, bool& overrun, int timeout) {
    int i = 0; 
    bool wait = true;
    int c;
    
    if ( !size )
        return -1;
    
    while ( wait && (i < size) ) {
        wait = false;
        unsigned long t = io_now() + (unsigned long)timeout;        // timeout
        unsigned long now;
        while ( GT_ULONG(t,(now = io_now())) ) {
            DPRINT("%ld %ld\n",t,now);
            if ( _get_ch(&c) > 0 ) {
                DPRINT("%02x %c\n",c,c);
                if ( buf )
                    buf[i] = c;
                i++;
                wait = true;
                break;
            }
        }
        DPRINT("?%ld %ld\n",t,now);
    }

    overrun = wait ? true : false;
    return i;
}

/* ------------------------------------------------------------------------ */
void Console::exit_not_compatible() {
    put_str("Your terminal is not compatible with the program\n");
    exit(0);
}

/* ------------------------------------------------------------------------ */
void Console::detect() {
    termid();
    if ( TTY_MODE_NOTCOMPATIBLE == _mode ) {
        exit_not_compatible();
    }
    else if ( TTY_MODE_UNKNOWN == _mode ) {
        DPRINT("Assigned ANSI by default\n");
        _mode = TTY_MODE_ANSI;
    }
}

/* ------------------------------------------------------------------------ */
void Console::termid() {
    char buf[16];   
    bool oldterm = false;                       // Old fashion terminal detected
    bool newterm = false;                       // New fashion terminal detected
    bool vt52compat = false;                    // VT100 in VT52 compatible mode detected, possible another terminal type, not VT100 (15IE...)
    bool overrun;
    char *s, *fnd;
    int len, param[16], par, npar, id;    
    
    _mode = TTY_MODE_UNKNOWN;
    _terminal_name = "UNKNOWN";
    
/* 
      3                                 ;       THE GENERAL FORMAT FOR OLD  3-CHARACTER ESCAPE SEQUENCE IS:
      4                                 ;
      5                                 ;                       <ESC> < / > + <UNIQUE CHARACTER> ,
      6                                 ;
      7                                 ;       WITH EXCEPTION FOR SPECIAL CASE OF VT71 WITH ADDITIOANAL INFORMATION
      8                                 ;       SET THROUGH JUMPERS OR THUMBWHEELS. FOR THIS CASE FORMAT WILL BE:
      9                                 ;
     10                                 ;                       <ESC> < / > N N N ~     , WHERE N IS 0 TO 7.
     11                                 ;
     12                                 ;       ONCE TERMID HAS RECOGNIZED YOUR TERMINAL AS AN OLDER TYPE, IT DETER-
     13                                 ;       MINES THE SPECIFIC MODEL BASED ON THE THIRD CHARACTER WHICH IS DISTIN-
     14                                 ;       QUISHING IN THIS CASE.
*/
    put_str("\033Z");                        // We transmit ESC Z for old fashion terminal type

    len = read(buf,sizeof(buf)-1,overrun,1000);  // timeout for slow terminal emulators
    buf[len] = '\0';
    DPRINT("received (on ESC Z): %s\n",buf);
    
    if ( overrun || len && (buf[0] != '\033') ) {
        DPRINT("NOTCOMPATIBLE terminal detected\n");
        _mode = TTY_MODE_NOTCOMPATIBLE;
        return;
    }
    
    do {
/*
     10 000200                                  TERM    <'A>,<VT50>                     ;ESC /A IS VT50
     11 000204                                  TERM    <'B>,<VT50>                     ;ESC /B IS VT50 W/COPIER
     12 000210                                  TERM    <'C>,<VT55>                     ;ESC /C IS A VT55
     13 000214                                  TERM    <'E>,<VT55>                     ;ESC /E IS A VT55 UC/LC
     14 000220                                  TERM    <'H>,<VT50H>                    ;ESC /H IS A VT50H
     15 000224                                  TERM    <'J>,<VT50H WITH COPIER>        ;ESC /J IS A VT50H W/COPIER
     16 000230                                  TERM    <'K>,<VT52>
     17 000234                                  TERM    <'L>,<VT52 WITH COPIER>         ;ESC /K IS A VT52
     18 000240                                  TERM    <'M>,<VT52 WITH PRINTER>        ;ESC /L IS A VT52 W/COPIER
     19 000244                                  TERM    <'Q>,<VT52J>                    ;ESC /M IS VT52 W/PRINTER
     20 000250                                  TERM    <'R>,<VT52J WITH COPIER>        ;ESC /Q IS VT52J 8 BIT
     21 000254                                  TERM    <'S>,<VT52J WITH COPIER>        ;ESC /R IS VT52J 7 BIT
     22 000260                                  TERM    <'T>,<VT52J WITH COPIER>        ;ESC /S IS VT52J 8BIT W/COPIER
     23 000264                                  TERM    <'Z>,<VT100 IN VT52 MODE>,VT100=YES ;ESC /Z VT1__ RUNNING AS VT
     24 000270                                  TERM    <'`>,<VT61>                     ;ESC /` IS A VT61
     25 000274                                  TERM    <'A>,<VT61 WITH COPIER>         ;ESC /A IS A VT61 W/COPIER
     26 000300                                  TERM    <'B>,<VT61 WITH PRINTER>        ;ESC /B IS VT61 W/PRINTER
     27 000304                                  TERM    <'C>,<VT61 WITH COPIER AND PRINTER> ;ESC /C IS VT61 W/COPIER&PRNT
     28 000310                                  TERM    <'~>,<VT71>                     ;ESC /~ IS VT71
     29 000314  000000                          .WORD   0                               ;END OF OLD FASHIONED LIST
*/
        s = buf;
        if ( *s++ != '\033' ) break;
        if ( *s++ != '/' ) break;
        static const char* old_terminal_codes = 
            "ABCEHJKLMQRSTZ`~";
        fnd = strchr(old_terminal_codes,*s);
        if ( !fnd ) {
/*            
     29                                 ; IF NOT FOUND, CHECK IF IT IS SPECIAL CASE OF VT71
     30                                 ; RESPONSE FORMAT FOR SPECIAL CASE OF VT71  IS < ESC / N N N ~ >
*/
            if ( isodigit(*s++) && isodigit(*s++) && (isodigit(*s++) && ('~' == *s++)) ) {
                _terminal_name = "VT71";
                _mode == TTY_MODE_NOTCOMPATIBLE;
                oldterm = true;
            }
            break;
        }
        id = fnd - old_terminal_codes;
        static const char *old_terminal_names[] = {
            "VT50",
            "VT50",
            "VT55",
            "VT55",
            "VT50H",
            "VT50H WITH COPIER",
            "VT52",
            "VT52 WITH COPIER",
            "VT52 WITH PRINTER",
            "VT52J",
            "VT52J WITH COPIER",
            "VT52J WITH COPIER",
            "VT52J WITH COPIER",
            "VT100 IN VT52 MODE",
            "VT61",
            "VT71",
        };
        _terminal_name = old_terminal_names[id];
        oldterm = true;                                 // Old fashion terminal detected 
        switch (id) {
        case 0:
        case 1:
        case 4:
            _mode = TTY_MODE_NOTCOMPATIBLE;            // VT50
            break;
        case 2:
        case 3:
            _mode = TTY_MODE_VT52;                     // VT55
            break;
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            _mode = TTY_MODE_VT52;
            break;
        case 13:
            _mode = TTY_MODE_VT52;                     // VT100 in VT52 mode
            vt52compat = true;
            break;
        case 14:
            _mode = TTY_MODE_NOTCOMPATIBLE;            // VT61
            break;
        case 15:
            _mode = TTY_MODE_NOTCOMPATIBLE;            // VT71
            break;
        }
    } while (0);
    
    if ( !oldterm ) {
/*        
      3                                 ;
      4                                 ;       FOR NEWER DEVICES, THE PRIMARY DEVICE ATTRIBUTE CONTROL IS USED TO
      5                                 ;       REQUEST OR REPORT THE IDENTIFICATION CODE ASSOCIATED WITH THE FAMI-
      6                                 ;       LY OF DEVICES IN WHICH THE INTERFACE IS IMPLEMENTED.
      7                                 ;       THE RESPONSE FOR THE PRIMARY DA REQUEST CONSISTS OF A DA CONTROL
      8                                 ;       SEQUENCE USING A PRIVATE PARAMETER STRING WITH THE INTRODUCER
      9                                 ;       FOLLOWED BY A VARIABLE NUMBER OF PARAMETERS.
     10                                 ;
     11                                 ;       THE GENERAL FORMAT FOR RESPONSE OF  NEWER DEVICES:
     12                                 ;
     13                                 ;               <ESC> < [ > < ? > < PS1 > < ; > < PS2 > < C > , WHERE
     14                                 ;
     15                                 ;       <ESC>[          DA CONTROL SEQUENCE
     16                                 ;       <?>             INTRODUCER
     17                                 ;       PS1, PS2        PARAMETERS (ONLY NUMBERS ARE LEGAL)
     18                                 ;       <;>             DELIMITER
     19                                 ;       <C>             TERMINATOR
     20                                 ;
     21                                 ;       ONCE TERMID DETERMINES YOUR TERMINAL IS A NEWER DEVICE, IT STARTS TO
     22                                 ;       COLLECT THE REST OF PARAMETERS. THE CHARACTERS UNTIL THE FIRST ";"
     23                                 ;       ARE TRANSFORMED AND ACCUMULATED INTO THE DECIMAL NUMBER. THIS NUMBER
     24                                 ;       IS USED IN THE TABLE SEARCH TO FIND THE MATCH WITH HARD CODED STANDARD
     25                                 ;       RESPONSES.
     26                                 ;       EXCEPTION: VT102J AND LA12 BOTH HAVE DA PRIMARY ID 15. TO DISTIN-
     27                                 ;       QUISH BETWEEN THESE TWO, THE SECOND OPTIONAL PARAMETER AFTER TRAILING
     28                                 ;       SEMICOLON HAS TO BE TESTED. ONLY LA12 HAS SECOND PARAMETER; "0" FOR
     29                                 ;       STANDARD LA12 OR "2" FOR LA12 ENHANCEMENT.
     30                                 ;
     31                                 ;       ANY TERMINAL THAT REPLIED WITH  "61" FOR THE  FIRST PARAMETER IS
     32                                 ;       CONSIDERED AS  VT1XX CLASS.
     33                                 ;       ANY TERMINAL THAT REPLIED WITH  "62" IS CONSIDERED AS VT2XX CLASS.
     34                                 ;       ANY TERMINAL THAT REPLIED WITH  "63" IS CONSIDERED AS VT3XX CLASS.
     35                                 ;
*/
        do {
            s = buf;
            if ( *s++ != '\033' ) break;                        // the response is not the one that was expected
            if ( *s++ != '[' )  break;
            if ( *s++ != '?' )  break;
            for ( npar = 0; npar < 16; ) {
                if ( !isdigit(*s) )
                    break;
                param[npar++] = strtonum(s,&s);
                if ( *s != ';' )
                    break;
                ++s;
            }
            if ( *s != 'c' ) break;
            if ( npar < 1 ) break;

            par = param[0];
/*
     10 000316                                  TERM    <1.>,<VT100>,VT100=YES          ;VT100 (MAY BE SUPPORTED AS VT102)
     11 000322                                  TERM    <2.>,<LA120>                    ; (LA120 UNSUPPORTED)
     12 000326                                  TERM    <3.>,<LA34>                     ;LA34/38
     13 000332                                  TERM    <4.>,<VT132>,VT100=YES          ;VT132  SUPPORTED AS VT102
     14 000336                                  TERM    <5.>,<VK100>                    ;VK100   UNSUPPORTED
     15 000342                                  TERM    <6.>,<VT102>,VT100=YES          ;VT102
     16 000346                                  TERM    <7.>,<VT131>,VT100=YES          ;VT131
     17 000352                                  TERM    <8.>,<VT278>                    ;VT278
     18 000356                                  TERM    <9.>,<LQPSE>                    ;LQPSE
     19 000362                                  TERM    <10.>,<LA100>                   ;LA100
     20 000366                                  TERM    <11.>,<LA120J>                  ;LA120J
     21 000372                                  TERM    <12.>,<VT125>,VT100=YES         ;VT125
     22 000376                                  TERM    <13.>,<LQP02>                   ;LQP02
     23                                                                                 ;14 UNUSED
     24 000402                                  TERM    <15.>,<VT102J>,VT100=YES        ;VT102J/LA12
     25 000406                                  TERM    <61.>,<VT100 CLASS>,VT100=YES
     26 000412                                  TERM    <62.>,<VT200 CLASS>,VT100=YES
     27 000416                                  TERM    <63.>,<VT300 CLASS>,VT100=YES
     28 000422                                  TERM    <64.>,<VT400 CLASS>,VT100=YES
     29 000426  000000                          .WORD   0
*/
            newterm = true;                             // New fashion terminal detected 
            if ( par < 16 ) {
                if ( 0 < par && par < 14 ) {
                    static const char *new_terminal_names[] = { 
                        "VT100",
                        "LA120",
                        "LA34",
                        "VT132",
                        "VK100",
                        "VT102",
                        "VT131",
                        "VT278",
                        "LQPSE",
                        "LA100",
                        "LA120J",
                        "VT125",
                        "LQP02",
                    };
                    _terminal_name = new_terminal_names[par];
                    if ( (1 << par) & ((1 << 1) | (1 << 4) | (1 << 6) | (1 << 7) | (1 << 12)) ) {
                        // Video terminal
                        _mode == TTY_MODE_ANSI;
                        break;
                    }
                    if ( (1 << par) & ((1 << 2) | (1 << 3) | (1 << 5) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 13)) ) {
                        // Printer etc. (or computer VT278)
                        _mode == TTY_MODE_NOTCOMPATIBLE;
                        break;
                    }
                }
                if ( par == 14 )
                    break;
                if ( par == 15 ) {
                    if ( npar < 2 ) break;
                    if ( param[1] == 0 || param[1] == 2 ) {
                        // LA12
                        _terminal_name = "LA12";
                        _mode == TTY_MODE_NOTCOMPATIBLE;
                    }
                    else {
                        // VT102J
                        _terminal_name = "VT102J";
                        _mode == TTY_MODE_ANSI;
                    }
                    break;
                }
            }
            else {
                if ( (61 <= par) && (par <= 64) ) {
                    static const char *tnames[] = {
                        "VT1XX class", "VT2XX class", "VT3XX class", "VT4XX class"
                    };
                    _terminal_name = tnames[par - 61];
                    _mode == TTY_MODE_ANSI;
                }
                break;
            }
        } while(0);
    }
    
    // The original termid algorithm doesn't contain folowed logic.
    // This part searches for ANSI terminals if there was no correct response to the ESC Z request
    // or there was an ESC/Z response. We assume ESC/Z response may be in case of a real VT100 compatible terminal
    // in VT52 mode or a VT52 compatible terminal.    
    // We just try ANSI "Cursor Position Report" ( ESC [ 6 n ) command, waiting ANSI terminal reports
    // current cursor coordinates. VT52 compatible terminal should recognize ESC [ command
    // (enable HOLD-SCREEN mode), folowed by usual 6 n characters, and not respond.
    
    if ( (oldterm && vt52compat) || (newterm && (TTY_MODE_UNKNOWN == _mode)) || (!oldterm && !newterm) ) {
        // second attempt. 
        if ( oldterm )
            // If VT100 in VT52 compatible mode detected,
            // then send setansi command (enter to ANSI mode).
            // If not compatible enough with the original VT100 terminal
            // does not recognize the command, it simply ignores.
            put_str("\033<");                                   

        put_str("\033[6n");     // send ANSI "Cursor Position Report" command,
                                // expecting "ESC[n;mR", where n is the row and m is the column.
    
        len = read(buf,sizeof(buf)-1,overrun);
        buf[len] = '\0';
        DPRINT("received (on ESC [6n): %s\n",buf);
    
        if ( len ) {
            // There was response
            do {
                if ( overrun ) break;                           // not ESC, the response is not the one that was expected  
                s = buf;
                if ( *s++ != '\033' ) break;
                if ( *s++ != '[' ) break;
                for ( npar = 0; npar < 16; ) {
                    if ( !isdigit(*s) )
                        break;
                    param[npar++] = strtonum(s,&s);
                    if ( *s != ';' )
                        break;
                    ++s;
                }
                if ( *s != 'R' ) break;                         // not 'R', the response is not the one that was expected
                if ( npar != 2 ) break;                         // number of parameters not 2, reply not expected
                _terminal_name = "ANSI compatible";
                _mode = TTY_MODE_ANSI;                          // set the ANSI type, even if it is already VT52
                newterm = true;
            } while (0);
            
            if ( oldterm && !newterm ) {
                // If VT100 in VT52 compatible mode detected.
                // And something went wrong, the terminal responsed an incorrect character sequence.
                // Then send setansi command (exit from ANSI mode).
                // If not compatible enough with the original VT100 terminal
                // does not recognize the command, it simply ignores
                put_str("\033<");   
            }
        }
        else {
            // There was no response.
            if ( oldterm )
            // We'd better return the VT52 compatible terminal from HOLD_SCREEN mode.
                put_str("\033\\");
        }
    }
       
    if ( oldterm || newterm )
        DPRINT("%s terminal detected\n",_terminal_name);
    else
        DPRINT("VT52 or ANSI compatible terminal not detected\n");
}

/* ------------------------------------------------------------------------ */
int Console::cls()
{
    int rc;
    if ( !_mode_detected ) {
        detect();
        _mode_detected = true;
    }    
    if ( _mode == TTY_MODE_ANSI ) {
        rc = put_str("\033[2J");
    }
    else {
        xy(1,1);
        rc = put_str("\033J");
    }
    return rc;
}

/* ------------------------------------------------------------------------ */
int Console::color(int c)
{
    int rc;
    if ( _mode == TTY_MODE_ANSI ) {
        rc = c == _color || !_colored ? 0 : put("\033[3%dm", _color = c);
    }
    else {
        rc = 0;
    }
    return rc;
}

/* ------------------------------------------------------------------------ */
int Console::bold()
{
    int rc;
    if ( _mode == TTY_MODE_ANSI ) {
        rc = _bold || !_colored ? 0 : (_bold = 1, put_str("\033[1m"));
    }
    else {
        rc = 0;
    }
    return rc;
}

/* ------------------------------------------------------------------------ */
int Console::uncol()
{
    int rc;
    if ( _mode == TTY_MODE_ANSI ) {
        if (_colored && (_color != 7 || _bgr || _bold)) {
            _color = 7; _bgr = 0; _bold = 0;
            rc = put_str("\033[0m");
        }
        else
            rc = 0;
    }
    else {
        rc = 0;
    }
    return rc;
}


/* ------------------------------------------------------------------------ */
int Console::bgr(int c)
{
    int rc;
    if ( _mode == TTY_MODE_ANSI ) {
        rc = c == _bgr || !_colored ? 0 : put("\033[4%dm", _bgr = c);
    }
    else {
        rc = 0;
    }
    return rc;
}

/* ------------------------------------------------------------------------ */
int Console::unbold()
{
    int rc;
    if ( _mode == TTY_MODE_ANSI ) {
        if (!_colored || !_bold) {
            rc = 0;
        }
        else {
            int col = _color, bg = _bgr;
            rc = uncol() + color(col) + bgr(bg);
        }
    }
    else {
        rc = 0;
    }
    return rc;
}


/* ------------------------------------------------------------------------ */
int Console::show_cursor(int yes)
{
    int rc;
    if ( _mode == TTY_MODE_ANSI ) {
        rc = put("\033[?25%c", yes ? 'h' : 'l');
    }
    else {
        rc = 0;
    }
    return rc;
}

/* ------------------------------------------------------------------------ */
void Console::box(int x, int y, int clr, char const *text)
{
	xy(x, y);
	color(clr);

	for (char const *i = text; *i; ++i)
		switch (*i) {
		case '\n':
			xy(x, ++y);
			break;
		case '[':
			bold();
			put_char(*i);
			break;
		case ']':
            put_char(*i);
			unbold();
			break;
		default:
			put_char(*i);
		}
	uncol();
}

/* ------------------------------------------------------------------------ */
const char *ansi_escapes[] = {
    "[A",
    "[B",
    "[C",
    "[D",
    0
};

const char *vt52_escapes[] = {
    "A",
    "B",
    "C",
    "D",
    0
};

int Console::check_reply( const char *escapes[], int timeout) {
    unsigned int bits = 0;
    
    for ( int i = 0; escapes[i]; ++i)
        bits |= (1 << i);
    
    int offset = 0;
    int found = -1;
    bool wait = true;
    while ( bits && wait ) {
        wait = false;
        unsigned long t = io_now() + (unsigned long)timeout;        // timeout
        while ( GT_ULONG(t,io_now()) ) {
            int c;
            if ( _get_ch(&c) > 0 ) {
                for ( int i = 0; found < 0 && escapes[i]; ++i) {
                    if ( bits & (1 << i) ) {
                        if ( c == escapes[i][offset] ) {
                            if ( !escapes[i][offset + 1] )
                                found = i;
                        }
                        else {
                            bits &= ~(1 << i);
                        }
                    }
                }
                offset++;
                wait = true;
                break;
            }
        }
    }        
    return found;
}

/* ------------------------------------------------------------------------ */
int Console::check_event() {
    int c;
    if ( _get_ch(&c) > 0 ) {
        switch (c) {
        case 'a':
        case 'A':
            return KEY_LEFT;
        case 'd':
        case 'D':
            return KEY_RIGHT;
        case 'w':
        case 'W':
            return KEY_UP;
        case 's':
        case 'S':
            return KEY_DOWN;
        case ' ':
            return KEY_DROP;
        case 'p':
        case 'P':
            return KEY_PAUSE;
        case 'q':
        case 'Q':
            return KEY_QUIT;
        case 27: {              // ESC
            const char **escapes = _mode == TTY_MODE_ANSI ? ansi_escapes : vt52_escapes;
            int esc = check_reply(escapes);
            switch (esc) {
                case 0:
                    return KEY_DOWN;
                case 1:
                    return KEY_UP;
                case 2:
                    return KEY_RIGHT;
                case 3:
                    return KEY_LEFT;
            }
        }}            
    }
    return KEY_NONE;
}

/* ------------------------------------------------------------------------ */
int Console::dock()
{
    int rc;
    if ( _mode == TTY_MODE_VT52 ) {
        rc = xy(1,1);
    }
    else {
        rc = 0;
    }
    return rc;
}

/* ------------------------------------------------------------------------ */
void Console::flush() {}
