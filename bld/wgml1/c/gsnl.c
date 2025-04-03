/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  currently, non-implementation of control word NL
*
****************************************************************************/


#include "wgml.h"


/**************************************************************************/
/* NULL LINE is generated internally by SCRIPT each time a null input     */
/* line is encountered.                                                   */
/*                                                                        */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커      */
/*      |       |                                                  |      */
/*      |  .NL  |                                                  |      */
/*      |       |                                                  |      */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸      */
/*                                                                        */
/* This control word does not cause a break. Any operands specified will  */
/* be ignored. Any pending input continuation will be cleared if the      */
/* previous input line ended with a continuation character.               */
/*    A null input record may result from Terminal input (.TE), from a    */
/* macro definition line or as a result of substitution of a set symbol   */
/* that has a null value. If some other effect is desired for null input  */
/* records then you may define a macro named "NL" to perform whatever is  */
/* required.                                                              */
/*    An input line that contains only, one or more blank characters is   */
/* processed by the Blank Line (.BL) control word. A null input line is   */
/* different from a blank input line.                                     */
/*                                                                        */
/**************************************************************************/

/************************************************************************/
/* this simply skips rest of the logical record                         */
/* the reason is that the OW Docs do not need it, but one does use it   */
/* NOTE: this is, actually, almost the implementation defined in the    */
/*       TSO; the only thing to add is to cancel the continuation       */
/*       character -- if and when that feature is implemented           */
/************************************************************************/

void scr_nl( void )
{
    scan_restart = scan_stop + 1;
    return;
}
