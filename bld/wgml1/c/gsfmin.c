/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'min( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'min(                                          */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'min(number<,number ...>):  The Minimum function returns the smallest  */
/*    in a list of numbers.                                                */
/*      &'min(5,6,7,8) ==> 5                                               */
/*      &'min(-5,(5*5),0) ==> -5                                           */
/*      &'min('5','6','7') ==> 5                                           */
/*      &'min(1,&'min(2,&'min(3,4))) ==> 1                                 */
/*                                                                         */
/*  ! Maximum number count is 6                                            */
/*                                                                         */
/***************************************************************************/


condcode    scr_min( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int32_t ressize )
{
    tok_type        parmx;
    condcode        cc;
    int             k;
    int             len;
    getnum_block    gn;
    int             minimum;

    (void)ressize;

    if( (parmcount < 2) || (parmcount > 6) ) {
        cc = neg;
        return( cc );
    }

    minimum = INT_MAX;

    gn.ignore_blanks = false;

    for( k = 0; k < parmcount; k++ ) {
        parmx = parms[k].arg;
        scr_unquote_parm( &parmx );
        len = parmx.e - parmx.s;        // length

        if( len == 0 ) {                // null string nothing to do
            continue;                   // skip empty value
        }
        gn.arg = parmx;
        cc = getnum( &gn );
        if( !(cc == pos  || cc == neg) ) {
            if( !ProcFlags.suppress_msg ) {
                xx_source_err_c( ERR_FUNC_PARM, "" );
            }
            return( cc );
        }
        if( gn.result < minimum ) {
            minimum = gn.result;        // new minimum
        }
    }

    *result += sprintf( *result, "%d", minimum );

    return( pos );
}
