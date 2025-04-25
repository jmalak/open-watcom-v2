/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :DOCNUM  tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :DOCNUM   attributes                                                  */
/***************************************************************************/
const   lay_att     docnum_att[7] =
    { e_left_adjust, e_right_adjust, e_page_position, e_font, e_pre_skip,
      e_docnum_string, e_dummy_zero };


/***************************************************************************/
/*  lay_docnum                                                             */
/***************************************************************************/

void    lay_docnum( const gmltag * entry )
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
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_docnum ) {
        ProcFlags.lay_xxx = el_docnum;
    }
    cc = lay_attr_and_value( &lay_attr );            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = docnum_att[k]; curr > 0; k++, curr = docnum_att[k] ) {
            if( strcmp( lay_att_names[curr], lay_attr.attname ) == 0 ) {
                p = lay_attr.val.name;
                switch( curr ) {
                case e_left_adjust:
                    if( AttrFlags.left_adjust ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.docnum.left_adjust );
                    AttrFlags.left_adjust = true;
                    break;
                case e_right_adjust:
                    if( AttrFlags.right_adjust ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.docnum.right_adjust );
                    AttrFlags.right_adjust = true;
                    break;
                case e_page_position:
                    if( AttrFlags.page_position ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_page_position( p, &lay_attr,
                                          &layout_work.docnum.page_position );
                    AttrFlags.page_position = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_font_number( p, &lay_attr, &layout_work.docnum.font );
                    if( layout_work.docnum.font >= wgml_font_cnt ) {
                        layout_work.docnum.font = 0;
                    }
                    AttrFlags.font = true;
                    break;
                case e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.docnum.pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case e_docnum_string:
                    if( AttrFlags.docnum_string ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_xx_string( p, &lay_attr, layout_work.docnum.string );
                    AttrFlags.docnum_string = true;
                    break;
                default:
                    internal_err( __FILE__, __LINE__ );
                }
                if( cvterr ) {          // there was an error
                    xx_err( ERR_ATT_VAL_INV );
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            xx_err( ERR_ATT_NAME_INV );
        }
        cc = lay_attr_and_value( &lay_attr );            // get att with value
    }
    scan_start = scan_stop;
    return;
}

