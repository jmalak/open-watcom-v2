/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML tag :BINCLUDE processing
*
****************************************************************************/


#include    "wgml.h"


/***************************************************************************/
/*  :BINCLUDE tag                                                          */
/***************************************************************************/

void    gml_binclude( const gmltag * entry )
{
    bool            depth_found             = false;
    bool            file_found              = false;
    bool            has_rec_type            = false;
    bool            reposition;
    bool            reposition_found        = false;
    char            file[_MAX_PATH];
    char            rt_buff[MAX_FILE_ATTR];
    char        *   p;
    char        *   pa;
    doc_element *   cur_el;
    inputcb     *   cb                      = input_cbs;
    su              depth_su;
    uint32_t        depth;

    if( (ProcFlags.doc_sect < doc_sect_gdoc) ) {
        if( (ProcFlags.doc_sect_nxt < doc_sect_gdoc) ) {
            xx_err_c( err_tag_before_gdoc, entry->tagname );
        }
    }

    script_process_break();                // flush existing text
    start_doc_sect();                   // if not already done

    file[0] = '\0';
    rt_buff[0] = '\0';
    p = scan_start;
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            pa = get_att_start( p );
            p = att_start;
            if( ProcFlags.reprocess_line ) {
                break;
            }
            if( strnicmp( "file", p, 4 ) == 0 ) {
                p += 4;
                p = get_att_value( p );
                if( val_start == NULL ) {
                    break;
                }
                file_found = true;
                memcpy_s( file, _MAX_PATH, val_start, val_len );
                if( val_len < _MAX_PATH ) {
                    file[val_len] = '\0';
                } else {
                    file[_MAX_PATH - 1] = '\0';
                }
                split_attr_file( file, rt_buff, MAX_FILE_ATTR );
                if( (rt_buff[0] != '\0') ) {
                    has_rec_type = true;
                    if( rt_buff[0] != 't' ) {
                        xx_warn( wng_rec_type_binclude );
                    }
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strnicmp( "depth", p, 5 ) == 0 ) {
                p += 5;
                p = get_att_value( p );
                if( val_start == NULL ) {
                    break;
                }
                depth_found = true;
                if( att_val_to_su( &depth_su, true ) ) {
                    break;
                }
                depth = conv_vert_unit( &depth_su, g_text_spacing, g_curr_font );
                if( depth > t_page.max_depth ) {
                    xx_line_err_c( err_inv_depth_binclude, val_start );
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strnicmp( "reposition", p, 10 ) == 0 ) {
                p += 10;
                p = get_att_value( p );
                if( val_start == NULL ) {
                    break;
                }
                reposition_found = true;
                if( strnicmp( "start", val_start, 5 ) == 0 ) {
                    reposition = true;  // moving following text down by depth
                } else if( strnicmp( "end", val_start, 3 ) == 0 ) {
                    reposition = false; // device at proper position after insertion
                } else {
                    xx_line_err_c( err_inv_att_val, val_start );
                }
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

    // detect missing required attributes
    if( !depth_found || !file_found || !reposition_found ) {
        xx_err( err_att_missing );
    }

    // only set up the doc_element if the file exists
    if( search_file_in_dirs( file, "", "", ds_doc_spec ) ) {
        if( depth == 0 ) {
            cur_el = alloc_doc_el(  el_binc );
        } else {
            set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );
            if( reposition && depth ) {                 // otherwise, element depth will be "0"
                cur_el = init_doc_el( el_binc, depth );
            } else {
                cur_el = init_doc_el( el_binc, 0 );
            }
        }
        cur_el->element.binc.depth = depth;
        cur_el->element.binc.cur_left = t_page.cur_width;
        cur_el->element.binc.force_FONT0 = false;
        cur_el->element.binc.has_rec_type = has_rec_type;
        cur_el->element.binc.fp = try_fp;
        try_fp = NULL;
        strncpy_s( cur_el->element.binc.file, _MAX_PATH, try_file_name, _MAX_PATH );

        if( WgmlFlags.inclist ) {
            g_info_lm( inf_curr_file, cur_el->element.binc.file );
            while( cb->fmflags & II_macro ) {                 // find prior file
                 cb = cb->prev;
            }
            g_info_lm( inf_curr_file, cb->s.f->filename );
        }

        insert_col_main( cur_el );

    } else {
        xx_err_c( err_file_not_found, file );
    }

    scan_start = scan_stop + 1;         // skip following text
    return;
}

