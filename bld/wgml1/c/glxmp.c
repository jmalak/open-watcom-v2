/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :XMP tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :XMP   attributes                                                     */
/***************************************************************************/

const   lay_att     xmp_att[7] =
    { e_left_indent, e_right_indent, e_pre_skip, e_post_skip, e_spacing,
      e_font, e_dummy_zero };


/*********************************************************************************/
/*Define the characteristics of the example entity.                              */
/*:XMP                                                                           */
/*       left_indent = '0.25i'                                                   */
/*       right_indent = 0                                                        */
/*       pre_skip = 2                                                            */
/*       post_skip = 0                                                           */
/*       spacing = 1                                                             */
/*       font = 0                                                                */
/*                                                                               */
/*left_indent This attribute accepts any valid horizontal space unit. The left   */
/*indent value is added to the current left margin. The left margin will         */
/*be reset to its previous value at the end of the example.                      */
/*                                                                               */
/*right_indent This attribute accepts any valid horizontal space unit. The right */
/*indent value is subtracted from the current right margin. The right            */
/*margin will be reset to its previous value at the end of the example.          */
/*                                                                               */
/*pre_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped before             */
/*the example. The pre-skip will be merged with the previous                     */
/*document entity's post-skip value. If a pre-skip occurs at the                 */
/*beginning of an output page, the pre-skip value has no effect.                 */
/*                                                                               */
/*post_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped after              */
/*the example. The post-skip will be merged with the next document               */
/*entity's pre-skip value. If a post-skip occurs at the end of an output         */
/*page, any remaining part of the skip is not carried over to the next           */
/*output page.                                                                   */
/*                                                                               */
/*spacing This attribute accepts a positive integer number. The spacing          */
/*determines the number of blank lines that are output between text              */
/*lines. If the line spacing is two, each text line will take two lines in       */
/*the output. The number of blank lines between text lines will                  */
/*therefore be the spacing value minus one. The spacing attribute                */
/*defines the line spacing within the example.                                   */
/*                                                                               */
/*font This attribute accepts a non-negative integer number. If a font           */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The font attribute defines the font of           */
/*the example. The font value is linked to the left_indent,                      */
/*right_indent, pre_skip and post_skip attributes (see "Font Linkage"            */
/*on page 77).                                                                   */
/*********************************************************************************/


/***************************************************************************/
/*  lay_xmp                                                                */
/***************************************************************************/

void    lay_xmp( const gmltag * entry )
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
    if( ProcFlags.lay_xxx != el_xmp ) {
        ProcFlags.lay_xxx = el_xmp;
    }
    // get one att with value
    while( (cc = lay_attr_and_value( &lay_attr )) == pos ) {
        cvterr = -1;
        for( k = 0, curr = xmp_att[k]; curr > 0; k++, curr = xmp_att[k] ) {
            if( strcmp( lay_att_names[curr], lay_attr.attname ) == 0 ) {
                p = lay_attr.val.name;
                switch( curr ) {
                case e_left_indent:
                    if( AttrFlags.left_indent ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.xmp.left_indent );
                    AttrFlags.left_indent = true;
                    break;
                case e_right_indent:
                    if( AttrFlags.right_indent ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.xmp.right_indent );
                    AttrFlags.right_indent = true;
                    break;
                case e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr, &layout_work.xmp.pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr, &layout_work.xmp.post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_spacing( p, &lay_attr, &layout_work.xmp.spacing );
                    AttrFlags.spacing = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_font_number( p, &lay_attr, &layout_work.xmp.font );
                    if( layout_work.xmp.font >= wgml_font_cnt ) {
                        layout_work.xmp.font = 0;
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
    if( layout_work.xmp.spacing < 1 ) { // enforce minimum value
        xx_err( ERR_NUM_ZERO );
    }

    scan_start = scan_stop;
    return;
}

