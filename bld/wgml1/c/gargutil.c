/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: utility functions for arguments
*
****************************************************************************/


#include "wgml.h"

/***************************************************************************/
/* validate and return the character parameter, or raise an error          */
/***************************************************************************/

char parse_char( const char *p, size_t len )
{
    char        c;

    c = '\0';
    if( len == 1 ) {
        c = p[0];
    } else if( len == 2 ) {         // 2 hex characters
        if( my_isxdigit( p[0] ) && my_isxdigit( p[1] ) ) {
            if( my_isdigit( p[0] ) ) {
                c = p[0] - '0';
            } else {
                c = my_toupper( p[0] ) - 'A' + 10;
            }
            if( my_isdigit( p[1] ) ) {
                c = c * 16 + p[1] - '0';
            } else {
                c = c * 16 + my_toupper( p[1] ) - 'A' + 10;
            }
        } else {
            xx_line_err_ci( err_cw_not_char, p, len );
        }
    } else {
        xx_line_err_ci( err_cw_not_char, p, len );
    }

    return( c );
}

/***************************************************************************/
/* return true if the character parameter is a string delimiter            */
/***************************************************************************/

bool is_quote_char( char c )
{
    if( c == s_q || c == d_q || c == slash || c == excl  || c == cent ||
        ((c == l_q) && (c != CW_sep_char)) ||
        c == not_c || c == vbar1 || c == vbar2 ) {
        return( true );
    } else {
        return( false );
    }
}

/***************************************************************************/
/*  scan blank delimited argument perhaps quoted                           */
/*                                                                         */
/*  extension: if unquoted and equalsign, then quoted parm allowed         */
/*            *var="value "                                                */
/*                                                                         */
/*  single-char arguments are special-cased                                */
/***************************************************************************/

condcode getarg( void )
{
    condcode    cc;
    char    *   p;
    char        quote;
    char        valquote;
    bool        quoted;
    bool        valquoted;

    if( scan_stop <= scan_start ) {     // already at end
        cc = omit;                      // arg omitted
    } else {
        p = scan_start;
        while( *p == ' ' && p <= scan_stop ) {// skip leading blanks
            p++;
        }
        if( p > scan_stop ) {
            return( omit );             // nothing found
        }

        if( p == scan_stop) {           // one character token found
            arg_flen = 1;
            g_tok_start = p;
            scan_start = p + 1;         // address of start for next call
            return( pos );              // arg found
        }

        quote = '\0';
        valquote = '\0';
        quoted = false;
        valquoted = false;
        g_tok_start = p;

        if( is_quote_char( *p ) ) {     // arg starts with quote
            quote = *p;
            p++;
            quoted = true;
        } else {
            quote = '\0';
            quoted = false;
        }
        for( ;; p++ ) {

            if( p > scan_stop || *p == '\0' ) {
                if( quoted ) {
                    quote = '\0';
                    quoted = false;
                    p = g_tok_start;            // find end of space-delimited token
                    while( (p < scan_stop) && (*p != ' ') ) {
                        p++;
                    }
                }
                break;
            }
            if( *p == ' ' && quote == '\0' ) {  // unquoted, blank is end
                break;
            }
            if( *p == quote ) {
                break;
            }
            if( quote == '\0' && (p[0] == '=') && is_quote_char( p[1] ) ) {
                valquoted = true;
                valquote = p[1];
                p += 2;
                for( ; *p != '\0' && p <= scan_stop; p++ ) {
                    if( *p == valquote ) {
                        p++;
                        break;
                    }
                }
                break;
            }
        }
        if( quoted ) {
            g_tok_start++;
            scan_start = p + 1;         // address of start for next call
        } else {
            scan_start = p;             // address of start for next call
        }
        arg_flen = p - g_tok_start;       // length of multichar arg
        if( arg_flen > 0 ) {
            if( quoted ) {
                cc = quotes;            // quoted arg found
            } else {
                cc = pos;               // arg found
            }
        } else {
            if( quoted ) {
                cc = quotes0;           // Nullstring
            } else {
                cc = omit;              // length zero
            }
        }
    }
    return( cc );
}

/***************************************************************************/
/*  scan quoted string argument                                            */
/*  special for if terms                                                   */
/***************************************************************************/

condcode getqst( void )
{
    condcode    cc;
    char    *   p;
    char        c;
    char        quote;
    bool        quoted;

    if( scan_stop <= scan_start ) {     // already at end
        cc = omit;                      // arg omitted
    } else {
        p = scan_start;
        while( *p == ' ' && p <= scan_stop ) {// skip leading blanks
            p++;
        }

        if( p > scan_stop ) {
            return( omit );             // nothing found
        }

        quote = '\0';
        quoted = false;
        g_tok_start = p;
        c = *p;
        if( is_quote_char( c ) ) {
            quote = c;      // single or double quotes, vertical bar and cent
            p++;
            quoted = true;
        } else {
            quote = '\0';
            quoted = false;
        }
        for( ; *p != '\0' && p <= scan_stop; p++ ) {  // look for end of string
            if( quoted ) {
                if( *p == quote ) {
                    if( p[1] == '\0' || p[1] == ' ' ) {
                        break;          // quote followed by blank or null is end
                    }
                    if( p[1] == quote ) {
                        continue;       // 2 quote chars not end of string
                    }
                }
            } else {                    // unquoted
                if( *p == ' ' ) {
                    break;              // blank is end
                }
            }
        }
        if( quoted ) {
            g_tok_start++;
            scan_start = p + 1;         // start address for next call
            arg_flen = p - g_tok_start; // length of arg
        } else {
            scan_start = p;             // address of start for next call
            arg_flen = p - g_tok_start; // length of arg
        }
        if( arg_flen > 0 ) {
            if( quoted ) {
                if( *p != quote ) {
                    cc = no;            // only start quote found
                } else {
                    cc = quotes;        // quoted arg found
                }
            } else {
                cc = no;                // not quoted
            }
        } else {
            cc = omit;                  // length zero
        }
    }
    return( cc );
}

/*
 * Test character as valid for an LAYOUT attribute name
 */

bool is_lay_att_char( char c )
{
    bool    test;

    test = my_isalpha( c );
    if( !test ) {
        test = ( c == '_' );
    }
    return( test );
}

/*
 * Test character as valid for a function name
 */

bool is_function_char( char c )
{
    bool    test;

    test = my_isalnum( c );
    return( test );
}

/*
 * Test character as valid for an identifier name
 */

bool is_id_char( char c )
{
    bool    test;

    test = my_isalnum( c );
    return( test );
}

/*
 * Test character as valid for a macro name
 */

bool is_macro_char( char c )
{
    bool    test;

    test = my_isalnum( c );
    if( !test ) {
        test = ( c == '@' ) || ( c == '#' ) || ( c == '$' ) || ( c == '_' );
    }
    return( test );
}

/*
 * Test character as valid for a symbol name
 */

bool is_symbol_char( char c )
{
    bool    test;

    test = my_isalnum( c );
    if( !test ) {
        test = ( c == '@' ) || ( c == '#' ) || ( c == '$' ) || ( c == '_' );
    }
    return( test );
}

/*
 * Test character for a full stop character
 */

bool is_stop_char( char c )
{
    bool    test;

    test = ( c == '.' ) || ( c == ':' ) || ( c == '!' ) || ( c == '?' );
    return( test );
}

/*
 * Test character for a space or tab character
 */

bool is_space_tab_char( char c )
{
    return( ( c == ' ' ) || ( c == '\t' ) );
}

/*
 * If first and last character are the same and one of the quote chars
 * the start and end pointers are adjusted
 * but only if a and z are not equal (that is, only if the value has more
 * than one character
 */

void unquote_if_quoted( char **a, char **z )
{
    if( (*a != *z) && (**a == **z) && is_quote_char( **a ) ) {
        *a += 1;
        *z -= 1;
    }
}

