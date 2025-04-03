/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'right( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'right(                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'right(string,length<,pad>):   To  generate  a  character  string  of  */
/*    'length'  characters with  'string' at  the end.    If 'length'  is  */
/*    longer than 'string' then 'pad' the  result,  if less then truncate  */
/*    on the left.                                                         */
/*      &'right('ABC D',8) ==> "   ABC D"                                  */
/*      &'right('ABC D',8,'.') ==> "...ABC D"                              */
/*      &'right('ABC  DEF',7) ==> "BC  DEF"                                */
/*                                                                         */
/***************************************************************************/

condcode    scr_right( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    condcode            cc;
    int                 k;
    int                 n;
    int                 len;
    getnum_block        gn;
    char                padchar;

    if( (parmcount < 2) || (parmcount > 3) ) {
        cc = neg;
        return( cc );
    }

    pval = parms[0].a;
    pend = parms[0].e;

    unquote_if_quoted( &pval, &pend );

    len = pend - pval + 1;              // total length

    gn.ignore_blanks = false;

    gn.argstart = parms[1].a;
    gn.argstop  = parms[1].e;
    cc = getnum( &gn );
    if( cc != pos ) {
        if( !ProcFlags.suppress_msg ) {
            xx_source_err_c( err_func_parm, "2 (length)" );
        }
        return( cc );
    }
    n = gn.result;

    if( n > 0 ) {                       // result not nullstring
        if( n > len ) {                 // padding needed
            padchar = ' ';              // default padchar
            if( parmcount > 2 ) {       // pad character specified
                if( parms[2].e >= parms[2].a ) {
                    char * pa = parms[2].a;
                    char * pe = parms[2].e;

                    unquote_if_quoted( &pa, &pe);
                    padchar = *pa;
                }
            }
            for( k = n - len; k > 0; k-- ) {
                if( ressize <= 0 ) {
                    break;
                }
                **result = padchar;
                *result += 1;
                ressize--;
            }
            for( ; pval <= pend; pval++ ) {
                if( ressize <= 0 ) {
                    break;
                }
                **result = *pval;
                *result += 1;
                ressize--;
            }
        } else {                        // no padding

            pval += len - n;
            for( ; pval <= pend; pval++ ) {
                if( ressize <= 0 ) {
                    break;
                }
                **result = *pval;
                *result += 1;
                ressize--;
            }
        }
    }
    **result = '\0';

    return( pos );
}
