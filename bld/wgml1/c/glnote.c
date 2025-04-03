/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :NOTE tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :NOTE   attributes                                                    */
/***************************************************************************/

const   lay_att     note_att[8] =
    { e_left_indent, e_right_indent, e_pre_skip, e_post_skip, e_font,
      e_spacing, e_note_string, e_dummy_zero };


/*********************************************************************************/
/*Define the characteristics of the note entity.                                 */
/*:NOTE                                                                          */
/*        left_indent = 0                                                        */
/*        right_indent = 0                                                       */
/*        pre_skip = 1                                                           */
/*        post_skip = 1                                                          */
/*        font = 2                                                               */
/*        spacing = 1                                                            */
/*        note_string = "NOTE: "                                                 */
/*                                                                               */
/*left_indent This attribute accepts any valid horizontal space unit. The left   */
/*indent value is added to the current left margin. The left margin will         */
/*be reset to its previous value at the end of the note.                         */
/*                                                                               */
/*right_indent This attribute accepts any valid horizontal space unit. The right */
/*indent value is subtracted from the current right margin. The right            */
/*margin will be reset to its previous value at the end of the note.             */
/*                                                                               */
/*pre_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped before             */
/*the note. The pre-skip will be merged with the previous document               */
/*entity's post-skip value. If a pre-skip occurs at the beginning of an          */
/*output page, the pre-skip value has no effect.                                 */
/*                                                                               */
/*post_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped after              */
/*the note. The post-skip will be merged with the next document                  */
/*entity's pre-skip value. If a post-skip occurs at the end of an output         */
/*page, any remaining part of the skip is not carried over to the next           */
/*output page.                                                                   */
/*                                                                               */
/*font This attribute accepts a non-negative integer number. If a font           */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The font attribute defines the font of           */
/*the text specified by the note_string attribute. The font value is             */
/*linked to the left_indent, right_indent, pre_skip and post_skip                */
/*attributes (see "Font Linkage" on page 77).                                    */
/*                                                                               */
/*spacing This attribute accepts a positive integer number. The spacing          */
/*determines the number of blank lines that are output between text              */
/*lines. If the line spacing is two, each text line will take two lines in       */
/*the output. The number of blank lines between text lines will                  */
/*therefore be the spacing value minus one. The spacing attribute                */
/*defines the line spacing within the note.                                      */
/*                                                                               */
/*note_string This attribute accepts a character string. The specified string    */
/*precedes the text of the note. The length of this string determines            */
/*indentation of the note text.                                                  */
/*********************************************************************************/


/***************************************************************************/
/*  lay_note                                                               */
/***************************************************************************/

void    lay_note( const gmltag * entry )
{
    char        *   p;
    condcode        cc;
    int             cvterr;
    int             k;
    lay_att         curr;

    (void)entry;

    p = scan_start;
    cvterr = false;

    if( !WgmlFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_note ) {
        ProcFlags.lay_xxx = el_note;
    }
    cc = get_attr_and_value();            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = note_att[k]; curr > 0; k++, curr = note_att[k] ) {

            if( !strnicmp( att_names[curr], g_att_val.att_name, g_att_val.att_len ) ) {
                p = g_att_val.val_name;

                switch( curr ) {
                case   e_left_indent:
                    if( AttrFlags.left_indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.note.left_indent );
                    AttrFlags.left_indent = true;
                    break;
                case   e_right_indent:
                    if( AttrFlags.right_indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.note.right_indent );
                    AttrFlags.right_indent = true;
                    break;
                case   e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.note.pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case   e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.note.post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case   e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_font_number( p, curr, &layout_work.note.font );
                    if( layout_work.note.font >= wgml_font_cnt ) {
                        layout_work.note.font = 0;
                    }
                    AttrFlags.font = true;
                    break;
                case   e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_spacing( p, curr, &layout_work.note.spacing );
                    AttrFlags.spacing = true;
                    break;
                case   e_note_string:
                    if( AttrFlags.note_string ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_xx_string( p, curr, layout_work.note.string );
                    AttrFlags.note_string = true;
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

