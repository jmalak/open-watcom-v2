/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML tags :XMP and :eXMP and helper functions
*
****************************************************************************/


#include    "wgml.h"


static  bool        concat_save;            // for ProcFlags.concat
static  bool        first_xline;            // special for first xmp LINE
static  font_number font_save;              // save for g_curr_font
static  group_type  sav_group_type;         // save prior group type
static  ju_enum     justify_save;           // for ProcFlags.justify

/***************************************************************************/
/*      :XMP [depth='vert-space-unit'].                                    */
/*           <paragraph elements>                                          */
/*           <basic document elements>                                     */
/* This tag signals the start of an example.  Each line of source text     */
/* following the example tag is placed in the output document without      */
/* normal text processing.  Spacing between words is preserved, and the    */
/* input text is not right justified.  Input source lines which do not fit */
/* on a line in the output document are split into two lines on a          */
/* character, rather than a word basis.  An example may be used where a    */
/* basic document element is permitted to appear, except within a figure,  */
/* footnote, or example.  A corresponding :exmp tag must be specified for  */
/* each :xmp tag.  If the example does not fit on the current page or      */
/* column, it is forced to the next one.  If the current column is empty,  */
/* the example will be split into two parts.  The depth attribute accepts  */
/* vertical space units as possible values.  The amount of specified       */
/* vertical space is created in the output before any source input text is */
/* processed.  The value of the depth attribute is linked to the current   */
/* font                                                                    */
/*                                                                         */
/***************************************************************************/

void gml_xmp( const gmltag * entry )
{
    char        *   p;
    char        *   pa;
    su              cur_su;
    uint32_t        depth;

    (void)entry;

    start_doc_sect();
    script_process_break();
    scan_err = false;

    g_keep_nest( "Example" );           // catch nesting errors

    if( is_ip_tag( nest_cb->c_tag ) ) {                 // inline phrase not closed
        g_err_tag_nest( str_tags[nest_cb->c_tag + 1] ); // end tag expected
    }

    font_save = g_curr_font;
    g_curr_font = layout_work.xmp.font;
    depth = 0;                          // default value: depth will be depth of box contents
    p = scan_start;
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            pa = get_att_start( p );
            p = g_att_start;
            if( ProcFlags.reprocess_line ) {
                break;
            }

            if( strnicmp( "depth", p, 5 ) == 0 ) {
                p += 5;
                p = get_att_value( p );
                if( val_start == NULL ) {
                    break;
                }
                if( att_val_to_su( &cur_su, true ) ) {
                    break;
                }
                depth = conv_vert_unit( &cur_su, g_text_spacing, g_curr_font );
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else {
                p = pa; // restore any spaces before non-attribute value
                break;
            }
        }
    }

    first_xline = true;

    init_nest_cb();
    nest_cb->p_stack = copy_to_nest_stack();
    nest_cb->left_indent = conv_hor_unit( &layout_work.xmp.left_indent, g_curr_font );
    nest_cb->right_indent = -1 * conv_hor_unit( &layout_work.xmp.right_indent, g_curr_font );
    nest_cb->font = g_curr_font;
    nest_cb->c_tag = t_XMP;

    t_page.cur_left += nest_cb->left_indent;
    t_page.max_width += nest_cb->right_indent;

    t_page.cur_width = t_page.cur_left;
    ProcFlags.keep_left_margin = true;  // keep special indent

    g_text_spacing = layout_work.xmp.spacing;

    set_skip_vars( NULL, &layout_work.xmp.pre_skip, &layout_work.xmp.post_skip,
                                                            g_text_spacing, g_curr_font );

    nest_cb->post_skip = g_post_skip;   // shift post_skip to follow eXMP
    g_post_skip = 0;

    sav_group_type = cur_group_type;
    cur_group_type = gt_xmp;
    cur_doc_el_group = alloc_doc_el_group( gt_xmp );
    cur_doc_el_group->next = t_doc_el_group;
    t_doc_el_group = cur_doc_el_group;
    cur_doc_el_group = NULL;

    concat_save = ProcFlags.concat;
    ProcFlags.concat = false;
    justify_save = ProcFlags.justify;
    ProcFlags.justify = ju_off;

    if( depth > 0 ) {
        g_blank_units_lines = depth;
        script_process_break();
    }

    if( !ProcFlags.reprocess_line
      && *p != '\0' ) { // text after tag
        SkipDot( p );                               // possible tag end
        if( *p != '\0' ) {
            if( IS_CONT_CHAR( p ) ) { // text is continuation character only
                /* placeholder */
            } else {
                process_text( p, g_curr_font);          // if text follows
            }
        }
    }

    /* Set up to skip first blank line if inside macro */

    if( input_cbs->fmflags & II_macro ) {
        ProcFlags.skip_blank_line = true;
    }

    scan_start = scan_stop + 1;
    return;
}


/***************************************************************************/
/* Format:  :eXMP.                                                         */
/*                                                                         */
/* This tag signals the end of an example. A corresponding :xmp tag must   */
/* be previously specified for each :exmp tag.                             */
/*                                                                         */
/***************************************************************************/

void gml_exmp( const gmltag * entry )
{
    bool            list_top;
    char        *   p;
    doc_element *   cur_el;
    tag_cb      *   wk;

    (void)entry;

    if( is_ip_tag( nest_cb->c_tag ) ) {                 // inline phrase not closed
        g_err_tag_nest( str_tags[nest_cb->c_tag + 1] ); // end tag expected
    }

    /* Ensure blank lines at end of XMP use correct font */

    if( (g_blank_units_lines == 0)
      && (g_blank_text_lines > 0) ) {
        g_blank_units_lines = g_blank_text_lines * wgml_fonts[layout_work.xmp.font].line_height;
    }
    g_blank_text_lines = 0;

    script_process_break();
    if( cur_group_type != gt_xmp ) {        // no preceding :XMP tag
        g_err_tag_prec( "XMP" );
    }
    g_curr_font = font_save;                // recover font in effect before XMP
    ProcFlags.concat = concat_save;
    ProcFlags.justify = justify_save;
    t_page.cur_left = nest_cb->lm;
    t_page.max_width = nest_cb->rm;
    g_post_skip = nest_cb->post_skip;       // shift post_skip to follow eXMP

    wk = nest_cb;
    nest_cb = nest_cb->prev;
    add_tag_cb_to_pool( wk );

    /* Place the accumulated xlines on the proper page */

    cur_group_type = sav_group_type;
    if( t_doc_el_group != NULL) {
        cur_doc_el_group = t_doc_el_group;      // detach current element group
        t_doc_el_group = t_doc_el_group->next;  // processed doc_elements go to next group, if any
        cur_doc_el_group->next = NULL;

        if( cur_doc_el_group->first != NULL ) {
            if( cur_doc_el_group->first->type == el_text ) {                    // only text has spacing
                cur_doc_el_group->first->element.text.first->units_spacing = 0; // no spacing on first line
            }

            if( (cur_doc_el_group->depth + t_page.cur_depth) > t_page.max_depth ) {
                next_column();  //  the block won't fit on this page (or in this column)
            }
        }

        list_top = true;
        while( cur_doc_el_group->first != NULL ) {
            cur_el = cur_doc_el_group->first;
            if( list_top
              && (input_cbs->fmflags & II_macro)
              && (cur_el->type == el_vspace) ) {     // first element is vspace
                if( cur_el->blank_lines > 0 ) {
                    cur_el->blank_lines -= wgml_fonts[cur_el->element.vspace.font].line_height;
                }
            }
            if( (cur_el->next != NULL)
              && (cur_el->type == el_text)
              && (cur_el->next->type == el_vspace) ) {
                cur_el->element.text.vspace_next = true;        // matches wgml 4.0
            }
            cur_doc_el_group->first = cur_doc_el_group->first->next;
            cur_el->next = NULL;
            insert_col_main( cur_el );
            list_top = false;                                   // first element done
        }

        add_doc_el_group_to_pool( cur_doc_el_group );
        cur_doc_el_group = NULL;
    }

    ProcFlags.skips_valid = false;      // activate post_skip for next element
    t_page.cur_width = t_page.cur_left;
    scan_err = false;
    p = scan_start;
    SkipDot( p );                       // over '.'
    if( *p != '\0' ) {
        if( (input_cbs->hidden_head != NULL)
          && !input_cbs->hidden_head->ip_start
          && IS_CONT_CHAR( p ) ) { // text is continuation character only
            if( input_cbs->fmflags & II_macro ) {
                /* placeholder */
            } else {
                if( input_cbs->hidden_head->hh_tag ) {
                    g_post_skip = 0;
                }
            }
        } else {
            do_force_pc( p );
        }
    } else {
        ProcFlags.force_pc = true;
    }

    scan_start = scan_stop + 1;
    return;
}

