/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Implementation of control word CM
*
****************************************************************************/


#include "wgml.h"

/***************************************************************************/
/*  scr_cm processing                                                      */
/***************************************************************************/

void    scr_cm( void )
{
    // simply do nothing
    scan_restart = scan_stop + 1;
}

