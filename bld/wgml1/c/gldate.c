/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :DATE  tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :DATE     attributes                                                  */
/***************************************************************************/
const   lay_att     date_att[7] =
    { e_date_form, e_left_adjust, e_right_adjust,
      e_page_position, e_font, e_pre_skip, e_dummy_zero };

/**********************************************************************************/
/*Defines the characteristics of the date entity in the standard tag format.      */
/*The :letdate layout tag defines the characteristics of the date entity in       */
/*the letter tag format.                                                          */
/*                                                                                */
/*:DATE                                                                           */
/*        date_form = "$ml $dsn, $yl"                                             */
/*        left_adjust = 0                                                         */
/*        right_adjust = '1i'                                                     */
/*        page_position = right                                                   */
/*        font = 0                                                                */
/*        pre_skip = 2                                                            */
/*                                                                                */
/*date_form The date_form attribute accepts a character string value which        */
/*defines the format of the date string. The year, month and day may              */
/*be specified separately and in any order by special date sequences.             */
/*These date sequences are started with a dollar($) sign and followed             */
/*by one to three characters. Text which is not recognized as a date              */
/*sequence can be entered to tailor the format of the resulting date.             */
/*The first character in a date sequence is a Y for the year, an M for            */
/*the month, or a D for the day. The next character is the L or S                 */
/*character to specify the long or short form of the date sequence. If            */
/*neither of these characters are present, the long form is used. When            */
/*the length specifier is present, the N character is used to format the          */
/*month or the day as a number. If the length specified is not present,           */
/*the month and day values are created in character form. The year is             */
/*always formatted as a number.                                                   */
/*                                                                                */
/*left_adjust The left_adjust attribute accepts any valid horizontal space unit.  */
/*The left margin is set to the page left margin plus the specified left          */
/*adjustment.                                                                     */
/*                                                                                */
/*right_adjust The right_adjust attribute accepts any valid horizontal space unit.*/
/*The right margin is set to the page right margin minus the specified            */
/*right adjustment.                                                               */
/*                                                                                */
/*page_position This attribute accepts the values left, right, center, and centre.*/
/*The position of the date between the left and right margins is determined       */
/*by the value selected. If left is the attribute value, the text is output       */
/*at the left margin. If right is the attribute value, the text is output         */
/*next to the right margin. When center or centre is specified, the text          */
/*is centered between the left and right margins.                                 */
/*                                                                                */
/*font This attribute accepts a non-negative integer number. If a font            */
/*number is used for which no font has been defined, WATCOM                       */
/*Script/GML will use font zero. The font numbers from zero to three              */
/*correspond directly to the highlighting levels specified by the                 */
/*highlighting phrase GML tags. The font attribute defines the font of            */
/*the date text. The font value is linked to the left_adjust, right_adjust        */
/*and pre_skip attributes (see "Font Linkage" on page 77).                        */
/*                                                                                */
/*pre_skip This attribute accepts vertical space units. A zero value means that   */
/*no lines are skipped. If the skip value is a line unit, it is multiplied        */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped before              */
/*the date. The pre-skip will be merged with the previous document                */
/*entity's post-skip value. If a pre-skip occurs at the beginning of an           */
/*output page, the pre-skip value has no effect.                                  */
/**********************************************************************************/


/***************************************************************************/
/*  lay_date                                                               */
/***************************************************************************/

void    lay_date( const gmltag * entry )
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
    if( ProcFlags.lay_xxx != el_date ) {
        ProcFlags.lay_xxx = el_date;
    }
    cc = lay_attr_and_value( &lay_attr );            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = date_att[k]; curr > 0; k++, curr = date_att[k] ) {
            if( strcmp( lay_att_names[curr], lay_attr.attname ) == 0 ) {
                p = lay_attr.val.name;
                switch( curr ) {
                case e_date_form:
                    if( AttrFlags.date_form ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_date_form( p, &lay_attr, layout_work.date.date_form );
                    AttrFlags.date_form = true;
                    break;
                case e_left_adjust:
                    if( AttrFlags.left_adjust ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.date.left_adjust );
                    AttrFlags.left_adjust = true;
                    break;
                case e_right_adjust:
                    if( AttrFlags.right_adjust ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.date.right_adjust );
                    AttrFlags.right_adjust = true;
                    break;
                case e_page_position:
                    if( AttrFlags.page_position ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_page_position( p, &lay_attr,
                                          &layout_work.date.page_position );
                    AttrFlags.page_position = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_font_number( p, &lay_attr, &layout_work.date.font );
                    if( layout_work.date.font >= wgml_font_cnt ) {
                        layout_work.date.font = 0;
                    }
                    AttrFlags.font = true;
                    break;
                case e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.date.pre_skip );
                    AttrFlags.pre_skip = true;
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

