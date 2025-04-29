/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  implement .se and .sr script control words
*
****************************************************************************/


#include "wgml.h"


/*****************************************************************************/
/* construct symbol name and optionally subscript from input                 */
/* Note: p must point to a symbol name, as no trailing "=<value" is required */
/*****************************************************************************/

char * scan_sym( char * p, symvar * sym, sub_index * subscript, char * * result, bool splittable )
{
    char        *   pend;
    int             p_level;
    char        *   psave;
    char            quote;
    char        *   sym_start;
    int             rc;
    size_t          k;
    sub_index       var_ind;
    symsub      *   symsubval;

    (void)result;

    psave = p;
    scan_err = false;
    sym->next = NULL;
    sym->flags = 0;
    *subscript = no_subscript;          // not subscripted

    SkipSpaces( p );                    // skip over spaces
    if( *p == d_q || *p == s_q || *p == l_q ) {
        quote = *p++;
    } else {
        quote = '\0';
    }
    if( *p == '*' ) {                   // local var
        p++;
        sym->flags = local_var;
    }
    sym_start = p;
    sym->name[0] = '\0';

    k = 0;
    while( is_symbol_char( *p ) ) {

        /* Special WGML 4.0 compatibility: When scanning symbol names,
         * the scanning stops after 10 characters (not including the '&').
         * It is thus possible to reference symbols with maximum-length
         * names without properly separating them, such as
         *   &longsymname
         * and &longsymnam will be resolved if it exists.
         */
        if( splittable && (k == SYM_NAME_LENGTH) )
            break;

        if( k < SYM_NAME_LENGTH ) {
            if( (k == 3) && (sym->name[0] != '$') ) {
                if( sym->name[0] == 's' &&
                    sym->name[1] == 'y' &&
                    sym->name[2] == 's' ) {

                    sym->name[0] = '$';   // create sys shortcut $
                    k = 1;
                }
            }
            sym->name[k++] = my_tolower( *p );
            sym->name[k] = '\0';
        } else {
            if( !scan_err ) {
                scan_err = true;
                if( !ProcFlags.suppress_msg ) {
                    // SC--074 For the symbol '%s'
                    //     The length of a symbol cannot exceed ten characters
                    symbol_name_length_err( sym_start );
                }
            }
        }
        p++;
    }

    if( p == sym_start ) {              // special for &*
        if( *p != ampchar ) {           // not &*&xx construct

            if( (sym->flags & local_var)
                && (input_cbs->fmflags & II_tag_mac) ) {

                strcpy_s( sym->name, SYM_NAME_LENGTH, MAC_STAR_NAME );
            } else if( (sym->flags & local_var) && (input_cbs->fmflags & II_file) ) {
                strcpy_s( sym->name, SYM_NAME_LENGTH, MAC_STAR_NAME );
            } else {
                scan_err = true;
            }
        }
    }
    if( quote != '\0' && quote == *p ) {        // over terminating quote
        p++;
    }
    pend = p;                                   // char after symbol name if not subscripted

    if( !scan_err && (*p == '(') ) {    // subscripted ?
        // find true end of subscript
        psave = p;
        p_level = 0;
        while( *p != '\0' ) {               // to end of buffer
            if( *p == '(' ) {
                p_level++;
            } else if( *p == ')' ) {
                p_level--;
                if( p_level == 0 ) {
                    break;
                }
            }
            p++;
        }
        pend = p + 1;                       // character after outermost ')'

        if( p_level > 0 ) {                 // at least one missing ')'
            /* Note: missing ')' is not an error in wgml 4.0 */
            scan_err = true;
        } else {
            p = psave + 1;
            if( *p == ')' ) {               // () is auto increment
                p++;
                var_ind = 0;
                if( sym->flags & local_var )  {
                    rc = find_symvar( input_cbs->local_dict, sym->name, var_ind, &symsubval );
                } else {
                    rc = find_symvar( global_dict, sym->name, var_ind, &symsubval );
                }
                if( rc > 0 ) {              // variable exists use last_auto_inc
                    *subscript = symsubval->base->last_auto_inc + 1;
                } else {
                    *subscript = 1;         // start with index 1
                }
                sym->flags |= auto_inc + subscripted;
            } else if( *p == '*' ) {        // * concatenates all elements
                p++;
                if( *p == '+' ) {
                    *subscript = pos_subscript; // positive indices only
                    p++;
                } else if( *p == '-' ) {
                    *subscript = neg_subscript; // negative indices only
                    p++;
                } else {
                    *subscript = all_subscript; // all indices
                }
                if( *p != ')' ) {
                    scan_err = true;
                }
            } else {
                char            *       pa;
                char            *   *   ppval;
                char                    valbuf[BUF_SIZE];
                condcode                cc;
                getnum_block            gn;

                memcpy_s( valbuf, buf_size, p, pend - p - 1 );
                valbuf[pend - p - 1] = '\0';
                pa = valbuf;
                ppval = &pa;

                ProcFlags.unresolved = false;
                finalize_subscript( ppval, splittable );
                if( ProcFlags.unresolved ) {
                    scan_err = true;
                } else {
                    gn.arg.e = gn.arg.s = valbuf;
                    while( *gn.arg.e != '\0' && (*gn.arg.e != ')') )
                        gn.arg.e++;
                    *gn.arg.e = '\0';    // make nul terminated string
                    gn.ignore_blanks = false;
                    cc = getnum( &gn );     // try numeric expression evaluation
                    if( cc == pos || cc == neg ) {
                        *subscript = gn.result;
                        if( *p == ')' ) {
                            p++;
                        }
                        SkipDot( p );
                        sym->flags |= subscripted;
                    } else {
                        if( !scan_err && !ProcFlags.suppress_msg ) {
                            xx_line_err_c( err_sub_invalid, p );
                        }
                        scan_err = true;
                    }

                }
            }
        }
        if( scan_err ) {
            p = psave;
        } else {
            p = pend;
        }
    }
    return( p );
}

/***************************************************************************/
/*  processing  SET                                                        */
/*                                                                         */
/*                          = <character string>                           */
/*         .SE       symbol = <numeric expression>                         */
/*                          <OFF>                                          */
/*  symbol may be subscripted (3) or () for auto increment 1 - n           */
/*                                                                         */
/*  This summarizes SR, since SE extends SR and SR is not implemented      */
/*         .se x1     off                                                  */
/*         .se x2a    'string (Note 1)                                     */
/*         .se x2b    'string (Note 1)                                     */
/*         .se x3a =  string                                               */
/*         .se x3b =  'string (Note 2)                                     */
/*         .se x4c =  'string'(Note 2)                                     */
/*         .se n1  =  1234                                                 */
/*         .se n2a =  (1+(2+5)/6)                                          */
/*         .se n2b =  -1+(2+5)/6)                                          */
/*  Note 1: these apply only to ', the first of which must be present      */
/*          at least one space must precede the initial '                  */
/*  Note 2: these apply to all delimiters: ', ", /, |, !, ^, 0x9b and,     */
/*          apparently, 0xdd and 0x60                                      */
/*          the final delimiter must be followed by a space or '\0'        */
/*          this means that 'abc'def' is a seven-character delimited value */
/*          provided it is at the end of the line or followed by a space   */
/*                                                                         */
/*  Except for local symbol *, spaces are removed from the end of the line */
/*  before further processing (so that spaces inside delimiters are not    */
/*  afftected, but any following the closing delimiter are)                */
/*                                                                         */
/***************************************************************************/

void    scr_se( void )
{
    char        *   p;
    char        *   valstart;
    int             rc;
    sub_index       subscript;
    symsub      *   symsubval;
    symvar          sym;
    sym_dict_hdl    working_dict;
    size_t          len;

    subscript = no_subscript;                       // not subscripted
    scan_err = false;
    p = scan_sym( scan_start, &sym, &subscript, NULL, false );

    if( strcmp( sym.name, MAC_STAR_NAME ) != 0 ) {  // remove trailing blanks from all symbols except *
        valstart = p;
        for( len = strlen( p ); len-- > 0; ) {
            if( p[len] != ' ' ) {
                break;
            }
            p[len] = '\0';
        }
        p = valstart;
    }

    if( sym.flags & local_var ) {
        working_dict = input_cbs->local_dict;
    } else {
        working_dict = global_dict;
    }

    if( ProcFlags.blanks_allowed ) {
        SkipSpaces( p );                        // skip over spaces
    }
    if( *p == '\0' ) {
        if( !ProcFlags.suppress_msg ) {
            xx_line_err_c( err_eq_expected, p);
        }
        scan_err = true;
    }
    if( !scan_err ) {
        if( *p == ')' ) {
            p++;
        }
        valstart = p;
        if( *p == '=' ) {                       // all other cases have no equal sign (see above)
            p++;
            if( ProcFlags.blanks_allowed ) {
                SkipSpaces( p );                // skip over spaces to value
            }
            valstart = p;
            if( is_quote_char( *valstart ) ) {      // quotes ?
                p++;
                while( *p != '\0' ) {   // remove final character, if it matches the start character              if( (*valstart == *p) && (!*(p+1) || (*(p+1) == ' ')) ) {
                    if( (*valstart == *p) && !*(p+1) ) {
                        break;
                    }
                    ++p;
                }
                if( (valstart < p) && (*p == *valstart) ) { // delete quotes if more than one character
                    valstart++;
                    *p = '\0';
                }
            } else {                                // numeric or undelimited string
                getnum_block    gn;
                condcode        cc;

                gn.arg.s = valstart;
                gn.arg.e = scan_stop;
                gn.ignore_blanks = true;
                cc = getnum( &gn );             // try numeric expression evaluation
                if( cc != notnum ) {
                    valstart = gn.resultstr;
                }                               // if notnum treat as character value
            }
            rc = add_symvar( working_dict, sym.name, valstart, subscript, sym.flags );
        } else if( *p == '\'' ) {               // \' may introduce valid value
            if( *(p - 1) == ' ' ) {             // but must be preceded by a space
                p++;
                while( *p != '\0' && (*valstart != *p) ) {  // look for final \'
                    p++;
                }
                valstart++;                                 // delete initial \'
                if( (valstart < p) && (*p == '\'') ) {      // delete \' at end
                    *p = '\0';
                }
                rc = add_symvar( working_dict, sym.name, valstart, subscript, sym.flags );
            } else {                                        // matches wgml 4.0
                if( !ProcFlags.suppress_msg ) {
                    xx_line_err_c( err_eq_expected, p);
                }
                scan_err = true;
            }
        } else if( strncmp( p, "off", 3 ) == 0 ) {       // OFF
            p += 3;
            rc = find_symvar( working_dict, sym.name, subscript, &symsubval );
            if( rc == 2 ) {
                symsubval->base->flags |= deleted;
            }
        } else {
            if( !ProcFlags.suppress_msg ) {
                xx_warn_cc( wng_miss_inv_value, sym.name, p );
            }
            scan_err = true;
        }
    }
    scan_restart = scan_stop;
    return;
}

