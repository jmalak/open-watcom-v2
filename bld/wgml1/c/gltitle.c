/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :TITLE  tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :TITLE    attributes                                                  */
/***************************************************************************/
const   lay_att     title_att[7] =
    { e_left_adjust, e_right_adjust, e_page_position, e_font, e_pre_top_skip,
      e_skip, e_dummy_zero };


/***************************************************************************/
/*  lay_title                                                              */
/***************************************************************************/

void    lay_title( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    lay_att_val         lay_attr;

    (void)entry;

    p = scan_start;
    cvterr = false;

    if( !WgmlFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_title ) {
        ProcFlags.lay_xxx = el_title;
    }
    cc = lay_attr_and_value( &lay_attr );            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = title_att[k]; curr > 0; k++, curr = title_att[k] ) {

            if( strnicmp( att_names[curr], lay_attr.att_name, lay_attr.att_len ) == 0 ) {
                p = lay_attr.val_name;

                switch( curr ) {
                case   e_left_adjust:
                    if( AttrFlags.left_adjust ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val_name - lay_attr.att_name + lay_attr.val_len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.title.left_adjust );
                    AttrFlags.left_adjust = true;
                    break;
                case   e_right_adjust:
                    if( AttrFlags.right_adjust ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val_name - lay_attr.att_name + lay_attr.val_len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.title.right_adjust );
                    AttrFlags.right_adjust = true;
                    break;
                case   e_page_position:
                    if( AttrFlags.page_position ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val_name - lay_attr.att_name + lay_attr.val_len);
                    }
                    cvterr = i_page_position( p, &lay_attr,
                                         &layout_work.title.page_position );
                    AttrFlags.page_position = true;
                    break;
                case   e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val_name - lay_attr.att_name + lay_attr.val_len);
                    }
                    cvterr = i_font_number( p, &lay_attr, &layout_work.title.font );
                    if( layout_work.title.font >= wgml_font_cnt ) {
                        layout_work.title.font = 0;
                    }
                    AttrFlags.font = true;
                    break;
                case   e_pre_top_skip:
                    if( AttrFlags.pre_top_skip ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val_name - lay_attr.att_name + lay_attr.val_len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.title.pre_top_skip );
                    AttrFlags.pre_top_skip = true;
                    break;
                case   e_skip:
                    if( AttrFlags.skip ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val_name - lay_attr.att_name + lay_attr.val_len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.title.skip );
                    AttrFlags.skip = true;
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

