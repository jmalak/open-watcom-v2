/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'d2c( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'d2c(                                          */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/* &'d2c(number<,n>):  To  convert a  decimal 'number'  to its  character  */
/*    representation of length 'n'.  The 'number' can be negative only if  */
/*    the length 'n' is specified.                                         */
/*      "&'d2c(129)" ==> "a"          The examples are in EBCDIC           */
/*      "&'d2c(129,1)" ==> "a"                                             */
/*      "&'d2c(129,2)" ==> " a"                                            */
/*      "&'d2c(-127,1)" ==> "a"                                            */
/*      "&'d2c(-127,2)" ==> "ÿa"                                           */
/*      "&'d2c(12,0)" ==> ""                                               */
/*                                                                         */
/*  ! The optional second parm is NOT implemented                          */
/*                                                                         */
/***************************************************************************/


condcode    scr_d2c( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    condcode            cc;
    int                 n;
    int                 len;
    getnum_block        gn;

    (void)ressize;

    if( parmcount != 1 ) {
        cc = neg;
        return( cc );
    }

    pval = parms[0].a;
    pend = parms[0].e;

    unquote_if_quoted( &pval, &pend );

    len = pend - pval + 1;              // default length

    if( len <= 0 ) {                    // null string nothing to do
        **result = '\0';
        return( pos );
    }

    n   = 0;
    gn.ignore_blanks = false;

    if( parms[1].e >= parms[1].a ) {
        gn.argstart = pval;
        gn.argstop  = pend;
        cc = getnum( &gn );
        if( (cc != pos) ) {
            if( !ProcFlags.suppress_msg ) {
                xx_source_err_c( err_func_parm, "1 (number)" );
            }
            return( cc );
        }
        n = gn.result;
    }

    **result = gn.result;
    *result += 1;
    **result = '\0';

    return( pos );
}
