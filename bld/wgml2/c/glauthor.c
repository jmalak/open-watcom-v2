/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description: WGML implement :AUTHOR tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :AUTHOR   attributes                                                  */
/***************************************************************************/
const   lay_att     author_att[7] =
    { e_left_adjust, e_right_adjust, e_page_position, e_font, e_pre_skip,
      e_skip, e_dummy_zero };

/**********************************************************************************/
/*Define the characteristics of the author entity.                                */
/*:AUTHOR                                                                         */
/*        left_adjust = 0                                                         */
/*        right_adjust = ٿ1iٿ                                                   */
/*        page_position = right                                                   */
/*        font = 0                                                                */
/*        pre_skip = 25                                                           */
/*        skip = 1                                                                */
/*                                                                                */
/*left_adjust The left_adjust attribute accepts any valid horizontal space unit.  */
/*The left margin is set to the page left margin plus the specified left          */
/*adjustment.                                                                     */
/*                                                                                */
/*right_adjust The right_adjust attribute accepts any valid horizontal space unit.*/
/*The right margin is set to the page right margin minus the specified            */
/*right adjustment.                                                               */
/*                                                                                */
/*page_position This attribute accepts the values left, right, center, and        */
/*centre. Theposition of the author line between the left and right margins is    */
/*determined by the value selected. If left is the attribute value, the           */
/*text is output at the left margin. If right is the attribute value, the         */
/*text is output next to the right margin. When center or centre is               */
/*specified, the text is centered between the left and right margins.             */
/*                                                                                */
/*font This attribute accepts a non-negative integer number. If a font            */
/*number is used for which no font has been defined, WATCOM                       */
/*Script/GML will use font zero. The font numbers from zero to three              */
/*correspond directly to the highlighting levels specified by the                 */
/*highlighting phrase GML tags. The font attribute defines the font of            */
/*the author lines. The font value is linked to the left_adjust,                  */
/*right_adjust, pre_skip and skip attributes (see "Font Linkage" on               */
/*page 77).                                                                       */
/*                                                                                */
/*pre_skip This attribute accepts vertical space units. A zero value means that   */
/*no lines are skipped. If the skip value is a line unit, it is multiplied        */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped before              */
/*the author lines. The pre-skip will be merged with the previous                 */
/*document entityٿs post-skip value. If a pre-skip occurs at the                 */
/*beginning of an output page, the pre-skip value has no effect.                  */
/*                                                                                */
/*skip This attribute accepts vertical space units. A zero value means that       */
/*no lines are skipped. If the skip value is a line unit, it is multiplied        */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped                     */
/*between author lines.                                                           */
/**********************************************************************************/

/***************************************************************************/
/*  lay_author                                                             */
/***************************************************************************/

void    lay_author( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 cvterr;
    int                 k;
    lay_att             curr;

    (void)entry;

    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_author ) {
        ProcFlags.lay_xxx = el_author;
    }
    cc = get_attr_and_value();            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = author_att[k]; curr > 0; k++, curr = author_att[k] ) {

            if( strnicmp( att_names[curr], g_att_val.att_name, g_att_val.att_len ) == 0 ) {
                p = g_att_val.val_name;

                switch( curr ) {
                case   e_left_adjust:
                    if( AttrFlags.left_adjust ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.author.left_adjust );
                    AttrFlags.left_adjust = true;
                    break;
                case   e_right_adjust:
                    if( AttrFlags.right_adjust ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.author.right_adjust );
                    AttrFlags.right_adjust = true;
                    break;
                case   e_page_position:
                    if( AttrFlags.page_position ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_page_position( p, curr,
                                          &layout_work.author.page_position );
                    AttrFlags.page_position = true;
                    break;
                case   e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_font_number( p, curr, &layout_work.author.font );
                    if( layout_work.author.font >= wgml_font_cnt ) {
                        layout_work.author.font = 0;
                    }
                    AttrFlags.font = true;
                    break;
                case   e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.author.pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case   e_skip:
                    if( AttrFlags.skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.author.skip );
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
        cc = get_attr_and_value();            // get att with value
    }
    scan_start = scan_stop + 1;
    return;
}

