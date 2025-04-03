/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'left( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'left(                                         */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'left(string,length<,pad>):   To  generate  a   character  string  of  */
/*    'length' characters  with 'string' in  its start.   If  'length' is  */
/*    longer than 'string' then 'pad' the result.                          */
/*      &'left('ABC D',8) ==> "ABC D   "                                   */
/*      &'left('ABC D',8,'.') ==> "ABC D..."                               */
/*      &'left('ABC  DEF',7) ==> "ABC  DE"                                 */
/*                                                                         */
/* ! optional parm PAD is NOT implemented                                  */
/*                                                                         */
/***************************************************************************/

condcode    scr_left( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    condcode            cc;
    int                 k;
    int                 len;
    getnum_block        gn;

    if( parmcount != 2 ) {
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

    if( parms[1].e >= parms[1].a ) {// length specified
        gn.argstart = parms[1].a;
        gn.argstop  = parms[1].e;
        cc = getnum( &gn );
        if( cc != pos ) {
            if( !ProcFlags.suppress_msg ) {
                xx_source_err_c( err_func_parm, "2 (length)" );
            }
            return( cc );
        }
        len = gn.result;
    }

    for( k = 0; k < len; k++ ) {        // copy from start
        if( (pval > pend) || (ressize <= 0) ) {
            break;
        }
        **result = *pval++;
        *result += 1;
        ressize--;
    }

    for( ; k < len; k++ ) {             // pad to length
        if( ressize <= 0 ) {
            break;
        }
        **result = ' ';
        *result += 1;
        ressize--;
    }

    **result = '\0';

    return( pos );
}

