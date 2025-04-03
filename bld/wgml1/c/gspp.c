/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  implements control word PP
*
****************************************************************************/


#include "wgml.h"


/**************************************************************************/
/* PARAGRAPH defines the start of a new paragraph.                        */
/*                                                                        */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커      */
/*      |       |                                                  |      */
/*      |  .PP  | <line>                                           |      */
/*      |       |                                                  |      */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸      */
/*                                                                        */
/* This control word causes a break, and then internally executes a .SK,  */
/* a .CC 2, and an .IL +3. The "line" operand starts one blank after the  */
/* control word. If the operand is missing, the text for the start of     */
/* the paragraph comes from the next input text line.                     */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/* Implements control word PP                                             */
/**************************************************************************/

void scr_pp( void )
{
    char    *   p;

    p = scan_start;

    /* BR is done before scr_pp() is called */

    /* SK */
    g_skip = 1;
    set_skip_vars( NULL, NULL, NULL, g_text_spacing, g_curr_font );

    /* CC 2 */
    if( (wgml_fonts[g_curr_font].line_height + t_page.cur_depth) > t_page.max_depth ) {
        next_column();
    }

    /* IL +3 */
    g_line_indent = 3 * tab_col;
    t_page.cur_width = t_page.cur_left + g_line_indent;

    post_space = 0;
    if( *p != '\0' ) {
        process_text( p, g_curr_font );
    }

    scan_restart = scan_stop + 1;
    return;
}

