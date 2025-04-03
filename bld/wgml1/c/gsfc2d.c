/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'c2d( )
*                    2. parm not implemented
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'c2d()                                         */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/* &'c2d(string<,n>):  To  convert the binary  representation of  a char-  */
/*    acter 'string'  to a decimal number.    The 'string' is  treated as  */
/*    signed when the length 'n' is included.                              */
/* ! examples are all in EBCDIC, ASCII shown below                         */
/*      &'c2d($) ==> 91             36                                     */
/*      &'c2d(a) ==> 129            97                                     */
/*      &'c2d(AA) ==> 49601      16705                                     */
/*      &'c2d('a',1) ==> -127       97 2. parm ignored                     */
/*      &'c2d('a',2) ==> 129        97 2. parm ignored                     */
/*      &'c2d('0a',2) ==> -3967     97 2. parm ignored                     */
/*      &'c2d('0a',1) ==> -127      97 2. parm ignored                     */
/*      &'c2d('X',0) ==> 0          88 2. parm ignored                     */
/*                                                                         */
/* ! 2. parameter not implemented                                          */
/*                                                                         */
/***************************************************************************/

condcode    scr_c2d( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    condcode            cc;
    uint32_t            n;
    char                linestr[MAX_L_AS_STR];
    char            *   p;

    if( (parmcount < 1) || (parmcount > 2) ) {// accept 2. parm, but ignore it
        cc = neg;
        return( cc );
    }

    pval = parms[0].a;
    pend = parms[0].e;

    unquote_if_quoted( &pval, &pend );


    n = 0;
    while( pval <= pend ) {
        n *= 256;                      // ignore overflow, let it wrap around
        n += (unsigned char)*pval;
        pval++;
    }
    ulongtodec( n, linestr );
    p = linestr;
    while( *p != '\0' && ressize > 0) {
        **result = *p++;
        *result += 1;
        ressize--;
    }
    return( pos );
}
