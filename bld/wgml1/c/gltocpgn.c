/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :TOCPGNUM tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :TOCPGNUM attributes                                                  */
/***************************************************************************/
const   lay_att     tocpgnum_att[3] =
    { e_size, e_font, e_dummy_zero };


/***************************************************************************/
/*  lay_tocpgnum                                                           */
/***************************************************************************/

void    lay_tocpgnum( const gmltag * entry )
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
    if( ProcFlags.lay_xxx != el_tocpgnum ) {
        ProcFlags.lay_xxx = el_tocpgnum;
    }
    // get one att with value
    while( (cc = lay_attr_and_value( &lay_attr )) == pos ) {
        cvterr = -1;
        for( k = 0, curr = tocpgnum_att[k]; curr > 0; k++, curr = tocpgnum_att[k] ) {
            if( strcmp( lay_att_names[curr], lay_attr.attname ) == 0 ) {
                p = lay_attr.val.name;
                switch( curr ) {
                case e_size:
                    if( AttrFlags.size ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.tocpgnum.size );
                    AttrFlags.size = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_font_number( p, &lay_attr, &layout_work.tocpgnum.font );
                    if( layout_work.tocpgnum.font >= wgml_font_cnt ) {
                        layout_work.tocpgnum.font = 0;
                    }
                    AttrFlags.font = true;
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
    }
    scan_start = scan_stop;
    return;
}

