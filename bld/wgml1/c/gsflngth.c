/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'length( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'length(                                       */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'length(string):  The Length function returns  the length of 'string'  */
/*    in characters.   The  'string' may be null,  in which  case zero is  */
/*    returned.                                                            */
/*      &'length(abc) ==> 3                                                */
/*      &'length('Time Flies') ==> 10                                      */
/*      abc&'length(time flies)xyz ==> abc10xyz                            */
/*      +&'length(one)*&'length(two) ==> +3*3                              */
/*      &'length(one,two,three) ==> too many operands                      */
/*                                                                         */
/***************************************************************************/

condcode    scr_length( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int32_t ressize )
{
    tok_type        parm1;
    int             len;

    (void)ressize;

    if( parmcount != 1 ) {
        return( neg );
    }

    parm1 = parms[0].arg;
    scr_unquote_parm( &parm1 );
    len = parm1.e - parm1.s;

    *result += sprintf( *result, "%d", len );

    return( pos );
}
