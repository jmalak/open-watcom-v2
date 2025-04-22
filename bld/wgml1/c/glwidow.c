/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :WIDOW tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :WIDOW attributes                                                     */
/***************************************************************************/
const   lay_att     widow_att[2] = { e_threshold, e_dummy_zero };

/*****************************************************************************/
/*Define the widowing control of document elements.                          */
/*:WIDOW                                                                     */
/*        threshold = 2                                                      */
/*threshold This attribute accepts as a value a non-negative integer number. */
/*The specified value indicates the minimum number of text lines             */
/*which must fit on the page. A document element will be forced to           */
/*the next page or column if the threshold requirement is not met.           */
/*****************************************************************************/

/***************************************************************************/
/*  lay_widow                                                              */
/***************************************************************************/

void    lay_widow( const gmltag * entry )
{
    char                *p;
    condcode            cc;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    lay_att_val         lay_attr;

    (void)entry;

    p = scan_start;

    if( !WgmlFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_widow ) {
        ProcFlags.lay_xxx = el_widow;
    }
    cc = lay_attr_and_value( &lay_attr );            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = widow_att[k]; curr > 0; k++, curr = widow_att[k] ) {
            if( strcmp( lay_att_names[curr], lay_attr.attname ) == 0 ) {
                p = lay_attr.val_name;
                switch( curr ) {
                case e_threshold:
                    if( AttrFlags.threshold ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val_name - lay_attr.att_name + lay_attr.val_len);
                    }
                    cvterr = i_threshold( p, &lay_attr, &layout_work.widow.threshold );
                    AttrFlags.threshold = true;
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
