/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML input line processing
*   some logic / ideas adopted from Watcom Script 3.2 IBM S/360 Assembler
*   see comment in wgml.c
****************************************************************************/

#include "wgml.h"

static  bool        sym_space;          // compiler workaround

/***************************************************************************/
/*  perform symbol substitution on certain special symbols:                */
/*    AMP (identified by the is_AMP tag)                                   */
/*  all other items starting with '&' are treated as text                  */
/***************************************************************************/
void process_late_subst( char * buf )
{
    char    *   p;
    char    *   symstart;
    char        tail[BUF_SIZE];
    char    *   tokenend;       // save end of current token
    char    *   tokenstart;     // save position of current '&'
    int         rc;
    sub_index   var_ind;        // subscript value (if symbol is subscripted)
    symsub  *   symsubval;      // value of symbol
    symvar      symvar_entry;

    p = strchr( buf, ampchar ); // look for & in buffer
    while( p != NULL ) {        // & found
        if( *(p + 1) == ' ' ) { // not a symbol substition, attribute, or function
            tokenend = p + 1;
        } else if( my_isalpha( p[1] )
          && p[2] == '\''
          && p[3] > ' ' ) {   // attribute
            tokenend = p + 3;
        } else if( *(p + 1) == '\'' ) {         // function or text
            p += 2;
            while( is_function_char(*p) )       // find end of function name
                p++;
            tokenend = p;
        } else {                                // symbol
            tokenstart = p;
            p++;                                // over '&'
            symstart = p;                       // remember start of symbol name
            scan_err = false;
            ProcFlags.suppress_msg = true;
            p = scan_sym( symstart, &symvar_entry, &var_ind, NULL, false );
            ProcFlags.suppress_msg = false;
            tokenend = p;
            if( !scan_err ) {                   // potentially qualifying symbol
                if( symvar_entry.flags & local_var ) {  // lookup var in dict
                    rc = find_symvar_l( input_cbs->local_dict, symvar_entry.name,
                                        var_ind, &symsubval );
                } else {
                    rc = find_symvar( global_dict, symvar_entry.name, var_ind,
                                      &symsubval );
                }
                if( rc == 2 ) {             // variable found + resolved
                    if( symsubval->base->flags & is_AMP ) {
                        /* replace symbol with value */
                        strcpy_s( tail, buf_size, tokenend );       // copy tail
                        p = tokenstart;
                        strcpy_s( p, buf_size, symsubval->value );  // copy value
                        if( tail[0] == '.' ) {
                            strcat_s( buf, buf_size, tail + 1);     // append tail to buf, skipping initial "."
                        } else {
                            strcat_s( buf, buf_size, tail );        // append tail to buf
                        }
                        tokenend = tokenstart + 1;                  // restart from replaced symbol.
                    }
                }
            }
        }
        p = tokenend;                   // skip argument
        p = strchr( p, ampchar );       // look for next & in buffer
    }
    return;
}


/*  split_input
 *  The (physical) line is split
 *  The second part will be processed by the next getline()
 *  pushing any already split part down
 *
 */

void split_input( char * buf, char * split_pos, i_flags fmflags )
{
    inp_line    *   wk;
    size_t          len;

    (void)buf;

    len = strlen( split_pos );          // length of second part
    if( len > 0 ) {
        wk = mem_alloc( len + sizeof( inp_line ) );
        wk->next = input_cbs->hidden_head;
        wk->fmflags  = fmflags;
        wk->fm_symbol = false;
        wk->hh_tag = input_cbs->hh_tag;
        wk->ip_start = false;
        wk->sym_space = false;
        input_cbs->hh_tag = false;
        strcpy( wk->value, split_pos ); // save second part

        input_cbs->hidden_head = wk;
        if( input_cbs->hidden_tail == NULL ) {
            input_cbs->hidden_tail = wk;
        }

        *split_pos = '\0';              // terminate first part
        ProcFlags.indented_text = false;// split done
    }
    return;
}


/*  split_input_var
 *  The second part is constructed from 2 parts
 *  used if a substituted variable starts with CW_sep_char
 */

static void split_input_var( char * buf, char * split_pos, char * part2, i_flags fmflags )
{
    inp_line    *   wk;
    size_t          len;

    (void)buf;

    len = strlen( split_pos ) + strlen( part2 );// length of second part
    if( len > 0 ) {
        wk = mem_alloc( len + sizeof( inp_line ) );
        wk->next = input_cbs->hidden_head;
        wk->fmflags  = fmflags;
        wk->fm_symbol = false;
        wk->sym_space = sym_space;
        wk->hh_tag = input_cbs->hh_tag;

        strcpy(wk->value, part2 );      // second part
        strcat(wk->value, split_pos );  // second part

        input_cbs->hidden_head = wk;
        if( input_cbs->hidden_tail == NULL ) {
            input_cbs->hidden_tail = wk;
        }
    }
    return;
}


/***************************************************************************/
/*  look for GML tag start character and split line if valid GML tag       */
/*  don't split if blank follows gml_char                                  */
/*  special for  xxx::::TAG construct                                      */
/*  don't split if line starts with :CMT.                                  */
/***************************************************************************/
static void split_at_GML_tag( void )
{
    bool        layoutsw;
    char    *   p;
    char    *   p2;
    char    *   pchar;
    char        c;
    gmltag  *   gle = NULL;             // GML layout tag entry
    gmltag  *   gse = NULL;             // GML system tag entry
    gtentry *   gue = NULL;             // GML user tag entry
    size_t      toklen;

    if( IS_CMT_TAG( buff2 ) && IS_TAG_END( buff2 + 4 ) ) {
        return;                         // no split for :cmt. line
    }

    /***********************************************************************/
    /*  Look for GML tag start char(s) until a known tag is found          */
    /*  then split the line                                                */
    /***********************************************************************/
    pchar = strchr( buff2 + 1, GML_char );
    while( pchar != NULL ) {
        while( *(pchar + 1) == GML_char ) {
            pchar++;                    // handle repeated GML_chars
        }
        for( p2 = pchar + 1; is_id_char( *p2 ); p2++ )
            {;/* empty */}

        if( IS_CMT_TAG( pchar ) && IS_TAG_END( pchar + 4 ) ) {
            // is comment
            *pchar = '\0';
            return;
        }
        if( (p2 > pchar + 1)
          && (IS_TAG_END2( p2 )
          || (*p2 == GML_char)) ) { // 'good' tag end
            c = *p2;
            if( ProcFlags.layout
              && (c == '\t') ) {
                c = ' ';                // replace tab with space in layout
            }
            *p2 = '\0';                 // null terminate string
            toklen = p2 - pchar - 1;

            input_cbs->hh_tag = false;// clear before testing
            /***************************************************************/
            /* Verify valid user or system tag                             */
            /***************************************************************/
            if( ( (gue = find_tag( tag_dict, pchar + 1 )) != NULL )
              || ( (gse = (gmltag *) find_sys_tag( pchar + 1, toklen )) != NULL )
              || ( (gle = (gmltag *) find_lay_tag( pchar + 1, toklen )) != NULL ) ) {
                *p2 = c;

                if( !input_cbs->fm_hh ) {
                    // remove spaces before tags at sol in restricted sections
                    // in or just before LAYOUT tag
                    layoutsw = ProcFlags.layout;
                    if( !layoutsw
                      && (strncmp( "LAYOUT", pchar + 1, 6 ) == 0 ) ) {
                        layoutsw = true;
                    }
                    if( (rs_loc > 0)
                      || layoutsw ) {
                        p = buff2;
                        SkipSpacesTabs( p );
                        if( p == pchar ) {  // only leading blanks
                            memmove_s( buff2, buf_size, pchar, buf_size - (p - buff2) );
                            buff2_lg = strnlen_s( buff2, buf_size );// new length
                            pchar = strchr( buff2 + 1, GML_char );  // try next GMLchar
                            continue;       // dummy split done try again
                        }
                    }
                }
                split_input( buff2, pchar, input_cbs->fmflags );    // split at GML tag
                input_cbs->hidden_head->hh_tag = true;  // not just text starting with GML_char
                p = input_cbs->hidden_head->value + 1;
                p2 = token_buf;
                while( is_macro_char( *p ) ) {
                    *p2++ = *p++;
                }
                *p2 = '\0';
                /* inserting CONT_chars for layout tags causes errors */
                if( (gse != NULL)
                  || ((gue != NULL)
                  && (gue->tagflags & tag_cont)) ) {
                    ADD_CONT_CHAR( pchar );
                }
                if( ((gse != NULL)
                  && (gse->tagclass & ip_start_tag)) ) {
                    input_cbs->hidden_head->ip_start = true;
                }
                if( ProcFlags.literal ) {       // if literal active
                    if( li_cnt < INT_MAX ) {    // we decrement, adjust for split line
                        li_cnt++;
                    }
                }
                break;                  // we did a split stop now
            } else {
                *p2 = c;
            }
        }
        pchar = strchr( pchar + 1, GML_char );  // try next GMLchar
    }
}


/***************************************************************************/
/*   Split input line at GML tag                                           */
/*   returns false if :cmt .* .dm found and line should be skipped         */
/***************************************************************************/

static bool split_input_buffer( void )
{
    int                 k;

    /***********************************************************************/
    /*  look for GML tag start character and split line at GML tag         */
    /*  special processing for some script control lines                   */
    /***********************************************************************/

    split_at_GML_tag();

    if( !ProcFlags.literal ) {

        /*******************************************************************/
        /* for :cmt. minimal processing                                    */
        /*******************************************************************/

        if( IS_CMT_TAG( buff2 ) && buff2[4] == '.' ) {
            return( false );
        }

        /*******************************************************************/
        /*  .xx SCRIPT control line                                        */
        /*******************************************************************/

        if( (*buff2 == SCR_char) ) {

            if( buff2[1] == '*'
              || (buff2[1] == '\''
              && buff2[2] == '*') ) {
                return( false );  // for .* comment minimal processing
            }

            /***************************************************************/
            /* if macro define (.dm xxx ... ) supress variable substitution*/
            /* for the sake of single line macro definition                */
            /* .dm xxx / &*1 / &*2 / &*0 / &* /                            */
            /*  and                                                        */
            /* ..dm xxx / &*1 / &*2 / &*0 / &* /                           */
            /***************************************************************/
            if( *(buff2 + 1) == SCR_char ) {
                k = 2;
            } else {
                k = 1;
            }
            if( strnicmp( buff2 + k, "dm ", 3 ) == 0 ) {
                return( false );
            }

            /***************************************************************/
            /*  for records starting  .' which were not indented or which  */
            /*  were indented but all indents were .'                      */
            /*  or if control word separator is 0x00                       */
            /*  ignore control word separator                              */
            /***************************************************************/

            if( (!ProcFlags.CW_force_sep
              && (*(buff2 + k) == '\''))
              || (CW_sep_char == '\0') ) {
                ProcFlags.CW_sep_ignore = true;
            } else {
                ProcFlags.CW_sep_ignore = false;
            }
        } else if( ProcFlags.indented_text ) {       // text, but preceded by indent
            ProcFlags.CW_sep_ignore = false;
        }
    }
    return( true );                     // further processing needed
}


/****************************************************************************/
/*  set either or both of two ProcFlags if c_entry->value is all spaces     */
/*    ProcFlags.co_on_indent is set if the item is at the start of the      */
/*    physical input record                                                 */
/*    ProcFlags.pre_fsp is set if the item is at the end of the             */
/*    physical input record                                                 */
/*                                                                          */
/*    Note: this function is used by parse_r2l() only                       */
/*          it was removed because it is needed in three separate places    */
/*          in parse_r2l() and needed some effort to develop                */
/****************************************************************************/

static void set_space_flags( sym_list_entry * c_entry, char * buf )
{
    char    *   p;

    if( *c_entry->value != '\0' ) {     // result must have a value
        p = c_entry->value;
        SkipSpaces( p );
        if( *p == '\0' ) {
            if( (!input_cbs->fm_hh)
              && (c_entry->item.s == buf) ) {
                ProcFlags.co_on_indent = true;
            }
            p = c_entry->item.e;           // this the byte after the symbol value
            SkipDot( p );
            if( (*p == '\0')
              && (input_cbs->hidden_head == NULL) ) {
                ProcFlags.pre_fsp = true;
            }
        }
    }
}

/***************************************************************************/
/*  parse the current input buffer from right to left, using the stack of  */
/*  sym_list_entry instances created previously                            */
/*  this is the algorithm for unwinding the stack:                         */
/*    if curr->type is not sl_text, then                                   */
/*      if curr->type is sl_split, then                                    */
/*        if subscript, exit immediately                                   */
/*        if not subscript, split the line                                 */
/*      otherwise                                                          */
/*        copy text from curr->end on from buf to tail                     */
/*        copy the value from curr->value to curr->start                   */
/*        if the first character in the tail is ".", then                  */
/*          append tail+1 to buf thus skipping the "."                     */
/*        otherwise append tail to buf                                     */
/*    otherwise do nothing                                                 */
/***************************************************************************/

static bool parse_r2l( sym_list_entry * stack, char * buf, bool subscript )
{
    char            *   p;
    char                tail[BUF_SIZE];
    size_t              cw_lg;
    sym_list_entry  *   curr            = stack;

    ProcFlags.co_on_indent = false;
    ProcFlags.substituted = false;
    tail[0] = '\0';

    while( curr != NULL ) {
        switch( curr->type ) {
        case sl_text:
            if( subscript ) {
                ProcFlags.unresolved = true;
            }
            break;
        case sl_attrib:
            set_space_flags( curr, buf );
            ProcFlags.substituted = true;
            strcpy_s( tail, buf_size, curr->item.e );      // copy tail
            p = curr->item.s;
            strcpy_s( p, buf_size, curr->value );       // copy value
            if( tail[0] == '.' ) {
                strcat_s( buf, buf_size, tail + 1);     // append tail to buf, skipping initial "."
            } else {
                strcat_s( buf, buf_size, tail );        // append tail to buf
            }
            break;
        case sl_funct:
            set_space_flags( curr, buf );
            ProcFlags.substituted = true;
            p = curr->item.s;

            strcpy_s( tail, buf_size, curr->item.e );      // copy tail
            p = curr->item.s;
            strcpy_s( p, buf_size, curr->value );       // copy value
            if( tail[0] == '.' ) {
                strcat_s( buf, buf_size, tail + 1);     // append tail to buf, skipping initial "."
            } else {
                strcat_s( buf, buf_size, tail );        // append tail to buf
            }
            break;
        case sl_symbol:
            set_space_flags( curr, buf );
            ProcFlags.substituted = true;
            if( !ProcFlags.if_cond
              && !ProcFlags.dd_macro
              && !curr->value[0] ) {
                ProcFlags.null_value = true;
            }
            strcpy_s( tail, buf_size, curr->item.e );      // copy tail
            p = curr->item.s;
            /* If we're replacing &* with &*, we have a real problem. */
            if( ((p[0] == ampchar)
              && (p[1] == '*'))
              && ((curr->value[0] == ampchar)
              && (curr->value[1] == '*')) ) {
                internal_err( __FILE__, __LINE__ );
                ProcFlags.substituted = false;          // try to avoid infinite loop
                break;
            }
            strcpy_s( p, buf_size, curr->value );       // copy value
            if( tail[0] == '.' ) {
                strcat_s( buf, buf_size, tail + 1);     // append tail to buf, skipping initial "."
            } else {
                strcat_s( buf, buf_size, tail );        // append tail to buf
            }
            if( input_cbs->fm_symbol ) {
                /* keep value if from prior symbol which created its own logical input record */
                sym_space = input_cbs->sym_space;
            } else {
                sym_space = false;
                if( curr->item.s == buf ) {              // symbol at start of input line
                    sym_space = true;
                } else {                                // symbol not at start of input line
                    if( *curr->value == !SCR_char ) {   // not an scw or macro
                        sym_space = (*(curr->item.s - 1) == ' ');
                    }
                }
            }
            input_cbs->sym_space = sym_space;
            break;
        case sl_split:
            if( subscript ) {
                ProcFlags.substituted = false;
                ProcFlags.unresolved = true;
            } else {
                ProcFlags.substituted = true;
                if( !ProcFlags.if_cond
                  && !ProcFlags.dd_macro
                  && !curr->value[0] ) {
                    ProcFlags.null_value = true;
                }
                /* NB: In some cases there may be no hidden_head after splitting. */
                split_input_var( buf, curr->item.e, &curr->value[1], input_cbs->fmflags );
                if( input_cbs->hidden_head != NULL ) {
                    input_cbs->hidden_head->fm_symbol = true;   // new logical input record
                }
                cw_lg = 0;
                for( p = buf; *p != ' '; p++ )
                    cw_lg++;                                // length of . plus CW
                cw_lg++;                                    // plus space after CW
                if( input_cbs->hidden_head ) {
                    if( ProcFlags.script_cw
                      && (buf + cw_lg == curr->item.s) ) {
                        input_cbs->hidden_head->sym_space = false;  // space is space after cw
                    } else {
                        if( curr->item.s == buf ) {          // symbol at start of input line
                            input_cbs->hidden_head->sym_space = true;
                        } else {
                            input_cbs->hidden_head->sym_space = (*(curr->item.s - 1) == ' ');
                        }
                    }
                }
                *curr->item.s = '\0';
            }
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
        if( subscript
          && (curr->type == sl_split) ) {
            break;
        }
        curr = curr->prev;
    }
    add_sym_list_entry_to_pool( stack );
    return( ProcFlags.substituted );
}


static void expand_subscripts( char *buf, size_t sz, const symvar *var, sub_index lo_bound, sub_index hi_bound )
{
    const symsub    *   ws;
    bool                put_comma = false;

    *buf = '\0';                                // clear output buffer

    for( ws = var->subscripts; ws != NULL; ws = ws->next ) {    // lowest subscript
        if( ws->subscript > hi_bound )          // past the upper range?
            break;
        if( (ws->subscript >= lo_bound)
          && (ws->subscript <= hi_bound) ) {
            if( put_comma )
                strcat_s( buf, sz, ", " );
            strcat_s( buf, sz, ws->value );
            put_comma = true;
        }
    }
}


/***************************************************************************/
/*  scan for symbol start character (&) or, if processing a SCRIPT control */
/*  word record, also for a control word separator.                        */
/***************************************************************************/

static char * scan_sym_or_sep( char *buf, bool splittable )
{
    char        *   p;
    char        *   pa;

    p = NULL;
    for( pa = buf; *pa != '\0'; pa++ ) {
        if( *pa == ampchar ) {
            p = pa;
            break;
        } else if( *pa == CW_sep_char ) {
            if( splittable
              && (ProcFlags.script_cw
              || ProcFlags.CW_force_sep)
              && !ProcFlags.CW_sep_ignore ) {
                split_input( buff2, pa + 1, II_none );      // split after CW_sep_char

                buff2_lg = pa - buff2;      // update length after splitting
                *(buff2 + buff2_lg) = '\0'; // terminate first record
                break;
            }
        }
    }
    return( p );
}

/***************************************************************************/
/*  parse the current input buffer from left to right, building a stack of */
/*  sym_list_entry instances                                               */
/*  fully resolve all items                                                */
/*  if splittable is true, then:                                           */
/*    return when a symbol (as opposed to an attribute or a function) has  */
/*    a value which starts with the control word separator (";" by         */
/*    default) or the end of buf is reached                                */
/*  if splittable is false, then:                                          */
/*    return only when the end of buf is reached                           */
/***************************************************************************/

static sym_list_entry * parse_l2r( char * buf, bool splittable )
{
    char            *       p;
    char            *       pa;
    char            *   *   ppval;
    char            *       symstart;           // start of symbol name
    char                    valbuf[BUF_SIZE];   // buffer for attribute function and function values
    int                     rc;
    int32_t                 valsize;
    sub_index               var_ind;            // subscript value (if symbol is subscripted)
    symsub          *       symsubval;          // value of symbol
    symvar                  symvar_entry;
    sym_list_entry  *       curr    = NULL;     // current top-of-stack
    sym_list_entry  *       temp    = NULL;     // used to create new top-of-stack

    p = scan_sym_or_sep( buf, splittable );
    while( p != NULL ) {         // & found
        temp = alloc_sym_list_entry();
        if( curr == NULL ) {
            curr = temp;            // initialize stack
        } else {
            temp->prev = curr;      // push stack down
            curr = temp;
        }
        if( *(p + 1) == ' ' ) {  // not a symbol substition, attribute, or function
            curr->item.s = p;
            curr->item.e = p + 2;
            curr->type = sl_text;               // text
        } else if( my_isalpha( p[1] )
          && (p[2] == '\'') ) {   // attribute or text
            curr->item.s = p;
            if( (p[3] > ' ') ) {
                curr->item.e = curr->item.s;
                while( !is_space_tab_char( *curr->item.e ) && (*curr->item.e != '\0') && (*curr->item.e != '.') )
                    curr->item.e++;
                pa = valbuf;
                ppval = &pa;

                if( WgmlFlags.firstpass
                  && (input_cbs->fmflags & II_research) ) {
                    add_single_func_research( curr->item.s + 1 );
                }

                curr->item.e = scr_single_funcs( curr->item.s, curr->item.e, ppval );
                if( ProcFlags.unresolved ) {
                    curr->type = sl_text;
                } else {
                    curr->type = sl_attrib;
                    strcpy_s( curr->value, buf_size, valbuf );      // save value in current stack entry
                }
            } else {
                curr->type = sl_text;
                curr->item.e = curr->item.s + 3;
            }
        } else if( *(p + 1) == '\'' ) {          // function or text
            curr->item.s = p;
            p += 2;                             // over "&'"
            while( is_function_char(*p) ) p++;  // find end of function name
            if( *p == '(' ) {                   // &'xyz( is start of multi char function
                curr->item.e = curr->item.s;
                while( !is_space_tab_char( *curr->item.e ) && (*curr->item.e != '\0') && (*curr->item.e != '.') )
                    curr->item.e++;
                pa = valbuf;
                ppval = &pa;
                valsize = buf_size - (curr->item.e - curr->item.s);
                curr->item.e = scr_multi_funcs( curr->item.s, p, ppval, valsize );
                if( ProcFlags.unresolved ) {
                    curr->type = sl_text;
                } else {
                    strcpy_s( curr->value, buf_size, valbuf );  // save value in current stack entry
                    curr->type = sl_funct;
                }
            } else {
                curr->item.e = p;
                curr->type = sl_text;           // text
            }
        } else {                                // symbol
            if( (*(p+1) == '*')
              && (*(p+2) == ampchar) ) {  // special for &*&<var>
                curr->item.s = p;
                curr->item.e = p + 2;
                curr->type = sl_text;
                p = p + 2;
            } else {
                curr->item.s = p;
                p++;                                // over '&'
                symstart = p;                       // remember start of symbol name
                scan_err = false;
                ProcFlags.suppress_msg = true;
                pa = valbuf;
                ppval = &pa;
                p = scan_sym( symstart, &symvar_entry, &var_ind, ppval, splittable );
                curr->item.e = p;
                if( scan_err ) {                        // problem with subscript
                    if( ProcFlags.unresolved ) {
                        curr->type = sl_text;
                        if( *curr->item.e == '\0' ) {
                            break;                      // end of text terminates processing
                        }
                        p = curr->item.e;                  // skip argument
                        p = strchr( p, ampchar );       // look for next & in buffer
                    } else {
                        if( !ProcFlags.CW_sep_ignore
                          && splittable
                          && (CW_sep_char != 0x00)
                          && (valbuf[0] == CW_sep_char)
                          && (valbuf[1] != CW_sep_char) ) {
                            strcpy_s( curr->value, buf_size, valbuf );  // repurpose curr
                            curr->item.s = p + 1;                        // & of symbol causing split
                            curr->type = sl_split;
                            break;              // line split terminates processing
                        } else {
                            var_ind = atol( valbuf );       // save value for use
                            *curr->value = '\0';            // overwrite with nothing
                            curr->type = sl_text;
                        }
                    }
                } else {
                    if( symvar_entry.flags & local_var ) {  // lookup var in dict
                        rc = find_symvar_l( input_cbs->local_dict, symvar_entry.name, var_ind,
                                            &symsubval );
                    } else {
                        rc = find_symvar( global_dict, symvar_entry.name, var_ind, &symsubval );
                    }
                    // subscript expansion needs special handling
                    if( (var_ind == all_subscript)
                      || (var_ind == neg_subscript)
                      || (var_ind == pos_subscript) ) {
                        sub_index   lo_bound = min_subscript;
                        sub_index   hi_bound = max_subscript;

                        // adjust default bounds which go from min to max
                        switch( var_ind ) {
                        case neg_subscript:
                            hi_bound = 0;
                            break;
                        case pos_subscript:
                            lo_bound = 0;
                            break;
                        default:
                            break;
                        }
                        curr->type = sl_symbol;
                        expand_subscripts( curr->value, buf_size, symsubval->base, lo_bound, hi_bound );
                    } else if( rc == 2 ) {          // variable found + resolved
                        if( !ProcFlags.CW_sep_ignore
                          && splittable
                          && CW_sep_char != 0x00
                          && symsubval->value[0] == CW_sep_char
                          && symsubval->value[1] != CW_sep_char ) {
                            curr->type = sl_split;
                            strcpy_s( curr->value, buf_size, symsubval->value );  // save value in current stack entry
                            SkipDot( curr->item.e );
                            break;              // line split terminates processing
                        } else if( symsubval->base->flags & is_AMP ) {
                            curr->type = sl_text;   // save for late substitution
                        } else {
                            curr->type = sl_symbol;
                            strcpy_s( curr->value, buf_size, symsubval->value );  // save value in current stack entry
                        }
                    } else if( (rc == 1)
                      && ((var_ind == all_subscript)
                      || (var_ind == neg_subscript)
                      || (var_ind == pos_subscript)) ) {
                        sub_index   lo_bound = min_subscript;
                        sub_index   hi_bound = max_subscript;

                        // adjust default bounds which go from min to max
                        switch( var_ind ) {
                        case neg_subscript:
                            hi_bound = 0;
                            break;
                        case pos_subscript:
                            lo_bound = 0;
                            break;
                        default:
                            break;
                        }
                        curr->type = sl_symbol;
                        expand_subscripts( curr->value, buf_size, symsubval->base, lo_bound, hi_bound );
                    } else if( symvar_entry.flags & local_var ) {   // undefined locals are set to ''
                        curr->type = sl_symbol;
                        curr->value[0] = '\0';
                    } else {                                        // undefined global
                        curr->type = sl_text;
                        curr->item.e  = symstart;                      // rescan for CW separator past the &
                    }
                }
            }
        }
        if( *curr->item.e == '\0' ) {
            break;                      // end of text terminates processing
        }
        p = curr->item.e;                  // skip argument
        p = scan_sym_or_sep( p, splittable );
    }
    return( curr );
}


/***************************************************************************/
/*  symbolic variables, single-letter attribute functions and multi-letter */
/*  functions are resolved here                                            */
/*  currently, this is done using two helper functions                     */
/***************************************************************************/

bool resolve_symvar_functions( char * buf, bool splittable )
{
    bool                anything_substituted    = false;
    sym_list_entry  *   stack;

    if( buf == NULL ) {
        return( false );                    // no text to process
    }
    stack = parse_l2r( buf, splittable );
    if( stack == NULL ) {
        return( false);                     // no stack to process
    }
    parse_r2l( stack, buf, false );
    anything_substituted |= ProcFlags.substituted;
    while( ProcFlags.substituted ) {
        stack = parse_l2r( buf, splittable );
        if( stack == NULL ) {
            break;                      // no stack to process
        }
        parse_r2l( stack, buf, false );
        anything_substituted |= ProcFlags.substituted;
    }
    return( anything_substituted );
}


/***************************************************************************/
/*  finalize the subscript value                                           */
/*  simplified version of resolve_symvar_functions()                       */
/*  no need to return an indicator if something has been substituted       */
/***************************************************************************/

void finalize_subscript( char * * result, bool splittable )
{
    sym_list_entry  *   stack;

    stack = parse_l2r( *result, splittable );
    if( stack == NULL ) {
        return;                             // no stack to process
    }
    parse_r2l( stack, *result, true );
    while( ProcFlags.substituted ) {
        stack = parse_l2r( *result, splittable );
        if( stack == NULL ) {
            break;                      // no stack to process
        }
        parse_r2l( stack, *result, true );
    }

    return;
}


/***************************************************************************/
/*  set  some switches if char is script controlword start or              */
/*  gml tag start                                                          */
/***************************************************************************/

void classify_record( char c )
{
    if( c == GML_char ) {               // classify input
        ProcFlags.gml_tag = true;
        ProcFlags.script_cw = false;
        ProcFlags.CW_force_sep = false;
    } else {
        ProcFlags.gml_tag = false;
        if( c == SCR_char ) {
            ProcFlags.script_cw = true;
        } else {
            ProcFlags.script_cw = false;
        }
    }
}


/***************************************************************************/
/*  Possibly reclassify record after substitution. There may be a SCRIPT   */
/*  control word, but not a new GML tag.                                   */
/***************************************************************************/

static void reclassify_record( char c )
{
    // if record was a GML tag, it will stay a GML tag, unless it's now
    // a SCRIPT control word (if that's even possible). But a record that
    // wasn't already a GML tag will *not* become one after substitution.
    if( c == SCR_char ) {
        ProcFlags.script_cw  = true;
        ProcFlags.gml_tag = false;
    } else {
        ProcFlags.script_cw  = false;
    }
}


/***************************************************************************/
/*  remove leading space                                                   */
/*  to catch lines such as: "    :TAG xyz."  -> ":TAG xyz."                */
/*  returns true if spaces removed                                         */
/***************************************************************************/

static bool remove_leading_space( void )
{
    char    * p;
    char    * p2;
    bool    removed = false;

    if( ProcFlags.literal
      || !ProcFlags.concat ) {  // .li active or .co OFF
        return( false );        // don't change input
    }
    p = buff2;
    SkipSpacesTabs( p );
    if( (p != buff2)
      && (*p == GML_char)
      && (cur_group_type != gt_xmp) ) {
        p2 = buff2;
        do {
            *p2++ = *p++;
        } while( *p != '\0' );
        *p2++ = '\0';
        *p2   = '\0';
        removed = true;
    }
    return( removed );
}


/***************************************************************************/
/*  take the contents of the input line in buff2 and try to make the best  */
/*  of it                                                                  */
/*  Processing as specified in wgmlref.pdf chapter 8.1 processing rules    */
/*  and results from testing with wgml 4                                   */
/*                                        imcomplete               TBD     */
/*                                                                         */
/***************************************************************************/

void process_line( void )
{
    bool    anything_substituted;

    anything_substituted = remove_leading_space();// for "   :TAG   " constructs

    classify_record( *buff2 );      // classify script CW, GML tag or nothing

    if( !split_input_buffer() ) {
        return;                         // .* .dm :cmt found
    }

    /***************************************************************************/
    /*  pre-identify IF statements                                             */
    /*  this is needed to avoid an empty symbol from having it's usual effect  */
    /*  when a line like                                                       */
    /*    .if &fred eq ''                                                      */
    /*  is used and "fred" has a null value (matching '')                      */
    /*  EL is also checked to handle lines like                                */
    /*    .el .if &fred eq ''                                                  */
    /*  note that symbol substition is done before IF is actually processed    */
    /*  this can probably be improved                                          */
    /***************************************************************************/
    if( ProcFlags.script_cw
      && (((toupper(*(buff2 + 1)) == 'I')
      && (toupper(*(buff2 + 2)) == 'F'))
      || ((toupper(*(buff2 + 1)) == 'E')
      && (toupper(*(buff2 + 2)) == 'L'))) ) {
        ProcFlags.if_cond = true;
    } else {
        ProcFlags.if_cond = false;
    }
    if( ProcFlags.indented_text ) {
        anything_substituted |= resolve_symvar_functions( buff2, false );
        ProcFlags.indented_text = false;
    } else {
        anything_substituted |= resolve_symvar_functions( buff2, true );
    }

    reclassify_record( *buff2 );    // reclassify SCRIPT/text after symbol substitution etc

    buff2_lg = strnlen_s( buff2, buf_size );

    if( (input_cbs->fmflags & II_research)
      && WgmlFlags.firstpass
      && anything_substituted ) {
        g_info_lm( inf_subst_line, buff2 ); // show line with substitution(s)
    }

    scan_start = buff2;
    scan_stop  = buff2 + buff2_lg;
    return;
}

