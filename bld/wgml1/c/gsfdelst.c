/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'delstr( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'delstr(                                       */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/* &'delstr(string,n<,length>):  The  Delete String function  deletes the  */
/*    part of 'string' starting at character  number 'n'.   The number of  */
/*    characters can be specified in 'length' and defaults from character  */
/*    'n' to the end of the string.  If 'string' is less than 'n' charac-  */
/*    ters long, then nothing is deleted.                                  */
/*      &'delstr('abcdef',3) ==> ab                                        */
/*      &'delstr('abcdef',3,2) ==> abef                                    */
/*      &'delstr('abcdef',10) ==> abcdef                                   */
/***************************************************************************/

condcode    scr_delstr( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    condcode            cc;
    int                 k;
    int                 n;
    int                 len;
    getnum_block        gn;

    if( (parmcount < 2) || (parmcount > 3) ) {
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

    n   = 0;                            // default start pos
    gn.ignore_blanks = false;

    if( parms[1].e >= parms[1].a ) {// start pos
        gn.argstart = parms[1].a;
        gn.argstop  = parms[1].e;
        cc = getnum( &gn );
        if( (cc != pos) || (gn.result == 0) ) {
            if( !ProcFlags.suppress_msg ) {
                xx_source_err_c( err_func_parm, "2 (startpos)" );
            }
            return( cc );
        }
        n = gn.result - 1;
    }

    if( parmcount > 2 ) {               // evalute length
        if( parms[2].e >= parms[2].a ) {// length specified
            gn.argstart = parms[2].a;
            gn.argstop  = parms[2].e;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_c( err_func_parm, "3 (length)" );
                }
                return( cc );
            }
            len = gn.result;
        }
    }

    k = 0;
    while( (k < n) && (pval <= pend) && (ressize > 0) ) {// copy unchanged before startpos
        **result = *pval++;
        *result += 1;
        k++;
        ressize--;
    }

    k = 0;
    while( (k < len) && (pval <= pend) ) {  // delete
        pval++;
        k++;
    }

    while( (pval <= pend) && (ressize > 0) ) {// copy unchanged
        **result = *pval++;
        *result += 1;
        ressize--;
    }

    **result = '\0';

    return( pos );
}
