/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :DL :GL :OL :SL :UL tags for LAYOUT processing
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :DL       attributes                                                  */
/***************************************************************************/
const   lay_att     dl_att[10] =
    { e_level, e_left_indent, e_right_indent, e_pre_skip, e_skip,
      e_spacing, e_post_skip, e_align, e_line_break, e_dummy_zero };

/***************************************************************************/
/*   :GL       attributes                                                  */
/***************************************************************************/
const   lay_att     gl_att[10] =
    { e_level, e_left_indent, e_right_indent, e_pre_skip, e_skip,
      e_spacing, e_post_skip, e_align, e_delim, e_dummy_zero };

/***************************************************************************/
/*   :OL       attributes                                                  */
/***************************************************************************/
const   lay_att     ol_att[12] =
    { e_level, e_left_indent, e_right_indent, e_pre_skip, e_skip,
      e_spacing, e_post_skip, e_font, e_align, e_number_style, e_number_font,
      e_dummy_zero };

/***************************************************************************/
/*   :SL       attributes                                                  */
/***************************************************************************/
const   lay_att     sl_att[9] =
    { e_level, e_left_indent, e_right_indent, e_pre_skip, e_skip,
      e_spacing, e_post_skip, e_font, e_dummy_zero };

/***************************************************************************/
/*   :UL       attributes                                                  */
/***************************************************************************/
const   lay_att     ul_att[13] =
    { e_level, e_left_indent, e_right_indent, e_pre_skip, e_skip,
      e_spacing, e_post_skip, e_font, e_align, e_bullet, e_bullet_translate,
      e_bullet_font, e_dummy_zero };


/********************************************************************************/
/*Define the characteristics of the definition list entity.                     */
/*:DL                                                                           */
/*        level = 1                                                             */
/*        left_indent = 0                                                       */
/*        right_indent = 0                                                      */
/*        pre_skip = 1                                                          */
/*        skip = 1                                                              */
/*        spacing = 1                                                           */
/*        post_skip = 1                                                         */
/*        align = '1i'                                                          */
/*        line_break = no                                                       */
/*                                                                              */
/*level This attribute accepts a positive integer number. If not specified, a   */
/*level value of '1'. is assumed. Each list level is separately               */
/*specified. For example, if two levels of the ordered list are                 */
/*specified, the :dl tag will be specified twice in the layout. When            */
/*some attributes for a new level of a list are not specified, the default      */
/*values for those attributes will be the values of the first level. Since      */
/*list levels may not be skipped, each new level of list must be                */
/*sequentially defined from the last specified level.                           */
/*if there is an ordered, simple, and second ordered list nested                */
/*together in the document, the simple and first ordered list will both         */
/*be from level one, while the last ordered list will be level two. The         */
/*appropriate level number is selected based on the nesting level of a          */
/*particular list type. If a list type is nested beyond the levels              */
/*specified in the layout, the levels are "cycled". For example, if there       */
/*are two levels of ordered list specified in the layout, and there are         */
/*three ordered lists nested, the third level of ordered list will use the      */
/*attributes of the level one ordered list. A fourth nested list would          */
/*use the attributes of the level two.                                          */
/*                                                                              */
/*left_indent This attribute accepts any valid horizontal space unit. The left  */
/*indent value is added to the current left margin. The left margin will        */
/*be reset to its previous value at the end of the definition list.             */
/*                                                                              */
/*right_indent This attribute accepts any valid horizontal space unit. The right*/
/*indent value is subtracted from the current right margin. The right           */
/*margin will be reset to its previous value at the end of the definition       */
/*list.                                                                         */
/*                                                                              */
/*pre_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped before            */
/*the definition list. The pre-skip will be merged with the previous            */
/*document entityٿs post-skip value. If a pre-skip occurs at the               */
/*beginning of an output page, the pre-skip value has no effect.                */
/*                                                                              */
/*skip This attribute accepts vertical space units. A zero value means that     */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped                   */
/*between each item of the definition list.                                     */
/*                                                                              */
/*spacing This attribute accepts a positive integer number. The spacing         */
/*determines the number of blank lines that are output between text             */
/*lines. If the line spacing is two, each text line will take two lines in      */
/*the output. The number of blank lines between text lines will                 */
/*therefore be the spacing value minus one. The spacing attribute               */
/*defines the line spacing within the items of the definition list.             */
/*                                                                              */
/*post_skip This attribute accepts vertical space units. A zero value means that*/
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped after             */
/*the definition list. The post-skip will be merged with the next               */
/*document entity's pre-skip value. If a post-skip occurs at the end of         */
/*an output page, any remaining part of the skip is not carried over to         */
/*the next output page.                                                         */
/*                                                                              */
/*align This attribute accepts any valid horizontal space unit. The align       */
/*value specifies the amount of space reserved for the definition term.         */
/*After the definition term is produced, the align value is added to the        */
/*current left margin. The left margin will be reset to its previous            */
/*value after the definition list item.                                         */
/*                                                                              */
/*line_break This attribute accepts the keyword values yes and no. If the value */
/*yes is specified, the data description starts a new line after the            */
/*definition term if the length of the term is larger than align value. If      */
/*the value no is specified, the definition term is allowed to intrude          */
/*into the data description area.                                               */
/********************************************************************************/

/*********************************************************************************/
/*Define the characteristics of the glossary list entity.                        */
/*:GL                                                                            */
/*        level = 1                                                              */
/*        left_indent = 0                                                        */
/*        right_indent = 0                                                       */
/*        pre_skip = 1                                                           */
/*        skip = 1                                                               */
/*        spacing = 1                                                            */
/*        post_skip = 1                                                          */
/*        align = 0                                                              */
/*        delim = ':'                                                            */
/*                                                                               */
/*level This attribute accepts a positive integer number. If not specified, a    */
/*level value of '1'. is assumed. Each list level is separately                  */
/*specified. For example, if two levels of the ordered list are                  */
/*specified, the :gl tag will be specified twice in the layout. When             */
/*some attributes for a new level of a list are not specified, the default       */
/*values for those attributes will be the values of the first level. Since       */
/*list levels may not be skipped, each new level of list must be                 */
/*sequentially defined from the last specified level.                            */
/*if there is an ordered, simple, and second ordered list nested                 */
/*together in the document, the simple and first ordered list will both          */
/*be from level one, while the last ordered list will be level two. The          */
/*appropriate level number is selected based on the nesting level of a           */
/*particular list type. If a list type is nested beyond the levels               */
/*specified in the layout, the levels are "cycled". For example, if there        */
/*are two levels of ordered list specified in the layout, and there are          */
/*three ordered lists nested, the third level of ordered list will use the       */
/*attributes of the level one ordered list. A fourth nested list would           */
/*use the attributes of the level two.                                           */
/*                                                                               */
/*left_indent This attribute accepts any valid horizontal space unit. The left   */
/*indent value is added to the current left margin. The left margin will         */
/*be reset to its previous value at the end of the glossary list.                */
/*right_indent This attribute accepts any valid horizontal space unit. The right */
/*indent value is subtracted from the current right margin. The right            */
/*margin will be reset to its previous value at the end of the glossary          */
/*list.                                                                          */
/*                                                                               */
/*pre_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped before             */
/*the glossary list. The pre-skip will be merged with the previous               */
/*document entity's post-skip value. If a pre-skip occurs at the                 */
/*beginning of an output page, the pre-skip value has no effect.                 */
/*                                                                               */
/*skip This attribute accepts vertical space units. A zero value means that      */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped                    */
/*between each item of the glossary list.                                        */
/*                                                                               */
/*spacing This attribute accepts a positive integer number. The spacing          */
/*determines the number of blank lines that are output between text              */
/*lines. If the line spacing is two, each text line will take two lines in       */
/*the output. The number of blank lines between text lines will                  */
/*therefore be the spacing value minus one. The spacing attribute                */
/*defines the line spacing within the items of the glossary list.                */
/*                                                                               */
/*post_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped after              */
/*the glossary list. The post-skip will be merged with the next                  */
/*document entity's pre-skip value. If a post-skip occurs at the end of          */
/*an output page, any remaining part of the skip is not carried over to          */
/*the next output page.                                                          */
/*                                                                               */
/*align This attribute accepts any valid horizontal space unit. The align        */
/*value specifies the amount of space reserved for the glossary term.            */
/*After the glossary term is produced, the align value is added to the           */
/*current left margin. The left margin will be reset to its previous             */
/*value after the glossary list item.                                            */
/*delim The quoted character value is used to separate the glossary term         */
/*from the glossary description.                                                 */
/*********************************************************************************/

/*********************************************************************************/
/*Define the characteristics of the ordered list entity.                         */
/*:OL                                                                            */
/*        level = 1                                                              */
/*        left_indent = 0                                                        */
/*        right_indent = 0                                                       */
/*        pre_skip = 1                                                           */
/*        skip = 1                                                               */
/*        spacing = 1                                                            */
/*        post_skip = 1                                                          */
/*        font = 0                                                               */
/*        align = '0.4i'                                                         */
/*        number_style = hd                                                      */
/*        number_font = 0                                                        */
/*                                                                               */
/*level This attribute accepts a positive integer number. If not specified, a    */
/*level value of '1'. is assumed. Each list level is separately                  */
/*specified. For example, if two levels of the ordered list are                  */
/*specified, the :ol tag will be specified twice in the layout. When             */
/*some attributes for a new level of a list are not specified, the default       */
/*values for those attributes will be the values of the first level. Since       */
/*list levels may not be skipped, each new level of list must be                 */
/*sequentially defined from the last specified level.                            */
/*if there is an ordered, simple, and second ordered list nested                 */
/*together in the document, the simple and first ordered list will both          */
/*be from level one, while the last ordered list will be level two. The          */
/*appropriate level number is selected based on the nesting level of a           */
/*particular list type. If a list type is nested beyond the levels               */
/*specified in the layout, the levels are "cycled". For example, if there        */
/*are two levels of ordered list specified in the layout, and there are          */
/*three ordered lists nested, the third level of ordered list will use the       */
/*attributes of the level one ordered list. A fourth nested list would           */
/*use the attributes of the level two.                                           */
/*                                                                               */
/*left_indent This attribute accepts any valid horizontal space unit. The left   */
/*indent value is added to the current left margin. The left margin will         */
/*be reset to its previous value at the end of the ordered list.                 */
/*right_indent This attribute accepts any valid horizontal space unit. The right */
/*indent value is subtracted from the current right margin. The right            */
/*margin will be reset to its previous value at the end of the ordered           */
/*list.                                                                          */
/*                                                                               */
/*pre_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped before             */
/*the ordered list. The pre-skip will be merged with the previous                */
/*document entity's post-skip value. If a pre-skip occurs at the                 */
/*beginning of an output page, the pre-skip value has no effect.                 */
/*                                                                               */
/*skip This attribute accepts vertical space units. A zero value means that      */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped                    */
/*between list items.                                                            */
/*                                                                               */
/*spacing This attribute accepts a positive integer number. The spacing          */
/*determines the number of blank lines that are output between text              */
/*lines. If the line spacing is two, each text line will take two lines in       */
/*the output. The number of blank lines between text lines will                  */
/*therefore be the spacing value minus one. The spacing attribute                */
/*defines the line spacing within the list item.                                 */
/*                                                                               */
/*post_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped after              */
/*the ordered list. The post-skip will be merged with the next                   */
/*document entity's pre-skip value. If a post-skip occurs at the end of          */
/*an output page, any remaining part of the skip is not carried over to          */
/*the next output page.                                                          */
/*                                                                               */
/*font This attribute accepts a non-negative integer number. If a font           */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The font attribute defines the font of           */
/*the ordered list text. The font value is linked to the left_indent,            */
/*right_indent, pre_skip, post_skip and skip attributes (see "Font               */
/*Linkage" on page 77).                                                          */
/*                                                                               */
/*align This attribute accepts any valid horizontal space unit. The align        */
/*value specifies the amount of space reserved for the list item                 */
/*number. After the list item number is produced, the align value is             */
/*added to the current left margin. The left margin will be reset to its         */
/*previous value after the list item.                                            */
/*                                                                               */
/*number_style This attribute sets the number style of the list item number.     */
/*(See "Number Style" on page 121).                                              */
/*                                                                               */
/*number_font This attribute accepts a non-negative integer number. If a font    */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The number font attribute defines                */
/*the font of the list item number. The font value is linked to the align        */
/*attibute (see "Font Linkage" on page 77).                                      */
/*********************************************************************************/

/********************************************************************************/
/*Define the characteristics of the simple list entity.                         */
/*:SL                                                                           */
/*        level = 1                                                             */
/*        left_indent = 0                                                       */
/*        right_indent = 0                                                      */
/*        pre_skip = 1                                                          */
/*        skip = 1                                                              */
/*        spacing = 1                                                           */
/*        post_skip = 1                                                         */
/*        font = 0                                                              */
/*                                                                              */
/*level This attribute accepts a positive integer number. If not specified, a   */
/*level value of '1'. is assumed. Each list level is separately                 */
/*specified. For example, if two levels of the ordered list are                 */
/*specified, the :sl tag will be specified twice in the layout. When            */
/*some attributes for a new level of a list are not specified, the default      */
/*values for those attributes will be the values of the first level. Since      */
/*list levels may not be skipped, each new level of list must be                */
/*sequentially defined from the last specified level.                           */
/*if there is an ordered, simple, and second ordered list nested                */
/*together in the document, the simple and first ordered list will both         */
/*be from level one, while the last ordered list will be level two. The         */
/*appropriate level number is selected based on the nesting level of a          */
/*particular list type. If a list type is nested beyond the levels              */
/*specified in the layout, the levels are "cycled". For example, if there       */
/*are two levels of ordered list specified in the layout, and there are         */
/*three ordered lists nested, the third level of ordered list will use the      */
/*attributes of the level one ordered list. A fourth nested list would          */
/*use the attributes of the level two.                                          */
/*                                                                              */
/*left_indent This attribute accepts any valid horizontal space unit. The left  */
/*indent value is added to the current left margin. The left margin will        */
/*be reset to its previous value at the end of the simple list.                 */
/*                                                                              */
/*right_indent This attribute accepts any valid horizontal space unit. The right*/
/*indent value is subtracted from the current right margin. The right           */
/*margin will be reset to its previous value at the end of the simple           */
/*list.                                                                         */
/*                                                                              */
/*pre_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped before            */
/*the simple list. The pre-skip will be merged with the previous                */
/*document entity's post-skip value. If a pre-skip occurs at the                */
/*beginning of an output page, the pre-skip value has no effect.                */
/*                                                                              */
/*skip This attribute accepts vertical space units. A zero value means that     */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped                   */
/*between list items.                                                           */
/*                                                                              */
/*spacing This attribute accepts a positive integer number. The spacing         */
/*determines the number of blank lines that are output between text             */
/*lines. If the line spacing is two, each text line will take two lines in      */
/*the output. The number of blank lines between text lines will                 */
/*therefore be the spacing value minus one. The spacing attribute               */
/*defines the line spacing within the list items.                               */
/*                                                                              */
/*post_skip This attribute accepts vertical space units. A zero value means that*/
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped after             */
/*the simple list. The post-skip will be merged with the next                   */
/*document entity's pre-skip value. If a post-skip occurs at the end of         */
/*an output page, any remaining part of the skip is not carried over to         */
/*the next output page.                                                         */
/*                                                                              */
/*font This attribute accepts a non-negative integer number. If a font          */
/*number is used for which no font has been defined, WATCOM                     */
/*Script/GML will use font zero. The font numbers from zero to three            */
/*correspond directly to the highlighting levels specified by the               */
/*highlighting phrase GML tags. The font attribute defines the font of          */
/*the simple list text. The font value is linked to the left_indent,            */
/*right_indent, pre_skip, post_skip and skip attributes (see "Font              */
/*Linkage" on page 77).                                                         */
/********************************************************************************/

/********************************************************************************/
/*Define the characteristics of the unordered list entity.                      */
/*:UL                                                                           */
/*        level = 1                                                             */
/*        left_indent = 0                                                       */
/*        right_indent = 0                                                      */
/*        pre_skip = 1                                                          */
/*        skip = 1                                                              */
/*        spacing = 1                                                           */
/*        post_skip = 1                                                         */
/*        font = 0                                                              */
/*        align = '0.4i'                                                        */
/*        bullet = '*'                                                          */
/*        bullet_translate = yes                                                */
/*        bullet_font = 0                                                       */
/*                                                                              */
/*level This attribute accepts a positive integer number. If not specified, a   */
/*level value of '1'. is assumed. Each list level is separately                 */
/*specified. For example, if two levels of the ordered list are                 */
/*specified, the :ul tag will be specified twice in the layout. When            */
/*some attributes for a new level of a list are not specified, the default      */
/*values for those attributes will be the values of the first level. Since      */
/*list levels may not be skipped, each new level of list must be                */
/*sequentially defined from the last specified level.                           */
/*if( there is an ordered, simple, and second ordered list nested ) {           */
/*together in the document, the simple and first ordered list will both         */
/*be from level one, while the last ordered list will be level two. The         */
/*appropriate level number is selected based on the nesting level of a          */
/*particular list type. If a list type is nested beyond the levels              */
/*specified in the layout, the levels are "cycled". For example, if there       */
/*are two levels of ordered list specified in the layout, and there are         */
/*three ordered lists nested, the third level of ordered list will use the      */
/*attributes of the level one ordered list. A fourth nested list would          */
/*use the attributes of the level two.                                          */
/*                                                                              */
/*left_indent This attribute accepts any valid horizontal space unit. The left  */
/*indent value is added to the current left margin. The left margin will        */
/*be reset to its previous value at the end of the unordered list.              */
/*                                                                              */
/*right_indent This attribute accepts any valid horizontal space unit. The right*/
/*indent value is subtracted from the current right margin. The right           */
/*margin will be reset to its previous value at the end of the unordered        */
/*list.                                                                         */
/*                                                                              */
/*pre_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped before            */
/*the unordered list. The pre-skip will be merged with the previous             */
/*document entity's post-skip value. If a pre-skip occurs at the                */
/*beginning of an output page, the pre-skip value has no effect.                */
/*                                                                              */
/*skip This attribute accepts vertical space units. A zero value means that     */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped                   */
/*between list items.                                                           */
/*                                                                              */
/*spacing This attribute accepts a positive integer number. The spacing         */
/*determines the number of blank lines that are output between text             */
/*lines. If the line spacing is two, each text line will take two lines in      */
/*the output. The number of blank lines between text lines will                 */
/*therefore be the spacing value minus one. The spacing attribute               */
/*defines the line spacing within the list items.                               */
/*                                                                              */
/*post_skip This attribute accepts vertical space units. A zero value means that*/
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped after             */
/*the unordered list. The post-skip will be merged with the next                */
/*document entity's pre-skip value. If a post-skip occurs at the end of         */
/*an output page, any remaining part of the skip is not carried over to         */
/*the next output page.                                                         */
/*                                                                              */
/*font This attribute accepts a non-negative integer number. If a font          */
/*number is used for which no font has been defined, WATCOM                     */
/*Script/GML will use font zero. The font numbers from zero to three            */
/*correspond directly to the highlighting levels specified by the               */
/*highlighting phrase GML tags. The font attribute defines the font of          */
/*the unordered list text. The font value is linked to the left_indent,         */
/*right_indent, pre_skip, post_skip and skip attributes (see "Font              */
/*Linkage" on page 77).                                                         */
/*                                                                              */
/*align This attribute accepts any valid horizontal space unit. The align       */
/*value specifies the amount of space reserved for the list item bullet.        */
/*After the list item bullet is produced, the align value is added to the       */
/*current left margin. The left margin will be reset to its previous            */
/*value after the list item.                                                    */
/*                                                                              */
/*bullet This attribute specifies the single character value which annotates an */
/*unordered list item.                                                          */
/*                                                                              */
/*bullet_translate This attribute accepts the keyword values yes and no.        */
/*If 'yes' is specified, input translation is performed on the annotation       */
/*character.                                                                    */
/*                                                                              */
/*bullet_font This attribute accepts a non-negative integer number. If a font   */
/*number is used for which no font has been defined, WATCOM                     */
/*Script/GML will use font zero. The font numbers from zero to three            */
/*correspond directly to the highlighting levels specified by the               */
/*highlighting phrase GML tags. The bullet_font attribute defines the           */
/*font of the annotation character. The font value is linked to the             */
/*align attibute (see "Font Linkage" on page 77).                               */
/********************************************************************************/


/***************************************************************************/
/*  lay_dl                                                                 */
/***************************************************************************/

void    lay_dl( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    dl_lay_level    *   curr_level  = NULL;
    dl_lay_level    *   dl_layout   = NULL;
    int                 k;
    lay_att             curr;

    (void)entry;

    p = scan_start;

    if( !WgmlFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_dl ) {
        ProcFlags.lay_xxx = el_dl;
    }

    /* Make a copy of the level 1 DL LAYOUT */

    dl_layout = mem_alloc( sizeof( dl_lay_level ) );
    dl_layout->next = NULL;
    dl_layout->left_indent = layout_work.dl.first->left_indent;
    dl_layout->right_indent = layout_work.dl.first->right_indent;
    dl_layout->pre_skip = layout_work.dl.first->pre_skip;
    dl_layout->skip = layout_work.dl.first->skip;
    dl_layout->post_skip = layout_work.dl.first->post_skip;
    dl_layout->align = layout_work.dl.first->align;
    dl_layout->spacing = layout_work.dl.first->spacing;
    dl_layout->line_break = layout_work.dl.first->line_break;
    dl_layout->level = layout_work.dl.first->level;

    /* Get the DL LAYOUT settings */

    cc = get_attr_and_value();            // get att with value
    while( cc == pos ) {
        for( k = 0, curr = dl_att[k]; curr > 0; k++, curr = dl_att[k] ) {

            if( strnicmp( att_names[curr], g_att_val.att_name, g_att_val.att_len ) == 0 ) {
                p = g_att_val.val_name;

                switch( curr ) {
                case   e_level:
                    if( AttrFlags.level ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_int8( p, curr, &dl_layout->level );
                    AttrFlags.level = true;
                    break;
                case   e_left_indent:
                    if( AttrFlags.left_indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &dl_layout->left_indent );
                    AttrFlags.left_indent = true;
                    break;
                case   e_right_indent:
                    if( AttrFlags.right_indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &dl_layout->right_indent );
                    AttrFlags.right_indent = true;
                    break;
                case   e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &dl_layout->pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case   e_skip:
                    if( AttrFlags.skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &dl_layout->skip );
                    AttrFlags.skip = true;
                    break;
                case   e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_spacing( p, curr, &dl_layout->spacing );
                    AttrFlags.spacing = true;
                    break;
                case   e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &dl_layout->post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case   e_align:
                    if( AttrFlags.align ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &dl_layout->align );
                    AttrFlags.align = true;
                    break;
                case   e_line_break:
                    if( AttrFlags.line_break ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_yes_no( p, curr, &dl_layout->line_break );
                    AttrFlags.line_break = true;
                    break;
                default:
                    internal_err( __FILE__, __LINE__ );
                }
                break;                  // break out of for loop
            }
        }
        cc = get_attr_and_value();            // get att with value
    }

    /* Now integrate the DL LAYOUT into the linked list */

    curr_level = layout_work.dl.first;
    while( curr_level->next != NULL ) {
        if( curr_level->level >= dl_layout->level ) {
            break;
        }
        curr_level = curr_level->next;
    }

    if( curr_level->level == dl_layout->level ) {

        /* Only items changed from the level 1 layout are to be copied */

        if( strcmp( dl_layout->left_indent.su_txt,
                layout_work.dl.first->left_indent.su_txt ) != 0 ) {
            curr_level->left_indent = dl_layout->left_indent;
        }
        if( strcmp( dl_layout->right_indent.su_txt,
                layout_work.dl.first->right_indent.su_txt ) != 0 ) {
            curr_level->right_indent = dl_layout->right_indent;
        }
        if( strcmp( dl_layout->pre_skip.su_txt,
                layout_work.dl.first->pre_skip.su_txt ) != 0 ) {
            curr_level->pre_skip = dl_layout->pre_skip;
        }
        if( strcmp( dl_layout->skip.su_txt,
                layout_work.dl.first->skip.su_txt ) != 0 ) {
            curr_level->skip = dl_layout->skip;
        }
        if( strcmp( dl_layout->post_skip.su_txt,
                layout_work.dl.first->post_skip.su_txt ) != 0 ) {
            curr_level->post_skip = dl_layout->post_skip;
        }
        if( strcmp( dl_layout->align.su_txt,
                layout_work.dl.first->align.su_txt ) != 0 ) {
            curr_level->align = dl_layout->align;
        }
        if( dl_layout->spacing != layout_work.dl.first->spacing ) {
            curr_level->spacing = dl_layout->spacing;
        }
        if( dl_layout->line_break != layout_work.dl.first->line_break ) {
            curr_level->line_break = dl_layout->line_break;
        }
        mem_free( dl_layout );
    } else if( curr_level->next == NULL ) {
        curr_level->next = dl_layout;
        layout_work.dl.max_level = dl_layout->level;
    } else if( curr_level == layout_work.dl.first ) {
        dl_layout->next = layout_work.dl.first;
        layout_work.dl.first = dl_layout;
    } else {
        dl_layout->next = curr_level->next;
        curr_level->next = dl_layout;
    }

    scan_start = scan_stop + 1;
    return;
}

/***************************************************************************/
/*  lay_gl                                                                 */
/***************************************************************************/

void    lay_gl( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    gl_lay_level    *   curr_level  = NULL;
    gl_lay_level    *   gl_layout   = NULL;
    int                 k;
    lay_att             curr;

    (void)entry;

    p = scan_start;

    if( !WgmlFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_gl ) {
        ProcFlags.lay_xxx = el_gl;
    }

    /* Make a copy of the level 1 GL LAYOUT */

    gl_layout = mem_alloc( sizeof( gl_lay_level ) );
    gl_layout->next = NULL;
    gl_layout->left_indent = layout_work.gl.first->left_indent;
    gl_layout->right_indent = layout_work.gl.first->right_indent;
    gl_layout->pre_skip = layout_work.gl.first->pre_skip;
    gl_layout->skip = layout_work.gl.first->skip;
    gl_layout->post_skip = layout_work.gl.first->post_skip;
    gl_layout->align = layout_work.gl.first->align;
    gl_layout->spacing = layout_work.gl.first->spacing;
    gl_layout->delim = layout_work.gl.first->delim;
    gl_layout->level = layout_work.gl.first->level;

    /* Get the GL LAYOUT settings */

    cc = get_attr_and_value();            // get att with value
    while( cc == pos ) {
        for( k = 0, curr = gl_att[k]; curr > 0; k++, curr = gl_att[k] ) {

            if( strnicmp( att_names[curr], g_att_val.att_name, g_att_val.att_len ) == 0 ) {
                p = g_att_val.val_name;

                switch( curr ) {
                case   e_level:
                    if( AttrFlags.level ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_int8( p, curr, &gl_layout->level );
                    AttrFlags.level = true;
                    break;
                case   e_left_indent:
                    if( AttrFlags.left_indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &gl_layout->left_indent );
                    AttrFlags.left_indent = true;
                    break;
                case   e_right_indent:
                    if( AttrFlags.right_indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &gl_layout->right_indent );
                    AttrFlags.right_indent = true;
                    break;
                case   e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &gl_layout->pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case   e_skip:
                    if( AttrFlags.skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &gl_layout->skip );
                    AttrFlags.skip = true;
                    break;
                case   e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_spacing( p, curr, &gl_layout->spacing );
                    AttrFlags.spacing = true;
                    break;
                case   e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &gl_layout->post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case   e_align:
                    if( AttrFlags.align ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &gl_layout->align );
                    AttrFlags.align = true;
                    break;
                case   e_delim:
                    if( AttrFlags.delim ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_char( p, curr, &gl_layout->delim );
                    AttrFlags.delim = true;
                    break;
                default:
                    internal_err( __FILE__, __LINE__ );
                }
                break;                  // break out of for loop
            }
        }
        cc = get_attr_and_value();            // get att with value
    }

    /* Now integrate the GL LAYOUT into the linked list */

    curr_level = layout_work.gl.first;
    while( curr_level->next != NULL ) {
        if( curr_level->level >= gl_layout->level ) {
            break;
        }
        curr_level = curr_level->next;
    }

    if( curr_level->level == gl_layout->level ) {

        /* Only items changed from the level 1 layout are to be copied */

        if( strcmp( gl_layout->left_indent.su_txt,
                layout_work.gl.first->left_indent.su_txt ) != 0 ) {
            curr_level->left_indent = gl_layout->left_indent;
        }
        if( strcmp( gl_layout->right_indent.su_txt,
                layout_work.gl.first->right_indent.su_txt ) != 0 ) {
            curr_level->right_indent = gl_layout->right_indent;
        }
        if( strcmp( gl_layout->pre_skip.su_txt,
                layout_work.gl.first->pre_skip.su_txt ) != 0 ) {
            curr_level->pre_skip = gl_layout->pre_skip;
        }
        if( strcmp( gl_layout->skip.su_txt,
                layout_work.gl.first->skip.su_txt ) != 0 ) {
            curr_level->skip = gl_layout->skip;
        }
        if( strcmp( gl_layout->post_skip.su_txt,
                layout_work.gl.first->post_skip.su_txt ) != 0 ) {
            curr_level->post_skip = gl_layout->post_skip;
        }
        if( strcmp( gl_layout->align.su_txt,
                layout_work.gl.first->align.su_txt ) != 0 ) {
            curr_level->align = gl_layout->align;
        }
        if( gl_layout->spacing != layout_work.gl.first->spacing ) {
            curr_level->spacing = gl_layout->spacing;
        }
        if( gl_layout->delim != layout_work.gl.first->delim ) {
            curr_level->delim = gl_layout->delim;
        }
        mem_free( gl_layout );
    } else if( curr_level->next == NULL ) {
        curr_level->next = gl_layout;
        layout_work.gl.max_level = gl_layout->level;
    } else if( curr_level == layout_work.gl.first ) {
        gl_layout->next = layout_work.gl.first;
        layout_work.gl.first = gl_layout;
    } else {
        gl_layout->next = curr_level->next;
        curr_level->next = gl_layout;
    }

    scan_start = scan_stop + 1;
    return;
}

/***************************************************************************/
/*  lay_ol                                                                 */
/***************************************************************************/

void    lay_ol( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    ol_lay_level    *   curr_level  = NULL;
    ol_lay_level    *   ol_layout   = NULL;

    (void)entry;

    p = scan_start;

    if( !WgmlFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_ol ) {
        ProcFlags.lay_xxx = el_ol;
    }

    /* Make a copy of the level 1 OL LAYOUT */

    ol_layout = mem_alloc( sizeof( ol_lay_level ) );
    ol_layout->next = NULL;
    ol_layout->left_indent = layout_work.ol.first->left_indent;
    ol_layout->right_indent = layout_work.ol.first->right_indent;
    ol_layout->pre_skip = layout_work.ol.first->pre_skip;
    ol_layout->skip = layout_work.ol.first->skip;
    ol_layout->post_skip = layout_work.ol.first->post_skip;
    ol_layout->align = layout_work.ol.first->align;
    ol_layout->spacing = layout_work.ol.first->spacing;
    ol_layout->font = layout_work.ol.first->font;
    ol_layout->number_style = layout_work.ol.first->number_style;
    ol_layout->number_font = layout_work.ol.first->number_font;
    ol_layout->level = layout_work.ol.first->level;

    /* Get the OL LAYOUT settings */

    cc = get_attr_and_value();            // get att with value
    while( cc == pos ) {
        for( k = 0, curr = ol_att[k]; curr > 0; k++, curr = ol_att[k] ) {

            if( strnicmp( att_names[curr], g_att_val.att_name, g_att_val.att_len ) == 0 ) {
                p = g_att_val.val_name;

                switch( curr ) {
                case   e_level:
                    if( AttrFlags.level ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_int8( p, curr, &ol_layout->level );
                    AttrFlags.level = true;
                    break;
                case   e_left_indent:
                    if( AttrFlags.left_indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &ol_layout->left_indent );
                    AttrFlags.left_indent = true;
                    break;
                case   e_right_indent:
                    if( AttrFlags.right_indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &ol_layout->right_indent );
                    AttrFlags.right_indent = true;
                    break;
                case   e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &ol_layout->pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case   e_skip:
                    if( AttrFlags.skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &ol_layout->skip );
                    AttrFlags.skip = true;
                    break;
                case   e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_spacing( p, curr, &ol_layout->spacing );
                    AttrFlags.spacing = true;
                    break;
                case   e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &ol_layout->post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case   e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_font_number( p, curr, &ol_layout->font );
                    if( ol_layout->font >= wgml_font_cnt ) {
                        ol_layout->font = 0;
                    }
                    AttrFlags.font = true;
                    break;
                case   e_align:
                    if( AttrFlags.align ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &ol_layout->align );
                    AttrFlags.align = true;
                    break;
                case   e_number_style:
                    if( AttrFlags.number_style ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_number_style( p, curr, &ol_layout->number_style );
                    AttrFlags.number_style = true;
                    break;
                case   e_number_font:
                    if( AttrFlags.number_font ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_font_number( p, curr, &ol_layout->number_font );
                    if( ol_layout->number_font >= wgml_font_cnt ) {
                        ol_layout->number_font = 0;
                    }
                    AttrFlags.number_font = true;
                    break;
                default:
                    internal_err( __FILE__, __LINE__ );
                }
                break;                  // break out of for loop
            }
        }
        cc = get_attr_and_value();            // get att with value
    }

    /* Now integrate the OL LAYOUT into the linked list */

    curr_level = layout_work.ol.first;
    while( curr_level->next != NULL ) {
        if( curr_level->level >= ol_layout->level ) {
            break;
        }
        curr_level = curr_level->next;
    }

    if( curr_level->level == ol_layout->level ) {

        /* Only items changed from the level 1 layout are to be copied */

        if( strcmp( ol_layout->left_indent.su_txt,
                layout_work.ol.first->left_indent.su_txt ) != 0 ) {
            curr_level->left_indent = ol_layout->left_indent;
        }
        if( strcmp( ol_layout->right_indent.su_txt,
                layout_work.ol.first->right_indent.su_txt ) != 0 ) {
            curr_level->right_indent = ol_layout->right_indent;
        }
        if( strcmp( ol_layout->pre_skip.su_txt,
                layout_work.ol.first->pre_skip.su_txt ) != 0 ) {
            curr_level->pre_skip = ol_layout->pre_skip;
        }
        if( strcmp( ol_layout->skip.su_txt,
                layout_work.ol.first->skip.su_txt ) != 0 ) {
            curr_level->skip = ol_layout->skip;
        }
        if( strcmp( ol_layout->post_skip.su_txt,
                layout_work.ol.first->post_skip.su_txt ) != 0 ) {
            curr_level->post_skip = ol_layout->post_skip;
        }
        if( strcmp( ol_layout->align.su_txt,
                layout_work.ol.first->align.su_txt ) != 0 ) {
            curr_level->align = ol_layout->align;
        }
        if( ol_layout->spacing != layout_work.ol.first->spacing ) {
            curr_level->spacing = ol_layout->spacing;
        }
        if( ol_layout->font != layout_work.ol.first->font ) {
            curr_level->font = ol_layout->font;
        }
        if( ol_layout->number_style != layout_work.ol.first->number_style ) {
            curr_level->number_style = ol_layout->number_style;
        }
        if( ol_layout->number_font != layout_work.ol.first->number_font ) {
            curr_level->number_font = ol_layout->number_font;
        }
        mem_free( ol_layout );
    } else if( curr_level->next == NULL ) {
        curr_level->next = ol_layout;
        layout_work.ol.max_level = ol_layout->level;
    } else if( curr_level == layout_work.ol.first ) {
        ol_layout->next = layout_work.ol.first;
        layout_work.ol.first = ol_layout;
    } else {
        ol_layout->next = curr_level->next;
        curr_level->next = ol_layout;
    }

    scan_start = scan_stop + 1;
    return;
}

/***************************************************************************/
/*  lay_sl                                                                 */
/***************************************************************************/

void    lay_sl( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    sl_lay_level    *   curr_level  = NULL;
    sl_lay_level    *   sl_layout   = NULL;

    (void)entry;

    p = scan_start;

    if( !WgmlFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_sl ) {
        ProcFlags.lay_xxx = el_sl;
    }

    /* Make a copy of the level 1 SL LAYOUT */

    sl_layout = mem_alloc( sizeof( sl_lay_level ) );
    sl_layout->next = NULL;
    sl_layout->left_indent = layout_work.sl.first->left_indent;
    sl_layout->right_indent = layout_work.sl.first->right_indent;
    sl_layout->pre_skip = layout_work.sl.first->pre_skip;
    sl_layout->skip = layout_work.sl.first->skip;
    sl_layout->post_skip = layout_work.sl.first->post_skip;
    sl_layout->spacing = layout_work.sl.first->spacing;
    sl_layout->font = layout_work.sl.first->font;
    sl_layout->level = layout_work.sl.first->level;

    /* Get the SL LAYOUT settings */

    cc = get_attr_and_value();            // get att with value
    while( cc == pos ) {
        for( k = 0, curr = sl_att[k]; curr > 0; k++, curr = sl_att[k] ) {

            if( strnicmp( att_names[curr], g_att_val.att_name, g_att_val.att_len ) == 0 ) {
                p = g_att_val.val_name;

                switch( curr ) {
                case   e_level:
                    if( AttrFlags.level ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_int8( p, curr, &sl_layout->level );
                    AttrFlags.level = true;
                    break;
                case   e_left_indent:
                    if( AttrFlags.left_indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &sl_layout->left_indent );
                    AttrFlags.left_indent = true;
                    break;
                case   e_right_indent:
                    if( AttrFlags.right_indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &sl_layout->right_indent );
                    AttrFlags.right_indent = true;
                    break;
                case   e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &sl_layout->pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case   e_skip:
                    if( AttrFlags.skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &sl_layout->skip );
                    AttrFlags.skip = true;
                    break;
                case   e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_spacing( p, curr, &sl_layout->spacing );
                    AttrFlags.spacing = true;
                    break;
                case   e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &sl_layout->post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case   e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_font_number( p, curr, &sl_layout->font );
                    if( sl_layout->font >= wgml_font_cnt ) {
                        sl_layout->font = 0;
                    }
                    AttrFlags.font = true;
                    break;
                default:
                    internal_err( __FILE__, __LINE__ );
                }
                break;                  // break out of for loop
            }
        }
        cc = get_attr_and_value();            // get att with value
    }

    /* Now integrate the SL LAYOUT into the linked list */

    curr_level = layout_work.sl.first;
    while( curr_level->next != NULL ) {
        if( curr_level->level >= sl_layout->level ) {
            break;
        }
        curr_level = curr_level->next;
    }

    if( curr_level->level == sl_layout->level ) {

        /* Only items changed from the level 1 layout are to be copied */

        if( strcmp( sl_layout->left_indent.su_txt,
                layout_work.sl.first->left_indent.su_txt ) != 0 ) {
            curr_level->left_indent = sl_layout->left_indent;
        }
        if( strcmp( sl_layout->right_indent.su_txt,
                layout_work.sl.first->right_indent.su_txt ) != 0 ) {
            curr_level->right_indent = sl_layout->right_indent;
        }
        if( strcmp( sl_layout->pre_skip.su_txt,
                layout_work.sl.first->pre_skip.su_txt ) != 0 ) {
            curr_level->pre_skip = sl_layout->pre_skip;
        }
        if( strcmp( sl_layout->skip.su_txt,
                layout_work.sl.first->skip.su_txt ) != 0 ) {
            curr_level->skip = sl_layout->skip;
        }
        if( strcmp( sl_layout->post_skip.su_txt,
                layout_work.sl.first->post_skip.su_txt ) != 0 ) {
            curr_level->post_skip = sl_layout->post_skip;
        }
        if( sl_layout->spacing != layout_work.sl.first->spacing ) {
            curr_level->spacing = sl_layout->spacing;
        }
        if( sl_layout->font != layout_work.sl.first->font ) {
            curr_level->font = sl_layout->font;
        }
        mem_free( sl_layout );
    } else if( curr_level->next == NULL ) {
        curr_level->next = sl_layout;
        layout_work.sl.max_level = sl_layout->level;
    } else if( curr_level == layout_work.sl.first ) {
        sl_layout->next = layout_work.sl.first;
        layout_work.sl.first = sl_layout;
    } else {
        sl_layout->next = curr_level->next;
        curr_level->next = sl_layout;
    }

    scan_start = scan_stop + 1;
    return;
}

/***************************************************************************/
/*  lay_ul                                                                 */
/***************************************************************************/

void    lay_ul( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    ul_lay_level    *   curr_level  = NULL;
    ul_lay_level    *   ul_layout   = NULL;

    (void)entry;

    p = scan_start;

    if( !WgmlFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_ul ) {
        ProcFlags.lay_xxx = el_ul;
    }

    /* Make a copy of the level 1 UL LAYOUT */

    ul_layout = mem_alloc( sizeof( ul_lay_level ) );
    ul_layout->next = NULL;
    ul_layout->left_indent = layout_work.ul.first->left_indent;
    ul_layout->right_indent = layout_work.ul.first->right_indent;
    ul_layout->pre_skip = layout_work.ul.first->pre_skip;
    ul_layout->skip = layout_work.ul.first->skip;
    ul_layout->post_skip = layout_work.ul.first->post_skip;
    ul_layout->align = layout_work.ul.first->align;
    ul_layout->spacing = layout_work.ul.first->spacing;
    ul_layout->font = layout_work.ul.first->font;
    ul_layout->bullet = layout_work.ul.first->bullet;
    ul_layout->bullet_translate = layout_work.ul.first->bullet_translate;
    ul_layout->bullet_font = layout_work.ul.first->bullet_font;
    ul_layout->level = layout_work.ul.first->level;

    /* Get the UL LAYOUT settings */

    cc = get_attr_and_value();            // get att with value
    while( cc == pos ) {
        for( k = 0, curr = ul_att[k]; curr > 0; k++, curr = ul_att[k] ) {

            if( strnicmp( att_names[curr], g_att_val.att_name, g_att_val.att_len ) == 0 ) {
                p = g_att_val.val_name;

                switch( curr ) {
                case   e_level:
                    if( AttrFlags.level ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_int8( p, curr, &ul_layout->level );
                    AttrFlags.level = true;
                    break;
                case   e_left_indent:
                    if( AttrFlags.left_indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &ul_layout->left_indent );
                    AttrFlags.left_indent = true;
                    break;
                case   e_right_indent:
                    if( AttrFlags.right_indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &ul_layout->right_indent );
                    AttrFlags.right_indent = true;
                    break;
                case   e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &ul_layout->pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case   e_skip:
                    if( AttrFlags.skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &ul_layout->skip );
                    AttrFlags.skip = true;
                    break;
                case   e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_spacing( p, curr, &ul_layout->spacing );
                    AttrFlags.spacing = true;
                    break;
                case   e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &ul_layout->post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case   e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_font_number( p, curr, &ul_layout->font );
                    if( ul_layout->font >= wgml_font_cnt ) {
                        ul_layout->font = 0;
                    }
                    AttrFlags.font = true;
                    break;
                case   e_align:
                    if( AttrFlags.align ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_space_unit( p, curr, &ul_layout->align );
                    AttrFlags.align = true;
                    break;
                case   e_bullet:
                    if( AttrFlags.bullet ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_char( p, curr, &ul_layout->bullet );
                    AttrFlags.bullet = true;
                    break;
                case   e_bullet_translate:
                    if( AttrFlags.bullet_translate ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_yes_no( p, curr, &ul_layout->bullet_translate );
                    AttrFlags.bullet_translate = true;
                    break;
                case   e_bullet_font:
                    if( AttrFlags.bullet_font ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    i_font_number( p, curr, &ul_layout->bullet_font );
                    if( ul_layout->bullet_font >= wgml_font_cnt ) {
                        ul_layout->bullet_font = 0;
                    }
                    AttrFlags.bullet_font = true;
                    break;
                default:
                    internal_err( __FILE__, __LINE__ );
                }
                break;                  // break out of for loop
            }
        }
        cc = get_attr_and_value();            // get att with value
    }

    /* Now integrate the UL LAYOUT into the linked list */

    curr_level = layout_work.ul.first;
    while( curr_level->next != NULL ) {
        if( curr_level->level >= ul_layout->level ) {
            break;
        }
        curr_level = curr_level->next;
    }

    if( curr_level->level == ul_layout->level ) {

        /* Only items changed from the level 1 layout are to be copied */

        if( strcmp( ul_layout->left_indent.su_txt,
                layout_work.ul.first->left_indent.su_txt ) != 0 ) {
            curr_level->left_indent = ul_layout->left_indent;
        }
        if( strcmp( ul_layout->right_indent.su_txt,
                layout_work.ul.first->right_indent.su_txt ) != 0 ) {
            curr_level->right_indent = ul_layout->right_indent;
        }
        if( strcmp( ul_layout->pre_skip.su_txt,
                layout_work.ul.first->pre_skip.su_txt ) != 0 ) {
            curr_level->pre_skip = ul_layout->pre_skip;
        }
        if( strcmp( ul_layout->skip.su_txt,
                layout_work.ul.first->skip.su_txt ) != 0 ) {
            curr_level->skip = ul_layout->skip;
        }
        if( strcmp( ul_layout->post_skip.su_txt,
                layout_work.ul.first->post_skip.su_txt ) != 0 ) {
            curr_level->post_skip = ul_layout->post_skip;
        }
        if( strcmp( ul_layout->align.su_txt,
                layout_work.ul.first->align.su_txt ) != 0 ) {
            curr_level->align = ul_layout->align;
        }
        if( ul_layout->spacing != layout_work.ul.first->spacing ) {
            curr_level->spacing = ul_layout->spacing;
        }
        if( ul_layout->font != layout_work.ul.first->font ) {
            curr_level->font = ul_layout->font;
        }
        if( ul_layout->bullet != layout_work.ul.first->bullet ) {
            curr_level->bullet = ul_layout->bullet;
        }
        if( ul_layout->bullet_translate != layout_work.ul.first->bullet_translate ) {
            curr_level->bullet_translate = ul_layout->bullet_translate;
        }
        if( ul_layout->bullet_font != layout_work.ul.first->bullet_font ) {
            curr_level->bullet_font = ul_layout->bullet_font;
        }
        mem_free( ul_layout );
    } else if( curr_level->next == NULL ) {
        curr_level->next = ul_layout;
        layout_work.ul.max_level = ul_layout->level;
    } else if( curr_level == layout_work.ul.first ) {
        ul_layout->next = layout_work.ul.first;
        layout_work.ul.first = ul_layout;
    } else {
        ul_layout->next = curr_level->next;
        curr_level->next = ul_layout;
    }

    scan_start = scan_stop + 1;
    return;
}

