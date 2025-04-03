/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: implement .oc (output comment) script control word
*
*
*  comments are from script-tso.txt
****************************************************************************/


#include "wgml.h"


/**************************************************************************/
/* OUTPUT COMMENT causes one line of  information to be inserted into the */
/* output as a comment.                                                   */
/*                                                                        */
/*      ------------------------------------------------------------      */
/*      |       |                                                  |      */
/*      |  .OC  |    <information>                                 |      */
/*      |       |                                                  |      */
/*      ------------------------------------------------------------      */
/*                                                                        */
/* This  control word  does not  cause a  break.   If  specified with  no */
/* operand, no action is taken.  The operand line is not itself formatted */
/* and  does not  add to  the count  of lines  on the  output page  being */
/* formatted.   The  comment line  is immediately  written to  the output */
/* file.   Thus  if it  were to occur  within a  paragraph or  within the */
/* BEGIN/END range of an "in-storage" text  block (.CC,  .FN,  etc),  the */
/* position of the comment  in the output would not agree  with its posi- */
/* tion in the input.   This would  be most noticeable in multiple-column */
/* mode.   This  control word would normally  be used when  the formatted */
/* output is  to be re-processed by  another program,  an editor,   or an */
/* intelligent output device.                                             */
/*                                                                        */
/* NOTES                                                                  */
/* (1) If multiple  passes are in effect,   the Output Comments  are only */
/*     written during the last pass.                                      */
/* (2) Output Comments are  processed even if the current page  is not to */
/*     be displayed because of FROM page options.                         */
/**************************************************************************/

void    scr_oc( void )
{
    char        *   p;

    if( WgmlFlags.lastpass ) {
        p = scan_start;                 // next char after .oc
        if( *p != '\0' ) {              // line operand specified
            SkipSpaces( p );
            if( *p != '\0' ) {
                ob_oc( p );
                if( input_cbs->fmflags & II_research ) {
                    out_msg( p );       // TBD
                    out_msg( "<-.oc.oc.oc\n" ); // TBD
                }
            }
        }
    }
    scan_restart = scan_stop + 1;
    return;
}

