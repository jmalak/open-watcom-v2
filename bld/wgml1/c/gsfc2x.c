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

condcode    scr_c2x( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int ressize )
{
    tok_type        parm1;

    if( parmcount != 1 ) {              // only 1 parm valid
        return( neg );
    }

    parm1 = parms[0].arg;
    scr_unquote_parm( &parm1 );

    while( (parm1.s < parm1.e) && (ressize > 1) ) {
        unsigned char c = *parm1.s++;
        **result = hex( c >> 4 );
        *result += 1;
        **result = hex( c & 0x0f );
        *result += 1;
        **result = 0;
        ressize -= 2;
    }
    **result = '\0';
    return( pos );
}
