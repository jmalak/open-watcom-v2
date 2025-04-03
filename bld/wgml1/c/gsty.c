/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: implement .ty script control word
*
*  comments are from script-tso.txt
****************************************************************************/


#include "wgml.h"


/**************************************************************************/
/* TYPE displays a line of information at the terminal.                   */
/*                                                                        */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커      */
/*      |       |                                                  |      */
/*      |  .TY  |    information                                   |      */
/*      |       |                                                  |      */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸      */
/*                                                                        */
/* This control word does not cause a break.  If used with no operands, a */
/* blank line is displayed.   The text  of the line displayed is modified */
/* by the .TR (Translate) characters currently in effect.                 */
/*                                                                        */
/* NOTES: in wgml 4.0, input translation (.TI) is not used with .TY       */
/*        in wgml 4.0, output translation (.TR) is not used with .TY      */
/*        in wgml 4.0, initial blank spaces are not output                */
/*                                                                        */
/* EXAMPLES                                                               */
/* (1) The .TY  control is useful  immediately preceding a  .TE (Terminal */
/*     Input) or .RV (Read Variable)  control word,  as a prompt for what */
/*     to type in response:                                               */
/*       .ty Type Name and Address in five lines or less:                 */
/*       .te 5                                                            */
/* (2) Prompt user for an input variable:                                 */
/*       .ty Specify number of output columns:                            */
/*       .rv ncols                                                        */
/*       .cd set &ncols                                                   */
/**************************************************************************/

void    scr_ty( void )
{
    char    *   p;

    p = scan_start;
    SkipSpaces( p );
    out_msg( "%s\n", p );
    scan_restart = scan_stop + 1;
    return;
}

