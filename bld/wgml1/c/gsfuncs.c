/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML controls multi letter functions  &'substr(), ...
*               and calls corresponding function see gsfuncs.h
*
*               scr_multi_funcs()
*
*      static helper functions
*               alloc_resbuf()        result work buffer
*               err_info()            output err info and set error result
*               find_end_of_parm()    find parm separator char
*               find_start_of_parm()  find beginning of parm
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*    SCR multi letter functions                                           */
/***************************************************************************/

static  const   scrfunc scr_functions[] = {
    #define pick( name, length, parms, optparms, routine )  { #name, length, parms, optparms, routine },
    #include "gsfuncs.h"
    #undef pick
    { " ", 0, 0, 0, NULL }              // end
};

#define SCR_FUNC_MAX (sizeof( scr_functions ) / sizeof( scr_functions[0] ) - 1)

static bool         multiletter_function;   // parm contains function
static bool         var_in_parm;            // parm contains variable


/****************************************************/
/*allocate a buffer for resolving symvars+functions */
/****************************************************/
static  char    * alloc_resbuf( inp_line ** in_wk )
{
    inp_line  * wk;

    if( *in_wk == NULL ) {
        *in_wk =  mem_alloc( sizeof( inp_line ) + buf_size );
        (*in_wk)->next = NULL;
    } else {
        wk = mem_alloc( sizeof( inp_line ) + buf_size );
        wk->next = *in_wk;
        *in_wk = wk;
    }
    return( (*in_wk)->value );
}


/*  find end of parm for multi letter functions
 *     end of parm is either , or )
 *     but only if outside of string and not in deeper paren level
 *     string delimiters are several chars
 *
 */

static  char    * find_end_of_parm( char *p, char *pend )
{
    char    quotechar[MAX_PAREN];
    bool    instring[MAX_PAREN];
    int     paren_level;
    char    c;
    char    cm1;
    char    cm2;
    bool    finished;
    bool    test_for_quote;             // only at first char after ( or ,

    paren_level = 0;
    quotechar[paren_level] ='\0';
    instring[paren_level] = false;
    instring[1] = false;
    finished = false;
    test_for_quote = true;
    c = '\0';
    cm1 = '\0';
    for(  ; *p != '\0'; p++ ) {
        cm2 = cm1;
        cm1 = c;
        c = *p;
        if( cm1 == ampchar ) {
            if( c == '\'' ) {
                multiletter_function = true;    // parm contains a function
            } else {
                var_in_parm = true;             // parm contains variable
            }
        }
        if( multiletter_function ) {
            multiletter_function = false;       // multiletter_function is static, reset
            p++;                            // over "'"
            p = find_end_of_parm( p, pend );
            if( *p == ',' ) {                // end of parm
                break;
            }
        } else if( instring[paren_level] ) {
            if( c == quotechar[paren_level] ) {
                instring[paren_level] = false;
            }
        } else if( test_for_quote
          && is_quote_char( c ) ) {
            if( (cm1 == ampchar)    /* &' sequence */
              || ((cm2 == ampchar)  /* &X' sequence */
              && my_isalpha( cm1 )) ) {
                /* no instring change */
            } else {
                instring[paren_level] = true;
                quotechar[paren_level] = c;
            }
            test_for_quote = false;
        } else {
            switch( c ) {
            case '(':
                paren_level += 1;
                test_for_quote = true;
                if( paren_level < MAX_PAREN ) {
                    instring[paren_level] = false;
                } else {
                    paren_level--;
                    finished = true;// error msg ??? TBD
                }
                break;
            case ')':
                paren_level--;
                if( paren_level <= 0 ) {
                    finished = true;
                    if( p < pend ) {
                        p++;    // step over final ')'
                    }
                    if( *p == '.' ) {
                        p++;    // step over '.' terminating subscripted symbol
                    }
                }
                break;
            case ',':
                if( paren_level == 0 ) {
                    finished = true;
                }
                test_for_quote = true;
                break;
            default:
                test_for_quote = false;
                break;
            }
        }
        if( finished
          || (p >= pend)) {
            break;
        }
    }
    return( p );
}


/***************************************************************************/
/*  find start of parm                                                     */
/*     leading blanks are skipped                                          */
/***************************************************************************/

static  char    * find_start_of_parm( char *p )
{
    ++p;                            // over ( or ,
    SkipSpaces( p );                // over unquoted blanks
    return( p );
}

static const scrfunc *check_multiletter_func( const char *p, const char *pend )
{
    char            funcname[FUN_NAME_LENGTH + 1];
    const scrfunc   *fninfo;
    size_t          fnlen;
    int             i;

    for( i = 0; i < FUN_NAME_LENGTH; i++ ) {
        if( p == pend )
            break;
        funcname[i] = my_tolower( *p++ );
    }
    funcname[i] = '\0';
    fnlen = strlen( funcname );
    fninfo = scr_functions;
    for( i = 0; i < SCR_FUNC_MAX; i++ ) {
        if( fnlen == fninfo->length
          && strcmp( funcname, fninfo->fname ) == 0 ) {
            if( (input_cbs->fmflags & II_research)
              && WgmlFlags.firstpass ) {
                out_msg( " Function %s found\n", fninfo->fname );
                add_multi_func_research( funcname );
            }
            ProcFlags.unresolved = false;
            return( fninfo );
        }
        fninfo++;
    }
    ProcFlags.unresolved = true;
    return( NULL );
}

/***************************************************************************/
/*  scr_multi_funcs  isolate function name, lookup name in table           */
/*                   scan function arguments                               */
/*                   call corresponding function                           */
/***************************************************************************/

char *scr_multi_funcs( char *in, char *pstart, char **result, int32_t valsize )
{
    char            *p;              // points into input buffer
    char            *pend;           // points into resbuf
    char            *pret;           // end of function in input buffer
    char            *resbuf;         // buffer into which parm.a and parm.e point
    condcode        cc;
    inp_line        *in_wk;          // stack of resbufs, one for each parameter
    int             k;
    int             m;
    int             parmcount;
    int             p_level;
    parm            parms[MAX_FUN_PARMS];
    const scrfunc   *fninfo;
    size_t          len;

    /* pstart points to the outer open parenthesis ('(') */

    // find true end of function
    p_level = 0;
    for( p = pstart; *p != '\0'; p++ ) {    // to end of buffer
        if( *p == '(' ) {
            p_level++;
        } else if( *p == ')' ) {
            p_level--;
            if( p_level == 0 ) {
                pend = p;               // pend points to outermost ')'
                break;
            }
        }
    }
    pret = p;                           // save for return (points to final ')')

    // test for valid functionname
    fninfo = check_multiletter_func( in + 2, pstart ); // over &' to function name
    if( fninfo == NULL ) {
        /* this is not an error in wgml 4.0 */
        return( pstart );
    }

    /* Missing ')' is only a problem if the function was found */

    if( p_level > 0 ) {      // at least one missing ')'
        xx_line_err_c( err_func_parm_end, pret - 1 );
    }

    // collect the mandatory parm(s)

    p = pstart;
    for( k = 0; k < fninfo->parm_cnt; k++ ) {

        multiletter_function = false;
        var_in_parm = false;

        parms[k].arg.s = find_start_of_parm( p );
        p = find_end_of_parm( parms[k].arg.s, pend );

        if( multiletter_function
          || var_in_parm ) {
            parms[k].redo = true;
        } else {
            parms[k].redo = false;
        }
        parms[k].arg.e = p - 1;
        parms[k + 1].arg.s = p + 1;

        if( p >= pend ) {
            break;                      // end of parms
        }
    }
    m = k + (k < fninfo->parm_cnt);// mandatory parm count

    if( m < fninfo->parm_cnt ) {
        xx_line_err_c( err_func_parm_miss, pret - 1 );
    }

    // collect the optional parm(s)
    if( p >= pend ) {               // no optional parms
        k = 0;
    } else {
        for( k = 0; k < fninfo->opt_parm_cnt; k++ ) {
            multiletter_function = false;
            var_in_parm = false;
            parms[m + k].arg.s = find_start_of_parm( p );
            p = find_end_of_parm( parms[m + k].arg.s, pend );

            if( multiletter_function
              || var_in_parm ) {
                parms[m + k].redo = true;
            } else {
                parms[m + k].redo = false;
            }
            parms[m + k].arg.e = p - 1;
            parms[m + k + 1].arg.s = p + 1;

            if( p >= pend ) {
                break;                  // end of parms
            }
        }
        k += (k < fninfo->opt_parm_cnt);
    }
    parmcount = m + k;                  // total parmcount
    parms[parmcount].arg.s = NULL;          // end of parms indicator

    /* Now resolve those parm that need it */

    in_wk = NULL;                       // no result buffer yet
    for( k = 0; k < parmcount; k++ ) {
        while( parms[k].redo ) {
            resbuf = alloc_resbuf( &in_wk );
            len = (parms[k].arg.e - parms[k].arg.s) + 1;
            if( len > buf_size )
                len = buf_size;
            strncpy( resbuf, parms[k].arg.s, len );// copy parm
            resbuf[len] = '\0';
            parms[k].arg.e = resbuf + len - 1;
            parms[k].arg.s = resbuf;
            if( (input_cbs->fmflags & II_research)
              && WgmlFlags.firstpass ) {
                out_msg( " Function %s parm %s found\n", fninfo->fname, resbuf );
            }

            if( !resolve_symvar_functions( resbuf, false ) )
                break; // no change, we're done

            pend = parms[k].arg.s + strlen( parms[k].arg.s );
            multiletter_function = false;
            var_in_parm = false;
            pend = find_end_of_parm( parms[k].arg.s, pend );

            if( multiletter_function
              || var_in_parm ) {
                parms[k].redo = true;
            } else {
                parms[k].redo = false;
            }

            parms[k].arg.e = resbuf + strlen( parms[k].arg.s ) - 1;
            if( (input_cbs->fmflags & II_research)
              && WgmlFlags.firstpass ) {
                out_msg( " Function      parm %s return\n", resbuf );
            }
        }
    }

    ProcFlags.suppress_msg = multiletter_function;

    cc = fninfo->fun( parms, parmcount, result, valsize );

    ProcFlags.suppress_msg = false;

    free_lines( in_wk );

    if( cc != pos ) {                   // error in function
        **result = '&';                 // result is & to preserve the input
        *result += 1;
        **result = '\0';

        ProcFlags.unresolved = true;

        return( pstart );
    }

    ProcFlags.substituted = true;
    return( pret + 1 );                 // all OK new scan position
}


/*
 * If first and last character are the same and one of the quote chars
 * the start and end pointers are adjusted
 * but only if a and z are not equal (that is, only if the value has more
 * than one character
 */

void scr_unquote_parm( tok_type *tok )
{
    if( (tok->s != tok->e) && (tok->s[0] == tok->e[0]) && is_quote_char( tok->s[0] ) ) {
        tok->s++;
        tok->e--;
    }
}

#if 0

/*
+++++++++++++++++++++++++++++++  used functions in OW documentation +++++++++
used single letter functions: others are unused
&e'  exist 0 or 1
&l'  length, if undefined length of name
&s'  subscript   low
&S'  superscript high
&u'  upper

&w'  width          implemented for internal use

used other functions:

&'delstr(
&'d2c(
&'index(
&'insert(
&'left(
&'length(
&'lower(
&'min(
&'pos(
&'right(
&'strip(
&'substr(
&'subword(
&'translate(
&'upper(
&'veclastpos(
&'vecpos(
&'word(
&'wordpos(
&'words(

&'c2x(           implemented for internal use
&'width(         implemented for internal use

new one OW 2.0
&'compare(
&'find(
*/

#endif
