/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  GML :CMT processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  :CMT.                                                                  */
/*                                                                         */
/* The information following the comment tag on the input line is treated  */
/* as a comment. Text data and GML tags following the comment tag are not  */
/* processed. Except between tag attributes, this tag may appear at any    */
/* point in the GML source                                                 */
/***************************************************************************/

extern  void    gml_cmt( const gmltag * entry )
{
    (void)entry;

    scan_start = scan_stop + 1;
    return;
}

