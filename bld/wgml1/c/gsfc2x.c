/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'c2x( )
****************************************************************************/


#include "wgml.h"


static unsigned char hex( unsigned char c )
{
    static const unsigned char htab[] = "0123456789ABCDEF";

    if( c < 16 ) {
        return( htab[c] );
    } else {
        return( '0' );
    }
}
/***************************************************************************/
/*  script string function &'c2x()                                         */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/* &'c2x(string):  To convert  a 'string' to its  hexadecimal representa-  */
/*    tion.                                                                */
/*                                                                         */
/* !    examples are all in EBCDIC,       ASCII shown below                */
/*      "&'c2x(abc)" ==> "818283"         "616263"                         */
/*      "&'c2x('1 A')" ==> "F140C1"       "312041"                         */
/*      "&'c2x('')" ==> ""                ""                               */
/*      "&'c2x('X',2)" ==> too many operands                               */
/*                                                                         */
/***************************************************************************/

condcode    scr_c2x( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;

    if( parmcount != 1 ) {              // only 1 parm valid
        return( neg );
    }

    pval = parms[0].a;
    pend = parms[0].e;

    unquote_if_quoted( &pval, &pend );

    while( (pval <= pend) && (ressize > 1) ) {
        **result = hex( (unsigned)*pval >> 4 );
        *result += 1;
        **result = hex( (unsigned)*pval & 0x0f );
        *result += 1;
        **result = 0;
        ressize -= 2;
        pval++;
    }
    return( pos );
}
