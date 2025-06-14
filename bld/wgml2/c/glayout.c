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
* Description: WGML implement :LAYOUT and :eLAYOUT tags
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*                                                                         */
/*  Format: :LAYOUT.                                                       */
/*                                                                         */
/* This tag starts a layout section. The layout tag is a special WATCOM    */
/* Script/GML tag used to modify the default layout of the output document.*/
/* More than one layout section may be present, but all layout sections    */
/* must appear before the :gdoc tag. The :elayout tag terminates a layout  */
/* section.                                                                */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/*  Format: :eLAYOUT.                                                      */
/*                                                                         */
/* This tag signals the end of a layout section. A corresponding :layout   */
/* tag must be previously specified for each :elayout tag.                 */
/***************************************************************************/

/***************************************************************************/
/*  gml_layout                                                             */
/***************************************************************************/

void    gml_layout( const gmltag * entry )
{
    char        *   p;

    p = scan_start;
    scan_start = scan_stop + 1;

    if( !GlobalFlags.firstpass ) {
        ProcFlags.layout = true;

        /*******************************************************************/
        /*  read and ignore all lines up to :eLAYOUT                       */
        /*******************************************************************/

        while( !ProcFlags.reprocess_line  ) {
            eat_lay_sub_tag();
            if( strnicmp( ":elayout", buff2, 8 ) ) {
                ProcFlags.reprocess_line = false;   // not :elayout, go on
            }
        }
        return;
    }

    if( !ProcFlags.lay_specified ) {
        ProcFlags.lay_specified = true;
        g_info_lm( inf_proc_lay );
    }

    if( *p == '\0' || *p == '.' ) {
        if( ProcFlags.layout ) {        // nested layout
            xx_err_c( err_nested_tag, entry->tagname );
        }
        ProcFlags.layout = true;
        return;
    } else {
        xx_err_cc( err_extra_ignored, tok_start, p );
    }
    return;
}


/***************************************************************************/
/*  lay_elayout     end of layout processing                               */
/***************************************************************************/

void    lay_elayout( const gmltag * entry )
{
    char        *   p;

    p = scan_start;
    scan_start = scan_stop + 1;

    if( !GlobalFlags.firstpass ) {
        ProcFlags.layout = false;
        return;                         // process during first pass only
    }

    if( *p == '\0' || *p == '.' ) {
        if( !ProcFlags.layout ) {       // not in layout processing
            xx_err_cc( err_no_lay, &(entry->tagname[1]), entry->tagname );
        }
        ProcFlags.layout = false;
        ProcFlags.lay_xxx = el_zero;

        return;
    } else {
        xx_err_cc( err_extra_ignored, tok_start, p );
    }
    return;
}

