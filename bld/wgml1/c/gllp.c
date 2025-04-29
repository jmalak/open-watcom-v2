/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :LP tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :LP    attributes                                                     */
/***************************************************************************/
const   lay_att     lp_att[7] =
    { e_left_indent, e_right_indent, e_line_indent, e_pre_skip, e_post_skip,
      e_spacing, e_dummy_zero };

/*********************************************************************************/
/*Define the characteristics of the list part entity.                            */
/*:LP                                                                            */
/*        left_indent = 0                                                        */
/*        right_indent = 0                                                       */
/*        line_indent = 0                                                        */
/*        pre_skip = 1                                                           */
/*        post_skip = 1                                                          */
/*        spacing = 1                                                            */
/*                                                                               */
/*left_indent This attribute accepts any valid horizontal space unit. The left   */
/*indent value is added to the current left margin. The left margin will         */
/*be reset to its previous value at the end of the list part.                    */
/*right_indent This attribute accepts any valid horizontal space unit. The right */
/*indent value is subtracted from the current right margin. The right            */
/*margin will be reset to its previous value at the end of the list part.        */
/*                                                                               */
/*line_indent The line_indent attribute accepts any valid horizontal space unit. */
/*This attribute specifies the amount of indentation for the first output        */
/*line of the list part.                                                         */
/*                                                                               */
/*pre_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped before             */
/*the list part. The pre-skip will be merged with the previous                   */
/*document entityٿs post-skip value. If a pre-skip occurs at the                */
/*beginning of an output page, the pre-skip value has no effect.                 */
/*                                                                               */
/*post_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped after              */
/*the list part. The post-skip will be merged with the next document             */
/*entityٿs pre-skip value. If a post-skip occurs at the end of an output        */
/*page, any remaining part of the skip is not carried over to the next           */
/*output page.                                                                   */
/*                                                                               */
/*spacing This attribute accepts a positive integer number. The spacing          */
/*determines the number of blank lines that are output between text              */
/*lines. If the line spacing is two, each text line will take two lines in       */
/*the output. The number of blank lines between text lines will                  */
/*therefore be the spacing value minus one. The spacing attribute                */
/*defines the line spacing within the list part.                                 */
/*********************************************************************************/

/***************************************************************************/
/*  lay_lp                                                                 */
/***************************************************************************/

void    lay_lp( const gmltag * entry )
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
    if( ProcFlags.lay_xxx != el_lp ) {
        ProcFlags.lay_xxx = el_lp;
    }
    // get one att with value
    while( (cc = lay_attr_and_value( &lay_attr )) == pos ) {
        cvterr = -1;
        for( k = 0, curr = lp_att[k]; curr > 0; k++, curr = lp_att[k] ) {
            if( strcmp( lay_att_names[curr], lay_attr.attname ) == 0 ) {
                p = lay_attr.val.name;
                switch( curr ) {
                case e_left_indent:
                    if( AttrFlags.left_indent ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.lp.left_indent );
                    AttrFlags.left_indent = true;
                    break;
                case e_right_indent:
                    if( AttrFlags.right_indent ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.lp.right_indent );
                    AttrFlags.right_indent = true;
                    break;
                case e_line_indent:
                    if( AttrFlags.line_indent ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.lp.line_indent );
                    AttrFlags.line_indent = true;
                    break;
                case e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr, &layout_work.lp.pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr, &layout_work.lp.post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_spacing( p, &lay_attr, &layout_work.lp.spacing );
                    AttrFlags.spacing = true;
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

