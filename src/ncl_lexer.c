/* =======================================================================
 * ncl_lexer.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include "ncl_lexer.h"

#include <assert.h>

typedef enum CharacterClass {
    /* todo: put the number in order */
    whiteClass      = 0x0001, /* white space */
    eolClass        = 0x0002, /* end of line */
    ctrlClass       = 0x0004, /* control characters */
    utf8Class       = 0x0010, /* used by utf8 */
    letterClass     = 0x0020, /* ascii letters */
    idSupClass      = 0x0080, /* additional characters valid in id: _ $ */
    operClass       = 0x0200, /* operators */
    digitClass      = 0x0040, /* digits */
    numSupClass     = 0x0400, /* additional characters valid in numbers: _ # . */
    stringBaseClass = 0x0008, /* valid characters in all strings */
    stringAuxClass  = 0x0100  /* additional characters valid in non interpolated strings */
} CharacterClass;

static unsigned short charClass[] = {
        // NUL   SOH     STX     ETX     EOT     ENQ     ACK     BEL      BS      HT      LF      VT      FF      CR      SO      SI
        0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0005, 0x0006, 0x0005, 0x0005, 0x0006, 0x0004, 0x0004,
        // DLE   DC1     DC2     DC3     DC4     NAK     SYN     ETB     CAN      EM     SUB     ESC      FS      GS      RS      US
        0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004,
        // SPC    !        "      #       $       %       &       '       (       )       *       +       ,       -       .       /
        0x0009, 0x0208, 0x0000, 0x0408, 0x0180, 0x0208, 0x0208, 0x0008, 0x0008, 0x0008, 0x0208, 0x0208, 0x0008, 0x0208, 0x0408, 0x0208,
        // 0      1       2       3       4       5       6       7       8       9       :       ;       <       =       >       ?
        0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0008, 0x0008, 0x0208, 0x0208, 0x0208, 0x0008,
        // @      A       B       C       D       E       F       G       H       I       J       K       L       M       N       O
        0x0008, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028,
        // P      Q       R       S       T       U       V       W       X       Y       Z       [       \       ]       ^       _
        0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0008, 0x0000, 0x0008, 0x0008, 0x0488,
        // `      a       b       c       d       e       f       g       h       i       j       k       l       m       n       o
        0x0008, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028,
        // p      q       r       s       t       u       v       w       x       y       z       {       |       }       ~      DEL
        0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0008, 0x0208, 0x0008, 0x0208, 0x0004,

        0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
        0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
        0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
        0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
        0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
        0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
        0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
        0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010
};

#define HAS_CLASS(c, cl) ((charClass[(unsigned char)(c)] & (unsigned)(cl)) != 0)

ncl_lexer_result ncl_lex(char const* cur, char const* end, bool skipEOL)
{
    start:
    do {
        while (cur != end && HAS_CLASS(*cur, whiteClass)) {
            ++cur;
        }
        if (cur != end && skipEOL && HAS_CLASS(*cur, eolClass)) {
            ++cur;
        }
    } while (cur != end && HAS_CLASS(*cur, whiteClass));

    ncl_lexer_result result;
    result.start = cur;
    if (cur == end) {
        result.end = cur;
        result.kind = ncl_eof_tk;
        return result;
    }

    if (HAS_CLASS(*cur, utf8Class)) {
        while (cur != end && HAS_CLASS(*cur, utf8Class)) {
            ++cur;
        }
        result.end = cur;
        result.kind = ncl_error_tk;
        return result;
    }
    switch (*cur++) {
        /* control characters, excepted those which are white space or eol */
        case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
        case 0x08:                                                        case 0x0E: case 0x0F:
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
        case 0x18: case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1E: case 0x1F:
        case 0x7F:
            result.end = cur;
            result.kind = ncl_error_tk;
            break;

        /* end of line */
        case 0x0A:
            if (cur != end && *cur == 0x0D) {
                ++cur;
            }
            result.end = cur;
            result.kind = ncl_eol_tk;
            break;

        case 0x0D:
            if (cur != end && *cur == 0x0A) {
                ++cur;
            }
            result.end = cur;
            result.kind = ncl_eol_tk;
            break;

        /* comments */
        case '#':
            while (cur != end && *cur != 0x0A && *cur != 0x0D) {
                ++cur;
            }
            goto start;

        /* identifiers */
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
        case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P':
        case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
        case 'Y': case 'Z':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
        case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
        case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x':
        case 'y': case 'z':
        case '$': case '_':
            while (cur != end && HAS_CLASS(*cur, letterClass|idSupClass|digitClass)) {
                ++cur;
            }
            result.end = cur;
            result.kind = ncl_id_tk;
            break;

        case '"':
            result.kind = ncl_string_tk;
            do {
                while (cur != end && HAS_CLASS(*cur, stringBaseClass|stringAuxClass)) {
                    ++cur;
                }
                if (cur != end) {
                    if (*cur == '\\') {
                        ++cur;
                        /* todo: validate other escape sequence */
                        if (cur != end && *cur == '"') {
                            ++cur;
                        }
                    } else if (*cur > 0x7F) {
                        /* todo: validate UTF-8 encoding, that's why it's outside the happy loop */
                        while (cur != end && *cur > 0x7F) {
                            ++cur;
                        }
                    } else if (*cur != '"') {
                        result.kind = ncl_error_tk;
                        if (*cur != '"' && *cur != 0x0A && *cur != 0x0D) {
                            ++cur;
                        }
                    }
                }
            } while (cur != end && *cur != '"' && *cur != 0x0A && *cur != 0x0D);
            if (cur != end && *cur == '"') {
                ++cur;
            } else {
                result.kind = ncl_error_tk;
            }
            result.end = cur;
            break;

        /* numbers */
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
        case '8': case '9':
            do {
                while (cur != end && HAS_CLASS(*cur, digitClass | letterClass | numSupClass)) {
                    ++cur;
                }
                if ((*cur == '+' || *cur == '-') && (cur[-1] == 'e' || cur[-1] == 'E' || cur[-1] == 'p' || cur[-1] == 'P')) {
                    ++cur;
                }
            } while (cur != end && HAS_CLASS(*cur, digitClass | letterClass | numSupClass));
            result.end = cur;
            result.kind = ncl_integer_tk;
            break;

        /* operators */
        case '!': case '%': case '&': case '*': case '+': case '-': case '/': case '<':
        case '=': case '>': case '|': case '~':
            while (cur != end && HAS_CLASS(*cur, operClass)) {
                ++cur;
            }
            result.end = cur;
            result.kind = ncl_operator_tk;
            break;

        /* punctuations */
        case '(':
            if (cur != end && *cur == ':') {
                ++cur;
                result.kind = ncl_reserved_tk;
            } else {
                result.kind = ncl_reserved_tk;
            }
            result.end = cur;
            break;

        case ':':
            if (cur != end && *cur == ')') {
                ++cur;
                result.kind = ncl_reserved_tk;
            } else if (cur != end && *cur == '=') {
                ++cur;
                result.kind = ncl_reserved_tk;
            } else if (cur != end && *cur == ']') {
                ++cur;
                result.kind = ncl_reserved_tk;
            } else if (cur != end && *cur == '}') {
                ++cur;
                result.kind = ncl_reserved_tk;
            } else {
                result.kind = ncl_reserved_tk;
            }
            result.end = cur;
            break;

        case '[':
            if (cur != end && *cur == ':') {
                ++cur;
                result.kind = ncl_reserved_tk;
            } else {
                result.kind = ncl_reserved_tk;
            }
            result.end = cur;
            break;

        case '{':
            if (cur != end && *cur == ':') {
                ++cur;
                result.kind = ncl_reserved_tk;
            } else {
                result.kind = ncl_reserved_tk;
            }
            result.end = cur;
            break;

        case '\'': case ')': case ',': case '.': case ';': case '?':  case '@': case '\\':
        case ']':  case '^': case '`': case '}':
            result.end = cur;
            result.kind = ncl_reserved_tk;
            break;

        /* should not happen */
        case 0x09: case 0x0B: case 0x0C: case ' ':
        default:
            assert(0);
            goto start;
    }
    return result;
};
