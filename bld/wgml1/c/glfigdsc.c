/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :FIGDESC tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :FIGDESC attributes                                                   */
/***************************************************************************/
const   lay_att     figdesc_att[3] =
    { e_pre_lines, e_font, e_dummy_zero };

/*********************************************************************************/
/*Define the characteristics of the figure description entity.                   */
/*:FIGDESC                                                                       */
/*        pre_lines = 1                                                          */
/*        font = 0                                                               */
/*                                                                               */
/*pre_lines This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting number of lines are skipped                   */
/*before the figure description. If the document entity starts a new             */
/*page, the specified number of lines are still skipped. The pre-lines           */
/*value is not merged with the previous document entity's post-skip              */
/*value. If the previous tag was :figcap, this value is ignored.                 */
/*                                                                               */
/*font This attribute accepts a non-negative integer number. If a font           */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The font attribute defines the font of           */
/*the figure description. The font value is linked to the pre_lines              */
/*attribute (see "Font Linkage" on page 77).                                     */
/*********************************************************************************/


/***************************************************************************/
/*  lay_figdesc                                                            */
/***************************************************************************/

void    lay_figdesc( const gmltag * entry )
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
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_figdesc ) {
        ProcFlags.lay_xxx = el_figdesc;
    }
    cc = lay_attr_and_value( &lay_attr );            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = figdesc_att[k]; curr > 0; k++, curr = figdesc_att[k] ) {
            if( strcmp( lay_att_names[curr], lay_attr.attname ) == 0 ) {
                p = lay_attr.val.name;
                switch( curr ) {
                case e_pre_lines:
                    if( AttrFlags.pre_lines ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.figdesc.pre_lines );
                    AttrFlags.pre_lines = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_font_number( p, &lay_attr, &layout_work.figdesc.font );
                    if( layout_work.figdesc.font >= wgml_font_cnt ) {
                        layout_work.figdesc.font = 0;
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
        cc = lay_attr_and_value( &lay_attr );            // get att with value
    }
    scan_start = scan_stop;
    return;
}

