/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter functions
*                                                     &'lower( )
*                                                     &'upper( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string functions  &'lower() and &'upper()                       */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'lower(string<,n<,length>>):   The  Lowercase  function  returns  the  */
/*    lowercase equivalent of the 'string' operand.   The first character  */
/*    to be lowercased may be specified  by 'n' and the 'length' defaults  */
/*    to the  end of the string.    The conversion to  lowercase includes  */
/*    only the alphabetic characters.                                      */
/*      &'lower(ABC) ==> abc                                               */
/*      &'lower('Now is the time') ==> now is the time                     */
/*      ABC&'lower(TIME FLIES)890 ==> ABCtime flies890                     */
/*      &'lower(ABC)...&'lower(890) ==> abc...890                          */
/*      &'lower(ABCDEFG,3) ==> ABcdefg                                     */
/*      &'lower(ABCDEFG,3,2) ==> ABcdEFG                                   */
/*      &'lower(ONE,TWO,THREE) ==> invalid operands                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'upper(string<,n<,length>>):   The  Uppercase  function  returns  the  */
/*    uppercase equivalent of the 'string' operand.   The first character  */
/*    to be uppercased may be specified  by 'n' and the 'length' defaults  */
/*    to the end of the string.    The conversion to uppercase is defined  */
/*    by the Translate Uppercase (.TU) control word.                       */
/*      &'upper(abc) ==> ABC                                               */
/*      &'upper('Now is the time') ==> NOW IS THE TIME                     */
/*      abc&'upper(time flies)xyz ==> abcTIME FLIESxyz                     */
/*      &'upper(abc)...&'upper(xyz) ==> ABC...XYZ                          */
/*      &'upper(abcdefg,3) ==> abCDEFG                                     */
/*      &'upper(abcdefg,3,2) ==> abCDefg                                   */
/*      &'upper(one,two,three) ==> invalid operands                        */
/*                                                                         */
/***************************************************************************/

static condcode scr_lowup( parm parms[MAX_FUN_PARMS], size_t parmcount,
                           char * * result, int32_t ressize, bool upper )
{
    char            *   pval;
    char            *   pend;
    condcode            cc;
    int                 k;
    int                 n;
    int                 len;
    getnum_block        gn;

    if( (parmcount < 1) || (parmcount > 3) ) {
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

    if( parmcount > 1 ) {               // evalute start pos
        if( parms[1].a <= parms[1].e ) {// start pos specified
            gn.argstart = parms[1].a;
            gn.argstop  = parms[1].e;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result > len) ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_c( err_func_parm, "2 (startpos)" );
                }
                return( cc );
            }
            n = gn.result - 1;
        }
    }

    if( parmcount > 2 ) {               // evalute length for upper
        if( parms[2].a <= parms[2].e ) {// length specified
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

    for( k = 0; k < n; k++ ) {          // copy unchanged before startpos
        if( (pval > pend) || (ressize <= 0) ) {
            break;
        }
        **result = *pval++;
        *result += 1;
        ressize--;
    }

    for( k = 0; k < len; k++ ) {        // translate
        if( (pval > pend) || (ressize <= 0) ) {
            break;
        }
        if( upper ) {
           **result = my_toupper( *pval++ );
        } else {
           **result = my_tolower( *pval++ );
        }
        *result += 1;
        ressize--;
    }

    for( ; pval <= pend; pval++ ) {     // copy unchanged
        if( ressize <= 0 ) {
            break;
        }
        **result = *pval;
        *result += 1;
        ressize--;
    }

    **result = '\0';

    return( pos );
}


condcode    scr_lower( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    return( scr_lowup( parms, parmcount, result, ressize, 0 ) );
}

condcode    scr_upper( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    return( scr_lowup( parms, parmcount, result, ressize, 1 ) );
}

