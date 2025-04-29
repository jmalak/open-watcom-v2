/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :HEADING tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :HEADING  attributes                                                  */
/***************************************************************************/
const   lay_att     heading_att[6] =
    { e_delim, e_stop_eject, e_para_indent, e_threshold, e_max_group,
      e_dummy_zero };


/*********************************************************************************/
/*Specify information which applies to headings in general.                      */
/*                                                                               */
/*:HEADING                                                                       */
/*        delim = '.'                                                            */
/*        stop_eject = no                                                        */
/*        para_indent = no                                                       */
/*        threshold = 2                                                          */
/*        max_group = 10                                                         */
/*                                                                               */
/*delim The delim attribute sets the heading number delimiter to a specific      */
/*character.                                                                     */
/*                                                                               */
/*stop_eject This attribute accepts the keyword values yes and no. If the value  */
/*yes is specified, a heading which would force the beginning of a               */
/*new page will not cause a page ejection if it immediately follows              */
/*another heading.                                                               */
/*                                                                               */
/*para_indent This attribute accepts the keyword values yes and no. If the value */
/*no is specified, the indentation of the first line in a paragraph after a      */
/*heading is suppressed.                                                         */
/*                                                                               */
/*threshold This attribute accepts as a value a non-negative integer number.     */
/*The specified value indicates the minimum number of text lines                 */
/*which must fit on the page. The heading will be forced to the next             */
/*page or column if the threshold requirements are not met by the                */
/*following document element. The threshold attribute of the heading             */
/*overrides the default threshold specified by the :widow tag.                   */
/*                                                                               */
/*max_group This attribute accepts a positive integer number. If a group of      */
/*headings are forced to a new page or column because of threshold               */
/*requirements, the specified value will limit the number of headings            */
/*forced as a group.                                                             */
/*********************************************************************************/


/***************************************************************************/
/*  lay_heading                                                            */
/***************************************************************************/

void    lay_heading( const gmltag * entry )
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
    if( ProcFlags.lay_xxx != el_heading ) {
        ProcFlags.lay_xxx = el_heading;
    }

    cc = lay_attr_and_value( &lay_attr );            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = heading_att[k]; curr > 0; k++, curr = heading_att[k] ) {
            if( strcmp( lay_att_names[curr], lay_attr.attname ) == 0 ) {
                p = lay_attr.val.name;
                switch( curr ) {
                case e_delim:
                    if( AttrFlags.delim ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_char( p, &lay_attr, &layout_work.heading.delim );
                    AttrFlags.delim = true;
                    break;
                case e_stop_eject:
                    if( AttrFlags.stop_eject ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_yes_no( p, &lay_attr,
                                           &layout_work.heading.stop_eject );
                    AttrFlags.stop_eject = true;
                    break;
                case e_para_indent:
                    if( AttrFlags.para_indent ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_yes_no( p, &lay_attr,
                                           &layout_work.heading.para_indent );
                    AttrFlags.para_indent = true;
                    break;
                case e_threshold:
                    if( AttrFlags.threshold ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_threshold( p, &lay_attr, &layout_work.heading.threshold );
                    AttrFlags.threshold = true;
                    break;
                case e_max_group:
                    if( AttrFlags.max_group ) {
                        xx_line_err_ci( ERR_ATT_DUP, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_int8( p, &lay_attr, &layout_work.heading.max_group );
                    AttrFlags.max_group = true;
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

