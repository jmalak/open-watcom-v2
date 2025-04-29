/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: implement ...label and .go script control words
*               not all options are used / implemented
*
*  comments are from script-tso.txt
****************************************************************************/


#include "wgml.h"


static char         gotarget[LABEL_NAME_LENGTH + 1] = { '\0' };
static int32_t      gotargetno = 0;

/***************************************************************************/
/*  search for  label name in current input label control block            */
/***************************************************************************/

static  labelcb *   find_label( char    *   name )
{
    labelcb *   lb;

    if( input_cbs->fmflags & II_tag_mac ) {
        lb = input_cbs->s.m->mac->label_cb;
    } else {
        lb = input_cbs->s.f->label_cb;
    }
    for( ; lb != NULL; lb = lb->prev ) {
        if( strncmp( name, lb->label_name, LABEL_NAME_LENGTH ) == 0 ) {
            return( lb );
        }
    }
    return( NULL );
}


/***************************************************************************/
/*  check whether current input line is the active go to target            */
/***************************************************************************/

void    gotarget_err( void )
{
    char    linestr[MAX_L_AS_STR];

    if( input_cbs->fmflags & II_tag_mac ) {
        if( gotargetno > 0 ) {
            uinttodec( gotargetno, linestr );
            xx_err_cc( err_goto, linestr, input_cbs->s.m->mac->macname );
        } else {
            xx_err_cc( err_goto, gotarget, input_cbs->s.m->mac->macname );
        }
    } else {
        xx_err_cc( err_goto, gotarget, input_cbs->s.f->filename );
    }
    gotargetno = 0;
    *gotarget = '\0';
}


bool        gotarget_reached( void )
{
    bool        reached;
    char    *   p;
    int         k;

    reached = false;
    if( gotargetno > 0 ) {              // lineno search
        if( input_cbs->fmflags & II_tag_mac ) {
            reached = input_cbs->s.m->lineno == gotargetno;
        } else {
            reached = input_cbs->s.f->lineno == gotargetno;
        }
    } else {                            // label name search
        p = buff2;
        if( p[0] == '.' && p[1] == '.' && p[2] == '.' ) { // "..."
            p += 3;
            SkipSpaces( p );
            if( *p != '\0' ) {
                k = 0;
                while( gotarget[k] && *p == gotarget[k] ) {
                    k++;
                    p++;
                }
                if( gotarget[k] == '\0' && ((*p == ' ') || (*p == '\0')) ) {
                    reached = true;
                }
            }
        }
    }
    return( reached );
}


/***************************************************************************/
/*  check whether new label is duplicate                                   */
/***************************************************************************/

static  condcode    test_duplicate( char *name, line_number lineno )
{
    labelcb     *   lb;

    lb = find_label( name );
    if( lb == NULL ) {
        return( omit );                 // really new label
    }
    if( lb->lineno == lineno ) {
        return( pos );                  // name and lineno match
    } else {
        return( neg);                   // name matches, different lineno
    }
}


/**************************************************************************/
/* ... (SET LABEL) defines an input line that has a "label".              */
/*                                                                        */
/*      ����������������������������������������������������������Ŀ      */
/*      |       |                                                  |      */
/*      |  ...  |    <label|n>  <line>                             |      */
/*      |       |                                                  |      */
/*      ������������������������������������������������������������      */
/*                                                                        */
/* A blank is not required between the ... and the label.                 */
/*                                                                        */
/* ...label <line>:  Labels are used as "target" lines for the GO control */
/*    word.   A label may consist of  a maximum of eight characters,  and */
/*    must be unique within the input file, Macro,  or Remote in which it */
/*    is defined.    Labels are stored  internally by SCRIPT  entirely in */
/*    uppercase; therefore, "...hi" and "... HI" are identical.           */
/* ...n <line>:  Numeric  labels are used to verify that  the line really */
/*    is the "nth" record in the input file,  Macro,  or Remote.   If the */
/*    verification fails, SCRIPT generates an error message.              */
/*                                                                        */
/* This  control word  does not  cause a  break.   The  optional text  or */
/* control-word "line", starting one blank after the label,  is processed */
/* after the  label field is scanned.    If "line" is omitted,   no other */
/* action is  performed.   Control  words within  the "line"  operand may */
/* cause a break.                                                         */
/*                                                                        */
/* EXAMPLES                                                               */
/* (1) ...skip .sk 2                                                      */
/*     This defines a "SKIP" label on a "skip two" statement.             */
/* (2) ... 99 This had better be line ninety-nine.                        */
/*     This verifies that the line of text occurs in input line 99 of the */
/*     current input file, or results in an error message if not.         */
/**************************************************************************/

void    scr_label( void )
{
    condcode        cc;
    getnum_block    gn;
    labelcb     *   lb;
    char            linestr[MAX_L_AS_STR];

    SkipSpaces( scan_start );       // may be ...LABEL or ...<blanks>LABEL, skip over blanks
    if( *scan_start == '\0'  ) {    // no label?
        xx_source_err_c( err_missing_name, "" );
    } else {

        gn.arg.s = scan_start;
        gn.arg.e = scan_stop;
        gn.ignore_blanks = false;
        cc = getnum( &gn );             // try numeric expression evaluation
        if( cc == pos ) {               // numeric linenumber

            scan_start = gn.arg.s;   // start for next token

            // check if lineno from label matches actual lineno

            if( input_cbs->fmflags & II_tag_mac ) {
                if( gn.result != input_cbs->s.m->lineno ) {
                    xx_source_err_c( err_label_line, gn.resultstr );
                }
            } else {
                if( gn.result != input_cbs->s.f->lineno ) {
                    xx_source_err_c( err_label_line, gn.resultstr );
                }
            }

            if( input_cbs->fmflags & II_tag_mac ) {
                  // numeric macro label no need to store
            } else {
                uinttodec( input_cbs->s.f->lineno, linestr );
                xx_warn_info_cc( wng_label_num, inf_file_line, linestr, input_cbs->s.f->filename );
            }

        } else {                        // no numeric label
            cc = getarg();
            if( cc == pos ) {           // label name specefied
                char    *   p;
                char    *   pt;
                int         len;

                p   = g_tok_start;
                pt  = token_buf;
                len = 0;
                while( len < arg_flen ) {   // copy to buffer
                    *pt++ = *p++;
                    len++;
                }
                *pt = '\0';
                if( len > LABEL_NAME_LENGTH ) {
                    xx_source_err_c( err_sym_long, token_buf );
                }

                if( input_cbs->fmflags & II_tag_mac ) {

                    cc = test_duplicate( token_buf, input_cbs->s.m->lineno );
                    if( cc == pos ) {   // ok name and lineno match
                        // nothing to do
                    } else {
                        if( cc == neg ) {   // name with different lineno
                            xx_source_err_c( err_label_dup, token_buf );
                        } else {        // new label
                            lb              = mem_alloc( sizeof( labelcb ) );
                            lb->prev        = input_cbs->s.m->mac->label_cb;
                            input_cbs->s.m->mac->label_cb = lb;
                            lb->pos         = 0;
                            lb->lineno      = input_cbs->s.m->lineno;
                            strcpy_s( lb->label_name, sizeof( lb->label_name ), token_buf );
                        }
                    }
                } else {
                    cc = test_duplicate( token_buf, input_cbs->s.f->lineno );
                    if( cc == pos ) {   // ok name and lineno match
                        // nothing to do
                    } else {
                        if( cc == neg ) {   // name with different lineno
                            xx_source_err_c( err_label_dup, token_buf );
                        } else {        // new label

                            lb              = mem_alloc( sizeof( labelcb ) );
                            lb->prev        = input_cbs->s.f->label_cb;
                            input_cbs->s.f->label_cb = lb;
                            lb->pos         = input_cbs->s.f->pos;
                            lb->lineno      = input_cbs->s.f->lineno;
                            strcpy_s( lb->label_name, sizeof( lb->label_name ), token_buf );
                        }
                    }
                }
            } else {
                xx_source_err_c( err_missing_name, "" );
            }
        }

        if( *scan_start == ' ' ) {
            scan_start++;               // skip one blank

            if( *scan_start ) {         // rest of line is not empty
                split_input( buff2, scan_start, input_cbs->fmflags );   // split and process next
            }
        }
        scan_restart = scan_stop + 1;
        return;
    }
}


/***************************************************************************/
/* GOTO transfers processing  to the specified input line  in the current  */
/* file or macro.                                                          */
/*                                                                         */
/*      ����������������������������������������������������������Ŀ       */
/*      |       |                                                  |       */
/*      |  .GO  |    <label|n|+n|-n>                               |       */
/*      |       |                                                  |       */
/*      ������������������������������������������������������������       */
/*                                                                         */
/*                                                                         */
/* <label>:   The specified  label  will be  converted  to uppercase  and  */
/*    processing will transfer  to the line defined  by the corresponding  */
/*    SET LABEL  (...)  control  word within  the current  input file  or  */
/*    Macro.                                                               */
/* <n|+n|-n>:  Alternatively,  an absolute line number or signed relative  */
/*    line number  may be specified.    In either case,   processing will  */
/*    continue at the specified line.                                      */
/*                                                                         */
/* This control word does not cause a break.  The transfer of control may  */
/* be forward or backward within a Macro and within an input file that is  */
/* on a  DASD device (Disk),   but may only be  forward in an  input file  */
/* being processed from a Unit Record device (Card Reader).   If the GOTO  */
/* control word is  used within an If  or Nested If,  SCRIPT  cannot know  */
/* where it will be positioned in any If structure.  After a GOTO, SCRIPT  */
/* will therefore assume  it is no longer  in the range of  any If struc-  */
/* ture.                                                                   */
/*                                                                         */
/* EXAMPLES                                                                */
/* (1) .if '&*1' eq DONE .go DONE                                          */
/*     Not finished yet.                                                   */
/*     ...DONE The End.                                                    */
/* (2) This example formats a produces a list of the first fifty numbers:  */
/*     .sr i=0                                                             */
/*     ...loop .sr i = &i + 1                                              */
/*     &i                                                                  */
/*     .if &i lt 50 .go loop                                               */
/*                                                                         */
/*                                                                         */
/* ! goto linenumber is not implemented for FILE, only for MACRO           */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

void    scr_go( void )
{
    condcode        cc;
    getnum_block    gn;
    labelcb     *   golb;
    int             k;

    input_cbs->if_cb->if_level = 0;     // .go terminates
    ProcFlags.keep_ifstate = false;     // ... all .if controls

    cc = getarg();
    if( cc != pos ) {
        xx_source_err_c( err_missing_name, "" );
    }

    gn.arg.s = g_tok_start;
    gn.arg.e = scan_stop;
    gn.ignore_blanks = false;
    cc = getnum( &gn );             // try numeric expression evaluation
    if( cc == pos  || cc  == neg) {     // numeric linenumber
        gotarget[0] = '\0';             // no target label name
        if( gn.num_sign == ' '  ) {     // absolute number
            gotargetno = gn.result;
        } else {
            if( input_cbs->fmflags & II_tag_mac ) {
                gotargetno = input_cbs->s.m->lineno;
            } else {
                gotargetno = input_cbs->s.f->lineno;
            }
            gotargetno += gn.result;    // relative target line number
        }

        if( gotargetno < 1 ) {
            xx_source_err( err_label_zero );
        }
        if( input_cbs->fmflags & II_tag_mac ) {
            if( gotargetno <= input_cbs->s.m->lineno ) {
                input_cbs->s.m->lineno = 0; // restart from beginning
                input_cbs->s.m->macline = input_cbs->s.m->mac->macline;
            }
        }
    } else {                            // no numeric target label

        gotargetno = 0;                 // no target lineno known
        if( arg_flen > LABEL_NAME_LENGTH ) {
            xx_source_err_c( err_sym_long, g_tok_start );
        }

        for( k = 0; k < LABEL_NAME_LENGTH; k++ ) {// copy to work
            gotarget[k] = *g_tok_start++;
        }
        gotarget[k] = '\0';

        golb = find_label( gotarget );
        if( golb != NULL ) {            // label already known
            gotargetno = golb->lineno;

            if( input_cbs->fmflags & II_tag_mac ) {
                if( golb->lineno <= input_cbs->s.m->lineno ) {
                    input_cbs->s.m->lineno = 0; // restart from beginning
                    input_cbs->s.m->macline = input_cbs->s.m->mac->macline;
                }
            } else {
                if( golb->lineno <= input_cbs->s.f->lineno ) {
                    fsetpos( input_cbs->s.f->fp, &golb->pos );
                    input_cbs->s.f->lineno = golb->lineno - 1;// position file
                }
            }
        }
    }
    free_lines( input_cbs->hidden_head );   // delete split line
    input_cbs->hidden_head = NULL;
    input_cbs->hidden_tail = NULL;
    ProcFlags.goto_active = true;       // special goto processing
    scan_restart = scan_stop + 1;

}


/***************************************************************************/
/*  print list of defined labels for macro or file                         */
/***************************************************************************/

void        print_labels( labelcb * lcb, char * name )
{
    char                fill[LABEL_NAME_LENGTH + 1];
    size_t              len;
    labelcb         *   lb;

    lb = lcb;
    if( lb != NULL ) {
        out_msg( "\nList of defined labels for %s:\n\n", name );
        while( lb != NULL ) {
            len = LABEL_NAME_LENGTH - strlen( lb->label_name );
            memset( fill, ' ', len );
            fill[len] = '\0';
            out_msg( "Label='%s'%s at line %d\n", lb->label_name, fill, lb->lineno );
            lb = lb->prev;
        }
    }
}

