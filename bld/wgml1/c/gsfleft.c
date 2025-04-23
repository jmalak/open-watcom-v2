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

condcode    scr_left( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int32_t ressize )
{
    tok_type        parm1;
    condcode        cc;
    int             k;
    int             len;
    getnum_block    gn;

    if( parmcount != 2 ) {
        cc = neg;
        return( cc );
    }

    parm1 = parms[0].arg;
    scr_unquote_parm( &parm1 );
    len = parm1.e - parm1.s + 1;        // default length

    if( len <= 0 ) {                    // null string nothing to do
        **result = '\0';
        return( pos );
    }

    if( parms[1].arg.s <= parms[1].arg.e ) {// length specified
        gn.arg = parms[1].arg;
        gn.arg.e++;
        cc = getnum( &gn );
        if( cc != pos ) {
            if( !ProcFlags.suppress_msg ) {
                xx_source_err_c( err_func_parm, "2 (length)" );
            }
            return( cc );
        }
        len = gn.result;
    }

    for( k = 0; k < len && (parm1.s <= parm1.e) && (ressize > 0); k++ ) {   // copy from start
        **result = *parm1.s++;
        *result += 1;
        ressize--;
    }

    for( ; k < len && (ressize > 0); k++ ) {             // pad to length
        **result = ' ';
        *result += 1;
        ressize--;
    }

    **result = '\0';

    return( pos );
}

