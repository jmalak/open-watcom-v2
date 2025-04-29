/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement several sub tags for :LAYOUT processing
*                   all those with only font attribute
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*                     several  attributes  with only font as value        */
/*                                                                         */
/* :DT :GT :DTHD :CIT :GD :DDHD :IXPGNUM :IXMAJOR                          */
/*                                                                         */
/***************************************************************************/

const   lay_att     xx_att[2] =
    { e_font, e_dummy_zero };


void    lay_xx( const gmltag * entry )
{
    char                *p;
    condcode            cc;
    font_number         *fontptr;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    lay_sub             x_tag;
    lay_att_val         lay_attr;

    p = scan_start;
    cvterr = false;

    if( !WgmlFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( strcmp( "CIT", entry->tagname ) == 0 ) {
        x_tag = el_cit;
        fontptr = &layout_work.cit.font;
    } else if( strcmp( "DTHD", entry->tagname ) == 0 ) {
        x_tag = el_dthd;
        fontptr = &layout_work.dthd.font;
    } else if( strcmp( "DT", entry->tagname ) == 0 ) {
        x_tag = el_dt;
        fontptr = &layout_work.dt.font;
    } else if( strcmp( "GT", entry->tagname ) == 0 ) {
        x_tag = el_gt;
        fontptr = &layout_work.gt.font;
    } else if( strcmp( "GD", entry->tagname ) == 0 ) {
        x_tag = el_gd;
        fontptr = &layout_work.gd.font;
    } else if( strcmp( "DDHD", entry->tagname ) == 0 ) {
        x_tag = el_ddhd;
        fontptr = &layout_work.ddhd.font;
    } else if( strcmp( "IXPGNUM", entry->tagname ) == 0 ) {
        x_tag = el_ixpgnum;
        fontptr = &layout_work.ixpgnum.font;
    } else if( strcmp( "IXMAJOR", entry->tagname ) == 0 ) {
        x_tag = el_ixmajor;
        fontptr = &layout_work.ixmajor.font;
    } else {
        internal_err( __FILE__, __LINE__ );
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != x_tag ) {
        ProcFlags.lay_xxx = x_tag;
    }
    cc = lay_attr_and_value( &lay_attr );            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = xx_att[k]; curr > 0; k++, curr = xx_att[k] ) {

            if( strnicmp( att_names[curr], lay_attr.att_name, lay_attr.att_len ) == 0 ) {
                p = lay_attr.val_name;

                switch( curr ) {
                case   e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val_name - lay_attr.att_name + lay_attr.val_len);
                    }
                    cvterr = i_font_number( p, &lay_attr, fontptr );
                    if( *fontptr >= wgml_font_cnt ) {
                        *fontptr = 0;
                    }
                    AttrFlags.font = true;
                    break;
                default:
                    internal_err( __FILE__, __LINE__ );
                }
                if( cvterr ) {          // there was an error
                    xx_err( err_att_val_inv );
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            xx_err( err_att_name_inv );
        }
        cc = lay_attr_and_value( &lay_attr );            // get att with value
    }
    scan_start = scan_stop + 1;
    return;
}

