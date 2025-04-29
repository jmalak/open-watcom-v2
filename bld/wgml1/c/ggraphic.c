/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML tag :GRAPHIC processing
*
****************************************************************************/


#include    "wgml.h"


/***************************************************************************/
/*  :GRAPHIC tag                                                           */
/***************************************************************************/

void    gml_graphic( const gmltag * entry )
{
    bool            depth_found             = false;
    bool            file_found              = false;
    char            file[_MAX_PATH];
    char            rt_buff[MAX_FILE_ATTR];
    char        *   p;
    char        *   pa;
    char        *   pb;
    doc_element *   cur_el;
    inputcb     *   cb                      = input_cbs;
    su              cur_su;
    uint32_t        depth;
    uint32_t        scale                   = 100;
    uint32_t        width;
    int32_t         xoff                    = 0;
    int32_t         yoff                    = 0;
    FILE            *fp;
    att_val_type    attr_val;
    char            attname[TAG_ATT_NAME_LENGTH + 1];

    if( (ProcFlags.doc_sect < doc_sect_gdoc) ) {
        if( (ProcFlags.doc_sect_nxt < doc_sect_gdoc) ) {
            xx_err_c( ERR_TAG_BEFORE_GDOC, entry->tagname );
        }
    }

    script_process_break();                // flush existing text
    start_doc_sect();                   // if not already done

    file[0] = '\0';
    rt_buff[0] = '\0';
    width = t_page.max_width - t_page.cur_left;

    p = scan_start;
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            p = get_att_name( p, &pa, attname );
            if( ProcFlags.reprocess_line ) {
                break;
            }
            if( strcmp( "file", attname ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.name == NULL ) {
                    break;
                }
                file_found = true;
                if( attr_val.len > _MAX_PATH - 1 )
                    attr_val.len = _MAX_PATH - 1;
                strncpy( file, attr_val.name, attr_val.len );
                file[attr_val.len] = '\0';
                split_attr_file( file, rt_buff, sizeof( rt_buff ) );
                if( (rt_buff[0] != '\0') ) {
                    xx_warn( WNG_REC_TYPE_GRAPHIC );
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "depth", attname ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.name == NULL ) {
                    break;
                }
                depth_found = true;
                if( att_val_to_su( &cur_su, true, &attr_val, false ) ) {
                    break;
                }
                depth = conv_vert_unit( &cur_su, g_text_spacing, g_curr_font );
                if( depth == 0 ) {
                    xx_line_err_c( ERR_INV_DEPTH_GRAPHIC_1, attr_val.name );
                }
                if( depth > t_page.max_depth ) {
                    xx_line_err_c( ERR_INV_DEPTH_GRAPHIC_2, attr_val.name );
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "width", attname ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.name == NULL ) {
                    break;
                }

                /* GRAPHIC uses the current column width even if "page" is specified */

                if( strcmp( "page", attr_val.specval ) == 0 ) {
                    // default value is the correct value to use
                } else if( strcmp( "column", attr_val.specval ) == 0 ) {
                    // default value is the correct value to use
                } else {    // value actually specifies the width
                    if( att_val_to_su( &cur_su, true, &attr_val, false ) ) {
                        break;
                    }
                    width = conv_hor_unit( &cur_su, g_curr_font );
                    if( width == 0 ) {
                        xx_line_err_c( ERR_INV_WIDTH_GRAPHIC_1, attr_val.name );
                    }
                    if( width > t_page.last_pane->col_width ) {
                        xx_line_err_c( ERR_INV_WIDTH_GRAPHIC_2, attr_val.name );
                    }
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "scale", attname ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.name == NULL ) {
                    break;
                }
                pb = attr_val.name;
                if( (*pb == '+') || (*pb == '-') ) {    // signs not allowed
                    xx_line_err_c( ERR_NUM_TOO_LARGE, attr_val.name );
                }
                scale = 0;
                while( my_isdigit( *pb ) ) {            // convert to number
                    scale = (10 * scale) + (*pb - '0');
                    pb++;
                    if( (pb - attr_val.name) > attr_val.len ) {  // value end reached
                        break;
                    }
                }
                if( scale > 0x7fffffff ) {              // wgml 4.0 limit
                    xx_line_err_c( ERR_NUM_TOO_LARGE, attr_val.name );
                }
                if( (pb - attr_val.name) < attr_val.len ) {      // value continues on
                    xx_line_err_c( ERR_NUM_TOO_LARGE, attr_val.name );
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "xoff", attname ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.name == NULL ) {
                    break;
                }
                if( att_val_to_su( &cur_su, false, &attr_val, false ) ) {
                    break;
                }
                xoff = conv_hor_unit( &cur_su, g_curr_font );
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "yoff", attname ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.name == NULL ) {
                    break;
                }
                if( att_val_to_su( &cur_su, false, &attr_val, false ) ) {
                    break;
                }
                yoff = conv_vert_unit( &cur_su, g_text_spacing, g_curr_font );
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else {    // no match = end-of-tag in wgml 4.0
                ProcFlags.tag_end_found = true;
                p = pa; // restore spaces before text
                break;
            }
        }
    }

    if( !depth_found || !file_found ) { // detect missing required attributes
        xx_err( ERR_ATT_MISSING );
    }

    if( !ProcFlags.ps_device ) {        // character devices ignore SK & post_skip
        g_skip = 0;
        g_post_skip = 0;
    }
    set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );

    // only set up the doc_element if the file exists
    fp = search_file_in_dirs( file, "", "", ds_doc_spec );
    if( fp != NULL ) {
        cur_el = init_doc_el( el_graph, depth );
        cur_el->element.graph.cur_left = t_page.cur_left;
        cur_el->element.graph.depth = depth;
        cur_el->element.graph.scale = scale;
        cur_el->element.graph.width = width;
        cur_el->element.graph.xoff = xoff;
        cur_el->element.graph.yoff = yoff;
        cur_el->element.graph.fp = fp;
        if( nest_cb->c_tag == t_NONE ){
            cur_el->element.graph.next_font = FONT0;
        } else {
            cur_el->element.graph.next_font = g_prev_font;
        }
        cur_el->element.graph.short_name = mem_strdup( file );
        cur_el->element.graph.filename = mem_strdup( try_file_name );

        if( WgmlFlags.inclist ) {
            g_info_lm( INF_CURR_FILE, cur_el->element.graph.filename );
            while( cb->fmflags & II_macro ) {                 // find prior file
                 cb = cb->prev;
            }
            g_info_lm( INF_CURR_FILE, cb->s.f->filename );
        }

        insert_col_main( cur_el );

    } else {
        xx_err_c( ERR_FILE_NOT_FOUND, file );
    }

    if( !ProcFlags.reprocess_line && *p != '\0' ) {
        SkipDot( p );                       // possible tag end
        if( *p != '\0' ) {                  // only if text follows
            process_text( p, g_curr_font );
        }
    }

    scan_start = scan_stop;
    return;
}


