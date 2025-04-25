/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :TITLEP tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :TITLEP   attributes                                                  */
/***************************************************************************/
const   lay_att     titlep_att[3] =
    { e_spacing, e_columns, e_dummy_zero };


/***************************************************************************/
/*  lay_titlep                                                             */
/***************************************************************************/

void    lay_titlep( const gmltag * entry )
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
    if( ProcFlags.lay_xxx != el_titlep ) {
        ProcFlags.lay_xxx = el_titlep;
    }
    cc = lay_attr_and_value( &lay_attr );            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = titlep_att[k]; curr > 0; k++, curr = titlep_att[k] ) {
            if( strcmp( lay_att_names[curr], lay_attr.attname ) == 0 ) {
                p = lay_attr.val.name;
                switch( curr ) {
                case e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_spacing( p, &lay_attr, &layout_work.titlep.spacing );
                    AttrFlags.spacing = true;
                    break;
                case e_columns:
                    if( AttrFlags.columns ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_int8( p, &lay_attr, &layout_work.titlep.columns );
                    AttrFlags.columns = true;
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

