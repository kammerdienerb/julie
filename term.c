#include <julie.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <termios.h>


static struct termios save_term;


#define TERM_BLACK                   "\033[0;30m"
#define TERM_BLUE                    "\033[0;34m"
#define TERM_GREEN                   "\033[0;32m"
#define TERM_CYAN                    "\033[0;36m"
#define TERM_RED                     "\033[0;31m"
#define TERM_PURPLE                  "\033[0;35m"
#define TERM_BROWN                   "\033[0;33m"
#define TERM_GRAY                    "\033[0;37m"
#define TERM_DARK_GRAY               "\033[1;30m"
#define TERM_LIGHT_BLUE              "\033[1;34m"
#define TERM_LIGHT_GREEN             "\033[1;32m"
#define TERM_LIGHT_CYAN              "\033[1;36m"
#define TERM_LIGHT_RED               "\033[1;31m"
#define TERM_LIGHT_PURPLE            "\033[1;35m"
#define TERM_YELLOW                  "\033[1;33m"
#define TERM_WHITE                   "\033[1;37m"
#define TERM_BG_BLACK                "\033[0;40m"
#define TERM_BG_BLUE                 "\033[0;44m"
#define TERM_BG_GREEN                "\033[0;42m"
#define TERM_BG_CYAN                 "\033[0;46m"
#define TERM_BG_RED                  "\033[0;41m"
#define TERM_BG_PURPLE               "\033[0;45m"
#define TERM_BG_GREY                 "\033[0;47m"
#define TERM_BG_WHITE                "\033[1;47m"
#define TERM_INVERSE                 "\033[7m"
#define TERM_SAVE                    "\0337"
#define TERM_RESTORE                 "\0338"
#define TERM_RESET                   "\033[0m"

#define TERM_ALT_SCREEN              "\033[?1049h"
#define TERM_STD_SCREEN              "\033[?1049l"
#define TERM_CLEAR_SCREEN            "\033[2J"
#define TERM_CLEAR_LINE_L            "\033[1K"
#define TERM_CLEAR_LINE_R            "\033[0K"
#define TERM_CLEAR_LINE              "\033[2K"
#define TERM_SCROLL_UP               "\033[1U"
#define TERM_SCROLL_DOWN             "\033[1S"

#define TERM_CURSOR_HOME             "\033[H"
#define TERM_CURSOR_HIDE             "\033[?25l"
#define TERM_CURSOR_SHOW             "\033[?25h"
#define TERM_CURSOR_MOVE_BEG         "\033["
#define TERM_CURSOR_MOVE_SEP         ";"
#define TERM_CURSOR_MOVE_END         "H"
#define TERM_ENABLE_BRACKETED_PASTE  "\033[?2004h"
#define TERM_DISABLE_BRACKETED_PASTE "\033[?2004l"

#define TERM_MOUSE_BUTTON_ENABLE     "\033[?1002h"
#define TERM_MOUSE_BUTTON_DISABLE    "\033[?1002l"
#define TERM_MOUSE_ANY_ENABLE        "\033[?1003h"
#define TERM_MOUSE_ANY_DISABLE       "\033[?1003l"
#define TERM_SGR_1006_ENABLE         "\033[?1006h"
#define TERM_SGR_1006_DISABLE        "\033[?1006l"

#define TERM_DEFAULT_READ_TIMEOUT (3)

enum {
    KEY_NULL  = 0,    /* NULL      */
    CTRL_A    = 1,    /* Ctrl-a    */
    CTRL_B    = 2,    /* Ctrl-b    */
    CTRL_C    = 3,    /* Ctrl-c    */
    CTRL_D    = 4,    /* Ctrl-d    */
    CTRL_E    = 5,    /* Ctrl-e    */
    CTRL_F    = 6,    /* Ctrl-f    */
    CTRL_G    = 7,    /* Ctrl-g    */
    CTRL_H    = 8,    /* Ctrl-h    */
    TAB       = 9,    /* Tab       */
    CTRL_J    = 10,   /* Ctrl-j    */
    NEWLINE   = 10,   /* Newline   */
    CTRL_K    = 11,   /* Ctrl-k    */
    CTRL_L    = 12,   /* Ctrl-l    */
    ENTER     = 13,   /* Enter     */
    CTRL_N    = 14,   /* Ctrl-n    */
    CTRL_O    = 15,   /* Ctrl-o    */
    CTRL_P    = 16,   /* Ctrl-p    */
    CTRL_Q    = 17,   /* Ctrl-q    */
    CTRL_R    = 18,   /* Ctrl-r    */
    CTRL_S    = 19,   /* Ctrl-s    */
    CTRL_T    = 20,   /* Ctrl-t    */
    CTRL_U    = 21,   /* Ctrl-u    */
    CTRL_V    = 22,   /* Ctrl-v    */
    CTRL_W    = 23,   /* Ctrl-w    */
    CTRL_X    = 24,   /* Ctrl-x    */
    CTRL_Y    = 25,   /* Ctrl-y    */
    CTRL_Z    = 26,   /* Ctrl-z    */
    ESC       = 27,   /* Escape    */
    CTRL_FS   = 31,   /* Ctrl-/    */
    BACKSPACE = 127,  /* Backspace */

    ASCII_KEY_MAX = 256,

    /* The following are just soft codes, not really reported by the
     * terminal directly. */
    ARROW_LEFT = 300,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    CTRL_ARROW_LEFT,
    CTRL_ARROW_RIGHT,
    CTRL_ARROW_UP,
    CTRL_ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN,
    SHIFT_TAB,
    FN1 = 330,
    FN2 = 331,
    FN3 = 332,
    FN4 = 333,
    FN5 = 334,
    FN6 = 335,
    FN7 = 336,
    FN8 = 337,
    FN9 = 338,
    FN10 = 339,
    FN11 = 340,
    FN12 = 341,
    MENU_KEY,
};

#define CTRL_KEY(c) ((c) & 0x9F)

#define IS_ARROW(k) ((k) >= ARROW_LEFT && (k) <= ARROW_DOWN)

#define MOUSE_PRESS        (0)
#define MOUSE_RELEASE      (1)
#define MOUSE_DRAG         (2)
#define MOUSE_OVER         (3)

#define MOUSE_BUTTON_LEFT   (0)
#define MOUSE_BUTTON_MIDDLE (1)
#define MOUSE_BUTTON_RIGHT  (2)
#define MOUSE_WHEEL_UP      (3)
#define MOUSE_WHEEL_DOWN    (4)

#define IS_MOUSE(k) ((k) < 0)

#define MOUSE_KIND(k)   (((k) >> 28) & 0x7)
#define MOUSE_BUTTON(k) (((k) >> 24) & 0xf)
#define MOUSE_ROW(k)    (((k) >> 12) & 0xfff)
#define MOUSE_COL(k)    (((k) >> 0)  & 0xfff)

#define MK_MOUSE(k, b, r, c) \
    (((((k) & 0x7)   << 28) \
    | (((b) & 0xf)   << 24) \
    | (((r) & 0xfff) << 12) \
    | (((c) & 0xfff) << 0)) \
    | 0x80000000)


int s_to_i(const char *s) {
    int i;

    sscanf(s, "%d", &i);

    return i;
}

static int esc_timeout(int *input) {
    char c;

    /* input[0] is ESC */

    if (read(0, &c, 1) == 0) {
        return 1;
    }
    input[1] = c;

    if (input[1] != '['
    &&  input[1] != 'O'
    &&  input[1] != ESC) {
        return 2;
    }

    if (read(0, &c, 1) == 0) {
        return 2;
    }
    input[2] = c;

    return 3;
}

static int esc_sequence(int *input) {
    char c;
    char buff[64];
    int  i;
    int  k;
    int  b;
    int  x;
    int  y;

    /* the input length is 3 */
    /* input[0] is ESC */

    if (input[1] == '[') { /* ESC [ sequences. */
        if (input[2] >= '0' && input[2] <= '9') {
            /* Extended escape, read additional byte. */
            if (read(0, &c, 1) == 0) {
                return 3;
            } else if (input[2] == '1') {
                input[3] = c;
                if (c == '~') {
                    input[0] = HOME_KEY;
                    return 1;
                } else if (c == ';') {
                    if (read(0, &c, 1) == 0) { return 4; }
                    input[4] = c;
                    if (c == '3') {
                        if (read(0, &c, 1) == 0) { return 5; }
                        input[5] = c;
                        switch (c) {
                            case 'A':
                                input[1] = ARROW_UP;
                                return 2;
                            case 'B':
                                input[1] = ARROW_DOWN;
                                return 2;
                            case 'C':
                                input[1] = ARROW_RIGHT;
                                return 2;
                            case 'D':
                                input[1] = ARROW_LEFT;
                                return 2;
                        }
                        return 6;
                    } else if (c == '5') {
                        if (read(0, &c, 1) == 0) { return 5; }
                        input[5] = c;
                        switch (c) {
                            case 'A':
                                input[1] = CTRL_ARROW_UP;
                                return 2;
                            case 'B':
                                input[1] = CTRL_ARROW_DOWN;
                                return 2;
                            case 'C':
                                input[1] = CTRL_ARROW_RIGHT;
                                return 2;
                            case 'D':
                                input[1] = CTRL_ARROW_LEFT;
                                return 2;
                        }
                        return 6;
                    }
                    return 5;
                } else if (c == '5') {
                    if (read(0, &c, 1) == 0) { return 4; }
                    input[4] = c;
                    if (c == '~') {
                        input[0] = FN5;
                        return 1;
                    }
                    return 5;
                } else if (c == '7') {
                    if (read(0, &c, 1) == 0) { return 4; }
                    input[4] = c;
                    if (c == '~') {
                        input[0] = FN6;
                        return 1;
                    }
                    return 5;
                } else if (c == '8') {
                    if (read(0, &c, 1) == 0) { return 4; }
                    input[4] = c;
                    if (c == '~') {
                        input[0] = FN7;
                        return 1;
                    }
                    return 5;
                } else if (c == '9') {
                    if (read(0, &c, 1) == 0) { return 4; }
                    input[4] = c;
                    if (c == '~') {
                        input[0] = FN8;
                        return 1;
                    }
                    return 5;
                }
                return 4;
            } else if (input[2] == '2') {
                if (c == '0') {
                    input[3] = c;

                    if (read(0, &c, 1) == 0) { return 4; }
                    input[4] = c;

                    if (c == '~') {
                        input[0] = FN9;
                        return 1;
                    }
                    return 5;
                } else if (c == '1') {
                    input[3] = c;
                    if (read(0, &c, 1) == 0) { return 4; }
                    input[4] = c;
                    if (c == '~') {
                        input[0] = FN10;
                        return 1;
                    }
                    return 5;
                } else if (c == '3') {
                    input[3] = c;
                    if (read(0, &c, 1) == 0) { return 4; }
                    input[4] = c;
                    if (c == '~') {
                        input[0] = FN11;
                        return 1;
                    }
                    return 5;
                } else if (c == '4') {
                    input[3] = c;
                    if (read(0, &c, 1) == 0) { return 4; }
                    input[4] = c;
                    if (c == '~') {
                        input[0] = FN12;
                        return 1;
                    }
                    return 5;
                }
                return 4;
            } else if (c == '~') {
                switch (input[2]) {
                    case '1':    { input[0] = HOME_KEY;  break; }
                    case '3':    { input[0] = DEL_KEY;   break; }
                    case '4':    { input[0] = END_KEY;   break; }
                    case '5':    { input[0] = PAGE_UP;   break; }
                    case '6':    { input[0] = PAGE_DOWN; break; }
                }
                return 1;
            } else if (input[2] == '5') {
                if (c == '7') {
                    input[3] = c;

                    if (read(0, &c, 1) == 0) { return 4; }
                    input[4] = c;
                    if (c == '3') {
                        if (read(0, &c, 1) == 0) { return 5; }
                        input[5] = c;
                        if (c == '6') {
                            if (read(0, &c, 1) == 0) { return 6; }
                            input[6] = c;
                            if (c == '3') {
                                if (read(0, &c, 1) == 0) { return 7; }
                                input[7] = c;
                                if (c == 'u') {
                                    input[0] = MENU_KEY;
                                    return 1;
                                }
                                return 8;
                            }
                            return 7;
                        }

                        return 6;
                    }
                    return 5;
                }
                return 4;
            }
        } else {
            switch (input[2]) {
                case 'A':    { input[0] = ARROW_UP;    break; }
                case 'B':    { input[0] = ARROW_DOWN;  break; }
                case 'C':    { input[0] = ARROW_RIGHT; break; }
                case 'D':    { input[0] = ARROW_LEFT;  break; }
                case 'H':    { input[0] = HOME_KEY;    break; }
                case 'F':    { input[0] = END_KEY;     break; }
                case 'P':    { input[0] = DEL_KEY;     break; }
                case 'Z':    { input[0] = SHIFT_TAB;   break; }
                case '<':    {
                    k = 0;

                    memset(buff, 0, sizeof(buff));
                    for (i = 0; read(0, &c, 1) && c != ';'; i += 1) { buff[i] = c; }
                    buff[i] = 0;
                    b = s_to_i(buff);

                    if (b >= 64) {
                        b = MOUSE_WHEEL_UP + (b - 64);
                    } else if (b >= 32) {
                        k  = MOUSE_DRAG;
                        b -= 32;
                    }

                    memset(buff, 0, sizeof(buff));
                    for (i = 0; read(0, &c, 1) && c != ';'; i += 1) { buff[i] = c; }
                    buff[i] = 0;
                    x = s_to_i(buff);

                    memset(buff, 0, sizeof(buff));
                    for (i = 0; read(0, &c, 1) && toupper(c) != 'M'; i += 1) { buff[i] = c; }
                    buff[i] = 0;
                    y = s_to_i(buff);

                    if (k != MOUSE_DRAG) {
                        k = (c == 'M') ? MOUSE_PRESS : MOUSE_RELEASE;
                    }

                    input[0] = MK_MOUSE(k, b, y, x);

                    break;
                }
            }
            return 1;
        }
    } else if (input[1] == 'O') { /* ESC O sequences. */
        switch (input[2]) {
            case 'A':    { input[0] = ARROW_UP;   break; }
            case 'B':    { input[0] = ARROW_DOWN; break; }
            case 'H':    { input[0] = HOME_KEY;   break; }
            case 'F':    { input[0] = END_KEY;    break; }
            case 'P':    { input[0] = FN1;        break; }
            case 'Q':    { input[0] = FN2;        break; }
            case 'R':    { input[0] = FN3;        break; }
            case 'S':    { input[0] = FN4;        break; }
        }
        return 1;
    }

    if (input[1] == ESC) {
        if (read(0, &c, 1)) {
            input[3] = c;
            if (input[2] == ESC && input[3] == ESC) { return 4; }
            return 1 + esc_sequence(input + 1);
        }
    }

    return 3;
}

char *key_to_string(int key) {
    char key_buff[16];

    switch (key) {
        case CTRL_A:
        case CTRL_B:
        case CTRL_C:
        case CTRL_D:
        case CTRL_E:
        case CTRL_F:
        case CTRL_G:
        case CTRL_H:
        case CTRL_J:
        case CTRL_K:
        case CTRL_L:
        case CTRL_N:
        case CTRL_O:
        case CTRL_P:
        case CTRL_Q:
        case CTRL_R:
        case CTRL_S:
        case CTRL_T:
        case CTRL_U:
        case CTRL_V:
        case CTRL_W:
        case CTRL_X:
        case CTRL_Y:
        case CTRL_Z:
            snprintf(key_buff, sizeof(key_buff), "ctrl-%c", 'a' + (key - CTRL_A));
            break;

        case CTRL_ARROW_LEFT:
            snprintf(key_buff, sizeof(key_buff), "ctrl-left");
            break;
        case CTRL_ARROW_RIGHT:
            snprintf(key_buff, sizeof(key_buff), "ctrl-right");
            break;
        case CTRL_ARROW_UP:
            snprintf(key_buff, sizeof(key_buff), "ctrl-up");
            break;
        case CTRL_ARROW_DOWN:
            snprintf(key_buff, sizeof(key_buff), "ctrl-down");
            break;

        case TAB:
            snprintf(key_buff, sizeof(key_buff), "tab");
            break;

        case ' ':
            snprintf(key_buff, sizeof(key_buff), "spc");
            break;

        case ENTER:
            snprintf(key_buff, sizeof(key_buff), "enter");
            break;

        case ESC:
            snprintf(key_buff, sizeof(key_buff), "esc");
            break;

        case CTRL_FS:
            snprintf(key_buff, sizeof(key_buff), "ctrl-/");
            break;

        case BACKSPACE:
            snprintf(key_buff, sizeof(key_buff), "bsp");
            break;

        case ARROW_LEFT:
            snprintf(key_buff, sizeof(key_buff), "left");
            break;
        case ARROW_RIGHT:
            snprintf(key_buff, sizeof(key_buff), "right");
            break;
        case ARROW_UP:
            snprintf(key_buff, sizeof(key_buff), "up");
            break;
        case ARROW_DOWN:
            snprintf(key_buff, sizeof(key_buff), "down");
            break;

        case DEL_KEY:
            snprintf(key_buff, sizeof(key_buff), "del");
            break;

        case HOME_KEY:
            snprintf(key_buff, sizeof(key_buff), "home");
            break;
        case END_KEY:
            snprintf(key_buff, sizeof(key_buff), "end");
            break;
        case PAGE_UP:
            snprintf(key_buff, sizeof(key_buff), "pageup");
            break;
        case PAGE_DOWN:
            snprintf(key_buff, sizeof(key_buff), "pagedown");
            break;

        case SHIFT_TAB:
            snprintf(key_buff, sizeof(key_buff), "shift-tab");
            break;

        case FN1:
        case FN2:
        case FN3:
        case FN4:
        case FN5:
        case FN6:
        case FN7:
        case FN8:
        case FN9:
        case FN10:
        case FN11:
        case FN12:
            snprintf(key_buff, sizeof(key_buff), "fn-%d", 1 + (key - FN1));
            break;

        case MENU_KEY:
            snprintf(key_buff, sizeof(key_buff), "menu");
            break;

        default:
            if (key < ASCII_KEY_MAX) {
                if (!isprint(key)) { return NULL; }
                snprintf(key_buff, sizeof(key_buff), "%c", (char)key);
            }
    }

    return strdup(key_buff);
}

Julie_Status term_init(Julie_Interp *interp, Julie_Value *expr, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status   status;
    struct termios raw_term;

    status = JULIE_SUCCESS;

    (void)values;
    if (n_values != 0) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, expr, 0, n_values, 0);
        *result = NULL;
        goto out;
    }


    tcgetattr(0, &save_term);
    raw_term = save_term;

    raw_term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    /* output modes - disable post processing */
    /* raw_term.c_oflag &= ~(OPOST); */
    /* control modes - set 8 bit chars */
    raw_term.c_cflag |= (CS8);
    /* local modes - choing off, canonical off, no extended functions,
     *      * no signal chars (^Z,^C) */
    raw_term.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);


    /* control chars - set return condition: min number of bytes and timer. */

    /* Return each byte, or zero for timeout. */
    raw_term.c_cc[VMIN] = 0;
    /* 300 ms timeout (unit is tens of second). */
    raw_term.c_cc[VTIME] = TERM_DEFAULT_READ_TIMEOUT;

    tcsetattr(0, TCSAFLUSH, &raw_term);

    setvbuf(stdout, NULL, _IONBF, 0);

/*     yed_register_sigwinch_handler(); */

    printf(TERM_ALT_SCREEN);
    printf(TERM_MOUSE_BUTTON_ENABLE);
/*     printf(TERM_MOUSE_ANY_ENABLE); */
    printf(TERM_SGR_1006_ENABLE);
    printf(TERM_CURSOR_HIDE);
    printf(TERM_CURSOR_HOME);

    fflush(stdout);


    *result = julie_nil_value(interp);

out:;
    return status;
}

Julie_Status term_fini(Julie_Interp *interp, Julie_Value *expr, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status status;

    status = JULIE_SUCCESS;

    (void)values;
    if (n_values != 0) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, expr, 0, n_values, 0);
        *result = NULL;
        goto out;
    }

/*     printf(TERM_MOUSE_ANY_DISABLE); */
    printf(TERM_SGR_1006_DISABLE);
    printf(TERM_MOUSE_BUTTON_DISABLE);
    printf(TERM_STD_SCREEN);
    printf(TERM_CURSOR_SHOW);

    fflush(stdout);

    tcsetattr(0, TCSAFLUSH, &save_term);

    *result = julie_nil_value(interp);

out:;
    return status;
}

Julie_Status term_clear(Julie_Interp *interp, Julie_Value *expr, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status status;

    status = JULIE_SUCCESS;

    (void)values;
    if (n_values != 0) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, expr, 0, n_values, 0);
        *result = NULL;
        goto out;
    }


    printf(TERM_CLEAR_SCREEN TERM_CURSOR_HOME);

    *result = julie_nil_value(interp);

out:;
    return status;
}

Julie_Status term_flush(Julie_Interp *interp, Julie_Value *expr, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status status;

    status = JULIE_SUCCESS;

    (void)values;
    if (n_values != 0) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, expr, 0, n_values, 0);
        *result = NULL;
        goto out;
    }



    *result = julie_nil_value(interp);

out:;
    return status;
}

Julie_Status term_get_event(Julie_Interp *interp, Julie_Value *expr, unsigned n_values, Julie_Value **values, Julie_Value **result) {
    Julie_Status  status;
    int           n;
    char          c;
    int           input[16];
    int           len;
    Julie_Value  *key;
    Julie_Value  *val;
    char         *str;

    status = JULIE_SUCCESS;

    *result = NULL;

    (void)values;
    if (n_values != 0) {
        status = JULIE_ERR_ARITY;
        julie_make_arity_error(interp, expr, 0, n_values, 0);
        *result = NULL;
        goto out;
    }

    n = read(0, &c, 1);
    if (n <= 0) { goto out_no_event; }

    input[0] = c;

    if (c == ESC) {
        len = esc_timeout(input);

        if (len == 3) {
            len = esc_sequence(input);
        }
    }


    if (IS_MOUSE(input[0])) {
        *result = julie_object_value(interp);

        key = julie_symbol_value(interp, julie_get_string_id(interp, "'type"));
        val = julie_symbol_value(interp, julie_get_string_id(interp, "'mouse"));
        julie_object_insert_field(interp, *result, key, val);
        julie_free_value(interp, key);
        julie_free_value(interp, val);

        key = julie_symbol_value(interp, julie_get_string_id(interp, "'action"));
        if (MOUSE_KIND(input[0]) == MOUSE_PRESS) {
            val = julie_symbol_value(interp, julie_get_string_id(interp, "'down"));
        } else if (MOUSE_KIND(input[0]) == MOUSE_RELEASE) {
            val = julie_symbol_value(interp, julie_get_string_id(interp, "'up"));
        } else if (MOUSE_KIND(input[0]) == MOUSE_DRAG) {
            val = julie_symbol_value(interp, julie_get_string_id(interp, "'drag"));
        } else if (MOUSE_KIND(input[0]) == MOUSE_OVER) {
            val = julie_symbol_value(interp, julie_get_string_id(interp, "'over"));
        } else {
            val = julie_symbol_value(interp, julie_get_string_id(interp, "???"));
        }
        julie_object_insert_field(interp, *result, key, val);
        julie_free_value(interp, key);
        julie_free_value(interp, val);

        key = julie_symbol_value(interp, julie_get_string_id(interp, "'button"));
        if (MOUSE_BUTTON(input[0]) == MOUSE_BUTTON_LEFT) {
            val = julie_symbol_value(interp, julie_get_string_id(interp, "'left"));
        } else if (MOUSE_BUTTON(input[0]) == MOUSE_BUTTON_MIDDLE) {
            val = julie_symbol_value(interp, julie_get_string_id(interp, "'middle"));
        } else if (MOUSE_BUTTON(input[0]) == MOUSE_BUTTON_RIGHT) {
            val = julie_symbol_value(interp, julie_get_string_id(interp, "'right"));
        } else {
            val = julie_symbol_value(interp, julie_get_string_id(interp, "???"));
        }
        julie_object_insert_field(interp, *result, key, val);
        julie_free_value(interp, key);
        julie_free_value(interp, val);

        key = julie_symbol_value(interp, julie_get_string_id(interp, "'row"));
        val = julie_sint_value(interp, MOUSE_ROW(input[0]));
        julie_object_insert_field(interp, *result, key, val);
        julie_free_value(interp, key);
        julie_free_value(interp, val);

        key = julie_symbol_value(interp, julie_get_string_id(interp, "'col"));
        val = julie_sint_value(interp, MOUSE_COL(input[0]));
        julie_object_insert_field(interp, *result, key, val);
        julie_free_value(interp, key);
        julie_free_value(interp, val);
    } else {
        str = key_to_string(input[0]);
        if (str == NULL) { goto out_no_event; }

        *result = julie_object_value(interp);

        key = julie_symbol_value(interp, julie_get_string_id(interp, "'type"));
        val = julie_symbol_value(interp, julie_get_string_id(interp, "'key"));
        julie_object_insert_field(interp, *result, key, val);
        julie_free_value(interp, key);
        julie_free_value(interp, val);
        key = julie_symbol_value(interp, julie_get_string_id(interp, "'key"));
        val = julie_string_value(interp, str);
        julie_object_insert_field(interp, *result, key, val);
        julie_free_value(interp, key);
        julie_free_value(interp, val);
        free(str);
    }


out_no_event:;
    if (*result == NULL) {
        *result = julie_nil_value(interp);
    }

out:;
    return status;
}

Julie_Value *julie_init_package(Julie_Interp *interp) {
#define JULIE_BIND_FN(_name, _fn) julie_bind_fn(interp, julie_get_string_id(interp, (_name)), (_fn))

    JULIE_BIND_FN("@term:init",      term_init);
    JULIE_BIND_FN("@term:fini",      term_fini);
    JULIE_BIND_FN("@term:clear",     term_clear);
    JULIE_BIND_FN("@term:flush",     term_flush);
    JULIE_BIND_FN("@term:get-event", term_get_event);

    return julie_string_value(interp, "term: draw in the terminal.");
}
