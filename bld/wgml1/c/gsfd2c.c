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


condcode    scr_d2c( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int ressize )
{
    tok_type        parm1;
    condcode        cc;
    int             n;
    getnum_block    gn;

    (void)ressize;

    if( parmcount != 1 ) {
        cc = neg;
        return( cc );
    }

    parm1 = parms[0].arg;
    scr_unquote_parm( &parm1 );

    if( parm1.s == parm1.e ) {      // null string nothing to do
        **result = '\0';
        return( pos );
    }

    n   = 0;
    gn.ignore_blanks = false;

    if( parms[1].arg.s != parms[1].arg.e ) {
        gn.arg = parm1;
        cc = getnum( &gn );
        if( (cc != pos) ) {
            if( !ProcFlags.suppress_msg ) {
                xx_source_err_c( ERR_FUNC_PARM, "1 (number)" );
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
