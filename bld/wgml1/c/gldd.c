/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :DD   tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :DD     attributes                                                    */
/***************************************************************************/
const   lay_att     dd_att[3] =
    { e_line_left, e_font, e_dummy_zero };

/*****************************************************************************/
/*Define the characteristics of the data description entity.                 */
/*                                                                           */
/*:DD                                                                        */
/*        line_left = '0.5i'                                                 */
/*        font = 0                                                           */
/*                                                                           */
/*line_left This attribute accepts any valid horizontal space unit. The      */
/*specified amount of space must be available on the output line after the   */
/*definition term which precedes the data description. If there is not       */
/*enough space, the data description will be started on the next output line.*/
/*                                                                           */
/*font This attribute accepts a non-negative integer number. If a font       */
/*number is used for which no font has been defined, WATCOM                  */
/*Script/GML will use font zero. The font numbers from zero to three         */
/*correspond directly to the highlighting levels specified by the            */
/*highlighting phrase GML tags. The font attribute defines the font of       */
/*the data description.                                                      */
/*                                                                           */
/*****************************************************************************/


/***************************************************************************/
/*  lay_dd                                                                 */
/***************************************************************************/

void    lay_dd( const gmltag * entry )
{
    char                *p;
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
    if( ProcFlags.lay_xxx != el_dd ) {
        ProcFlags.lay_xxx = el_dd;
    }
    cc = lay_attr_and_value( &lay_attr );            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = dd_att[k]; curr > 0; k++, curr = dd_att[k] ) {
            if( strcmp( lay_att_names[curr], lay_attr.attname ) == 0 ) {
                p = lay_attr.val_name;
                switch( curr ) {
                case e_line_left:
                    if( AttrFlags.line_left ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val_name - lay_attr.att_name + lay_attr.val_len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.dd.line_left );
                    AttrFlags.line_left = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val_name - lay_attr.att_name + lay_attr.val_len);
                    }
                    cvterr = i_font_number( p, &lay_attr, &layout_work.dd.font );
                    if( layout_work.dd.font >= wgml_font_cnt ) {
                        layout_work.dd.font = 0;
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

