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

condcode    scr_right( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int32_t ressize )
{
    tok_type        parm1;
    condcode        cc;
    int             k;
    int             n;
    int             len;
    getnum_block    gn;
    char            padchar;

    if( (parmcount < 2) || (parmcount > 3) ) {
        cc = neg;
        return( cc );
    }

    parm1 = parms[0].arg;
    unquote_if_quoted( &parm1 );
    len = parm1.e - parm1.s + 1;              // total length

    gn.ignore_blanks = false;
    gn.arg = parms[1].arg;
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
                if( parms[2].arg.s <= parms[2].arg.e ) {
                    tok_type parm = parms[2].arg;
                    unquote_if_quoted( &parm);
                    if( parm.s <= parm.e ) {
                        padchar = *parm.s;
                    }
                }
            }
            for( k = n - len; k > 0 && ressize > 0; k-- ) {
                **result = padchar;
                *result += 1;
                ressize--;
            }
            while( parm1.s <= parm1.e && ressize > 0 ) {
                **result = *parm1.s++;
                *result += 1;
                ressize--;
            }
        } else {                        // no padding
            parm1.s += len - n;
            while( parm1.s <= parm1.e && ressize > 0 ) {
                **result = *parm1.s++;
                *result += 1;
                ressize--;
            }
        }
    }
    **result = '\0';

    return( pos );
}
