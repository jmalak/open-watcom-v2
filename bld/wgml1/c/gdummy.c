/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: implement dummy support for not (yet) implemented
*              control words and tags
*                    gml_dummy()
*                    lay_dummy()
*                    scr_dummy()
*   NOTE:   output modified to list only the item name, not the context
*
****************************************************************************/


#include "wgml.h"

/***************************************************************************/
/*  scr_dummy        processing                                            */
/***************************************************************************/

void    scr_dummy( void )
{
    char            cwcurr[4];          // control word string for msg

    cwcurr[0] = SCR_char;
    cwcurr[1] = *(token_buf);
    cwcurr[2] = *(token_buf + 1);
    cwcurr[3] = '\0';

    scan_restart = scan_stop + 1;

    xx_warn_c( wng_unsupp_cw, cwcurr );
}


/***************************************************************************/
/*  gml_dummy        processing                                            */
/***************************************************************************/

void    gml_dummy( const gmltag * entry )
{

    scan_start = scan_stop + 1;

    xx_warn_c( wng_unsupp_tag, entry->tagname );
}

/***************************************************************************/
/*  lay_dummy        processing                                            */
/***************************************************************************/

void    lay_dummy( const gmltag * entry )
{

    scan_start = scan_stop + 1;

    if( WgmlFlags.firstpass ) {       // layout msg only in pass 1
        xx_warn_c( wng_unsupp_lay, entry->tagname );
    }
    eat_lay_sub_tag();                  // ignore any attribute / value
}


