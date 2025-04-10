/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'compare( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'compare(                                      */
/*                                                                         */
/***************************************************************************/

/***************************************************************************
 *
 * &'compare(string1,string2<,pad>):  To return the position of the first
 *    character  that does  not match  in 'string1'  and 'string2'.    If
 *    required, the shorter string will be padded on the right with 'pad'
 *    which defaults to a blank.   If the strings are identical then zero
 *    is returned.
 *      &'compare('ABC','ABC') ==> 0
 *      &'compare(ABC,AP) ==> 2
 *      &'compare('AB ','AB') ==> 0
 *      &'compare('AB ','AB',' ') ==> 0
 *      &'compare('AB ','AB','X') ==> 3
 *      &'compare('AB-- ','AB','-') ==> 5
 *
 ***************************************************************************/

condcode    scr_compare( parm parms[MAX_FUN_PARMS], size_t parmcount, char ** result, int32_t ressize )
{
    char            *pend;
    char            *pval1;
    char            *pval2;
    condcode        cc;
    int             i;
    int             index;
    size_t          len1;
    size_t          len2;
    size_t          len;
    char            padchar;

    (void)ressize;

    if( parmcount < 2 || parmcount > 3 ) {
        cc = neg;
        return( cc );
    }

    pval1 = parms[0].a;
    pend = parms[0].e;

    unquote_if_quoted( &pval1, &pend );

    len1 = pend - pval1 + 1;   // param1 length

    pval2 = parms[1].a;
    pend = parms[1].e;

    unquote_if_quoted( &pval2, &pend );

    len2 = pend - pval2 + 1;   // param2 length

    len = len1;
    if( len < len2 )
        len = len2;

    index = 0;
    if( len > 0 ) {
        padchar = ' ';
        if( parmcount > 2 ) {
            char *pval = parms[2].a;

            pend = parms[2].e;
            unquote_if_quoted( &pval, &pend );
            padchar = *pval;
        }
        for( i = 0; i < len; i++ ) {
            char    c1;
            char    c2;

            if( i < len1 ) {
                c1 = pval1[i];
            } else {
                c1 = padchar;
            }
            if( i < len2 ) {
                c2 = pval2[i];
            } else {
                c2 = padchar;
            }
            if( c1 != c2 ) {
                index = i + 1;
                break;
            }
        }
    }
    *result += sprintf( *result, "%d", index );
    return( pos );
}

