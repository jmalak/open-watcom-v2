/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :LAYOUT and :eLAYOUT tags
*
****************************************************************************/


#include "wgml.h"


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

    if( !WgmlFlags.firstpass ) {
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

    if( !WgmlFlags.firstpass ) {
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

