/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :H0 - :H6 tags for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :Hx    attributes                                                     */
/***************************************************************************/

const   lay_att     hx_att[18] =
    { e_group, e_indent, e_pre_top_skip, e_pre_skip, e_post_skip,
      e_spacing, e_font, e_number_font, e_number_form, e_page_position,
      e_number_style, e_page_eject, e_line_break, e_display_heading,
      e_number_reset, e_case, e_align, e_dummy_zero };


/***********************************************************************************/
/*Define the characteristics of a heading tag,                                     */
/*where n is between zero and six inclusive.                                       */
/*:H0                                                                              */
/*        group = 0                                                                */
/*        indent = '0.5i'                                                          */
/*        pre_top_skip = 4                                                         */
/*        pre_skip = 0                                                             */
/*        post_skip = 4                                                            */
/*        spacing = 1                                                              */
/*        font = 3                                                                 */
/*        number_font = 3                                                          */
/*        number_form = none                                                       */
/*        page_position = left                                                     */
/*        number_style = h                                                         */
/*        page_eject = yes                                                         */
/*        line_break = yes                                                         */
/*        display_heading = yes                                                    */
/*        number_reset = yes                                                       */
/*        case = mixed                                                             */
/*        align = 0                                                                */
/*                                                                                 */
/*group The group attribute accepts any non-negative number between 0 and          */
/*9. The group value determines which set of headings are processed                */
/*bye the heading. tags/control words.                                             */
/*                                                                                 */
/*indent The indent attribute accepts any valid horizontal space unit. The         */
/*indent value is added to the offset determined by the page position              */
/*attribute, giving the starting offset from the left margin for the               */
/*heading.                                                                         */
/*                                                                                 */
/*pre_top_skip This attribute accepts vertical space units. A zero value means     */
/*that no lines are skipped. If the skip value is a line unit, it is multiplied    */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for            */
/*more information). The resulting amount of space is skipped before               */
/*the heading. The pre-top-skip will be merged with the previous                   */
/*document entity's post-skip value. The specified space is still                  */
/*skipped at the beginning of a new page.                                          */
/*                                                                                 */
/*post_skip This attribute accepts vertical space units. A zero value means that   */
/*no lines are skipped. If the skip value is a line unit, it is multiplied         */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for            */
/*more information). The resulting amount of space is skipped after                */
/*the heading. The post-skip will be merged with the next document                 */
/*entity's pre-skip value. If a post-skip occurs at the end of an output           */
/*page, any remaining part of the skip is not carried over to the next             */
/*output page.                                                                     */
/*                                                                                 */
/*pre_skip This attribute accepts vertical space units. A zero value means that    */
/*no lines are skipped. If the skip value is a line unit, it is multiplied         */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for            */
/*more information). The resulting amount of space is skipped before               */
/*the heading. The pre-skip will be merged with the previous                       */
/*document entity's post-skip value. If a pre-skip occurs at the                   */
/*beginning of an output page, the pre-skip value has no effect.                   */
/*                                                                                 */
/*spacing This attribute accepts a positive integer number. The spacing            */
/*determines the number of blank lines that are output between text                */
/*lines. If the line spacing is two, each text line will take two lines in         */
/*the output. The number of blank lines between text lines will                    */
/*therefore be the spacing value minus one. The spacing attribute                  */
/*defines the line spacing within the heading if it takes more than one            */
/*line.                                                                            */
/*                                                                                 */
/*font This attribute accepts a non-negative integer number. If a font             */
/*number is used for which no font has been defined, WATCOM                        */
/*Script/GML will use font zero. The font numbers from zero to three               */
/*correspond directly to the highlighting levels specified by the                  */
/*highlighting phrase GML tags. The font attribute defines the font of             */
/*the heading text. The font value is linked to the indent,                        */
/*pre_top_skip and post_skip attributes (see "Font Linkage" on page 77).           */
/*                                                                                 */
/*number_font This attribute accepts a non-negative integer number. If a font      */
/*number is used for which no font has been defined, WATCOM                        */
/*Script/GML will use font zero. The font numbers from zero to three               */
/*correspond directly to the highlighting levels specified by the                  */
/*highlighting phrase GML tags. The number font attribute defines                  */
/*the font of the heading number.                                                  */
/*                                                                                 */
/*number_form This attribute accepts the values none, prop, and new. The specified */
/*value determines the format of the heading number. The value none                */
/*indicates that no number is to be output. The value prop indicates               */
/*that the number is composed of the number for the current level                  */
/*prefixed by the number for the previous level and the number                     */
/*delimiter specified in the heading layout tag. The value new                     */
/*indicates that only the number of the current level is to be output.             */
/*                                                                                 */
/*page_position This attribute accepts the values left, right, center, and centre. */
/*The position of the heading between the left and right margins is                */
/*determined by the value selected. If left is the attribute value, the            */
/*text is output at the left margin. If right is the attribute value, the          */
/*text is output next to the right margin. When center or centre is                */
/*specified, the text is centered between the left and right margins.              */
/*                                                                                 */
/*number_style This attribute sets the number style of the heading number. (See    */
/*"Number Style" on page 121).                                                     */
/*                                                                                 */
/*page_eject This attribute accepts the keyword values yes, no, odd, and even. If  */
/*the value no is specified, the heading is one column wide and is not             */
/*forced to a new page. The heading is always placed on a new page                 */
/*when the value yes is specified. Values other than no cause the                  */
/*heading to be treated as a page wide heading in a multi-column                   */
/*document.                                                                        */
/*The values odd and even will place the heading on a new page if the              */
/*parity (odd or even) of the current page number does not match the               */
/*specified value. When two headings appear together, the attribute                */
/*value stop_eject=yes of the :heading layout tag will normally                    */
/*prevent the the second heading from going to the next page. The                  */
/*odd and even values act on the heading without regard to the                     */
/*stop_eject value.                                                                */
/*                                                                                 */
/*line_break This attribute accepts the keyword values yes and no. If the value    */
/*yes is specified, the skip value specified by the post_skip attribute            */
/*will be issued. If the value no is specified, the skip value specified           */
/*by the post_skip attribute will be ignored. If a paragraph follows               */
/*the heading, the paragraph text will start on the same line as the               */
/*heading.                                                                         */
/*                                                                                 */
/*display_heading This attribute accepts the keyword values yes and no. If the     */
/*value no is specified, the heading line will not be displayed. The heading       */
/*will still be internally created, and used in the table of contents.             */
/*                                                                                 */
/*number_reset This attribute accepts the keyword values yes and no. When a        */
/*heading is processed, all heading levels after it have their heading             */
/*numbers reset. When the value 'no' is specified, the number of the               */
/*next level of heading is not reset.                                              */
/*                                                                                 */
/*case This attribute accepts the keyword values mixed, upper and lower.           */
/*When a heading is processed, the text is converted to upper or lower             */
/*case when the values UPPER or LOWER are used. The text is left                   */
/*unchanged when the value MIXED is used.                                          */
/*                                                                                 */
/*align This attribute accepts any valid horizontal space unit. The align          */
/*value specifies the amount of space reserved for the heading. After              */
/*the heading is produced, the align value is added to the current left            */
/*margin. The left margin will be reset to its previous value after the            */
/*heading.                                                                         */
/***********************************************************************************/

/***************************************************************************/
/*  lay_hx                                                                 */
/***************************************************************************/

void    lay_hx( const gmltag * entry )
{
    char        *   p;
    condcode        cc;
    int             cvterr;
    int             hx_l;
    int             k;
    int8_t          l_group     =   0;
    lay_att         curr;

    p = scan_start;

    if( !WgmlFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    switch( entry->tagname[1] ) {
    case   '0':
        hx_l = el_h0;
        break;
    case   '1':
        hx_l = el_h1;
        break;
    case   '2':
        hx_l = el_h2;
        break;
    case   '3':
        hx_l = el_h3;
        break;
    case   '4':
        hx_l = el_h4;
        break;
    case   '5':
        hx_l = el_h5;
        break;
    case   '6':
        hx_l = el_h6;
        break;
    default:
        hx_l = el_h6;
        break;
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != hx_l ) {
        ProcFlags.lay_xxx = hx_l;
    }

    hx_l = entry->tagname[1] - '0';     // construct Hx level
    if( hx_l > 6 ) {
        hx_l = 6;
        internal_err( __FILE__, __LINE__ );
    }
    ProcFlags.hx_level = hx_l;

    cc = get_attr_and_value();            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = hx_att[k]; curr > 0; k++, curr = hx_att[k] ) {

            if( !strnicmp( att_names[curr], g_att_val.att_name, g_att_val.att_len ) ) {
                p = g_att_val.val_name;

                switch( curr ) {
                case   e_group:
                    if( AttrFlags.group ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_int8( p, curr, &l_group );
                    AttrFlags.group = true;
                    break;
                case   e_indent:
                    if( AttrFlags.indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.hx.hx_head[hx_l].indent );
                    AttrFlags.indent = true;
                    break;
                case   e_pre_top_skip:
                    if( AttrFlags.pre_top_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr, &layout_work.hx.hx_sect[hx_l].pre_top_skip );
                    AttrFlags.pre_top_skip = true;
                    break;
                case   e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr, &layout_work.hx.hx_head[hx_l].pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case   e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr, &layout_work.hx.hx_sect[hx_l].post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case   e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_spacing( p, curr, &layout_work.hx.hx_sect[hx_l].spacing );
                    AttrFlags.spacing = true;
                    break;
                case   e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_font_number( p, curr, &layout_work.hx.hx_sect[hx_l].text_font );
                    if( layout_work.hx.hx_sect[hx_l].text_font >= wgml_font_cnt ) {
                        layout_work.hx.hx_sect[hx_l].text_font = 0;
                    }
                    AttrFlags.font = true;
                    break;
                case   e_number_font:
                    if( AttrFlags.number_font ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_font_number( p, curr, &layout_work.hx.hx_head[hx_l].number_font );
                    if( layout_work.hx.hx_head[hx_l].number_font >= wgml_font_cnt ) {
                        layout_work.hx.hx_head[hx_l].number_font = 0;
                    }
                    AttrFlags.number_font = true;
                    break;
                case   e_number_form:
                    if( AttrFlags.number_form ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_number_form( p, curr, &layout_work.hx.hx_head[hx_l].number_form );
                    AttrFlags.number_form = true;
                    break;
                case   e_page_position:
                    if( AttrFlags.page_position ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_page_position( p, curr, &layout_work.hx.hx_head[hx_l].line_position );
                    AttrFlags.page_position = true;
                    break;
                case   e_number_style:
                    if( AttrFlags.number_style ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_number_style( p, curr, &layout_work.hx.hx_head[hx_l].number_style );
                    AttrFlags.number_style = true;
                    break;
                case   e_page_eject:
                    if( AttrFlags.page_eject ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_page_eject( p, curr, &layout_work.hx.hx_head[hx_l].page_eject );
                    AttrFlags.page_eject = true;
                    break;
                case   e_line_break:
                    if( AttrFlags.line_break ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_yes_no( p, curr, &layout_work.hx.hx_head[hx_l].line_break );
                    AttrFlags.line_break = true;
                    break;
                case   e_display_heading:
                    if( AttrFlags.display_heading ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_yes_no( p, curr, &layout_work.hx.hx_head[hx_l].display_heading );
                    AttrFlags.display_heading = true;
                    break;
                case   e_number_reset:
                    if( AttrFlags.number_reset ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_yes_no( p, curr, &layout_work.hx.hx_head[hx_l].number_reset );
                    AttrFlags.number_reset = true;
                    break;
                case   e_case:
                    if( AttrFlags.case_a ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_case( p, curr, &layout_work.hx.hx_head[hx_l].hd_case );
                    AttrFlags.case_a = true;
                    break;
                case   e_align:
                    if( AttrFlags.align ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr, &layout_work.hx.hx_head[hx_l].align );
                    AttrFlags.align = true;
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
    if( l_group > 0 ) {         // only group 0 is supported
        xx_warn_c( wng_unsupp_lay_att, "group" );
    }
    scan_start = scan_stop + 1;
    return;
}

