/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML tags :DOCNUM processing
*
****************************************************************************/


#include    "wgml.h"


/***************************************************************************/
/*  :docnum.docnum   tag                                                       */
/***************************************************************************/

void    gml_docnum( const gmltag * entry )
{
    char        *   buff            = NULL;
    char        *   p;
    font_number     font_save;
    page_pos        old_line_pos;
    size_t          buff_len;
    uint32_t        left_indent;
    uint32_t        right_indent;


    if( !((ProcFlags.doc_sect == doc_sect_titlep) ||
          (ProcFlags.doc_sect_nxt == doc_sect_titlep)) ) {
        xx_nest_err_cc( err_tag_wrong_sect, entry->tagname, ":TITLEP section" );
    }

    if( ProcFlags.docnum_tag_seen ) {   // only one DOCNUM tag allowed
        xx_line_err_c( err_2nd_docnum, buff2 );
    }

    p = scan_start;
    SkipDot( p );                       // over . to docnum
    SkipSpaces( p );                    // over WS to attribute

    /* Prepend the docnum_string, if any */

    if( layout_work.docnum.string != NULL ) {
        buff_len = strlen( layout_work.docnum.string ) + strlen ( p ) + 1;
        buff = mem_alloc( buff_len );
        strcpy_s( buff, buff_len, layout_work.docnum.string );
        strcat_s( buff, buff_len    , p );
        p = buff;
    }

    if( WgmlFlags.firstpass && *p != '\0' ) {
        add_symvar( global_dict, "$docnum", p, no_subscript, 0 );
    }
    ProcFlags.docnum_tag_seen = true;

    scr_process_break();
    start_doc_sect();                       // if not already done

    font_save = g_curr_font;
    g_curr_font = layout_work.docnum.font;
    g_text_spacing = layout_work.titlep.spacing;

    /************************************************************/
    /*  pre_skip is treated as pre_top_skip because it is       */
    /*  always used at the top of the page, despite the docs    */
    /************************************************************/

    set_skip_vars( NULL, &layout_work.docnum.pre_skip, NULL, g_text_spacing, g_curr_font );


    left_indent = conv_hor_unit( &layout_work.docnum.left_adjust, g_curr_font );
    right_indent = conv_hor_unit( &layout_work.docnum.right_adjust, g_curr_font );

    t_page.cur_left += left_indent;
    t_page.cur_width = t_page.cur_left;
    if( t_page.max_width < right_indent ) {
        xx_line_err_c( err_page_width_too_small, val_start );
    } else {
        t_page.max_width -= right_indent;
    }
    ProcFlags.keep_left_margin = true;  // keep special indent
    old_line_pos = line_position;
    line_position = layout_work.docnum.page_position;
    ProcFlags.as_text_line = true;
    if( *p != '\0' ) {
        process_text( p, g_curr_font );
    } else {
        ProcFlags.titlep_starting = true;
    }
    scr_process_break();                // commit docnum line (or blank line)

    if( buff != NULL ) {
        mem_free( buff );
    }

    g_curr_font = font_save;
    line_position = old_line_pos;
    scan_start = scan_stop + 1;
}
