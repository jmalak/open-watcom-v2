/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  currently, non-implementation of control word CS
*
****************************************************************************/


#include "wgml.h"


/**************************************************************************/
/* CONDITIONAL SECTION provides a means of creating a section of text     */
/* and/or control words that will only be included during SCRIPT          */
/* processing under the specified condition.                              */
/*                                                                        */
/*       旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커     */
/*       |       |                                                  |     */
/*       |       | <n> <ON|OFF>                                     |     */
/*       |  .CS  |                                                  |     */
/*       |       | <n> <INCLUDE|IGNORE>                             |     */
/*       |       |                                                  |     */
/*       읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸     */
/*                                                                        */
/* <ON|OFF>: ON defines the start of a conditional section that will      */
/*    never be processed by SCRIPT, and OFF defines the end of that       */
/*    block.                                                              */
/* n <INCLUDE|IGNORE>: "n" specifies the number of a conditional-section  */
/*    block (from 1 to 99) that is either to be INCLUDEd or IGNOREd       */
/*    during SCRIPT processing. INCLUDE is the initial and default state  */
/*    for all 99 block numbers.                                           */
/* n <ON|OFF>: ON defines the start of a conditional-section block that   */
/*    will or will not be processed depending on the INCLUDE/IGNORE state */
/*    currently in effect for "conditional section n" blocks.             */
/*                                                                        */
/* This control word does not cause a break. For each of the 99 block     */
/* numbers, any number of conditional sections may occur. When SCRIPT     */
/* encounters the ON of an un-numbered conditional-section block or of a  */
/* numbered block for which the state is IGNORE, it goes into a special   */
/* "read input and throw it away" mode until it finds the OFF of that     */
/* block.                                                                 */
/*                                                                        */
/* EXAMPLES                                                               */
/* (1) .cs 1 ignore                                                       */
/* This is a                                                              */
/* .cs 1 on;Introductory User's                                           */
/* .cs 1 off                                                              */
/* .cs 2 on;Reference                                                     */
/* .cs 2 off                                                              */
/* Manual.                                                                */
/* produces: This is a Reference Manual.                                  */
/* (2) .if &debug eq no .th .cs on                                        */
/* .ty Imbedding the CHECKER file;.im CHECKER                             */
/* .cs off                                                                */
/* The message about and the imbedding of CHECKER will not occur if       */
/* &debug has the value "no".                                             */
/**************************************************************************/

/************************************************************************/
/* this simply skips rest of the logical record                         */
/* the reason is that the OW Docs do not need it, but they do use it    */
/* they use it in an apparent attempt to allow examples to be hidden,   */
/* but never use IGNORE to actually hide them!                          */
/************************************************************************/

void scr_cs( void )
{
    scan_restart = scan_stop + 1;
    return;
}
