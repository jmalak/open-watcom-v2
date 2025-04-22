/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :FLPGNUM tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :FLPGNUM  attributes                                                  */
/***************************************************************************/
const   lay_att     flpgnum_att[3] =
    { e_size, e_font, e_dummy_zero };


/***************************************************************************/
/*  lay_flpgnum                                                            */
/***************************************************************************/

void    lay_flpgnum( const gmltag * entry )
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
    if( ProcFlags.lay_xxx != el_flpgnum ) {
        ProcFlags.lay_xxx = el_flpgnum;
    }
    cc = lay_attr_and_value( &lay_attr );            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = flpgnum_att[k]; curr > 0; k++, curr = flpgnum_att[k] ) {
            if( strcmp( lay_att_names[curr], lay_attr.attname ) == 0 ) {
                p = lay_attr.val_name;
                switch( curr ) {
                case e_size:
                    if( AttrFlags.size ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val_name - lay_attr.att_name + lay_attr.val_len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.flpgnum.size );
                    AttrFlags.size = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val_name - lay_attr.att_name + lay_attr.val_len);
                    }
                    cvterr = i_font_number( p, &lay_attr, &layout_work.flpgnum.font );
                    if( layout_work.flpgnum.font >= wgml_font_cnt ) {
                        layout_work.flpgnum.font = 0;
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
    scan_start = scan_stop;
    return;
}

