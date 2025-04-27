/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'index( ) and &'pos( )
*               They are only different in the parameter order:
*                   &'index( haystack, needle,   ... )
*                   &'pos  ( needle,   haystack, ... )
*               and &'lastpos()
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*                                                                         */
/* &'index(haystack,needle<,start>):   The  Index  function  returns  the  */
/*    character position of the string 'needle' in the string 'haystack'.  */
/*    If not found, the function returns zero.  The first character posi-  */
/*    tion to be searched in 'haystack' may be specified with the 'start'  */
/*    number and  it defaults to the  first character position  in 'hays-  */
/*    tack'.                                                               */
/*      &'index('abcdef','c') ==> 3                                        */
/*      &'index('abcdef','cd') ==> 3                                       */
/*      &'index('abcdef','yz') ==> 0                                       */
/*      &'index('ab','abcdef') ==> 0                                       */
/*      &'index('ababab','ab',2) ==> 3                                     */
/*      &'index('ababab','ab',6) ==> 0                                     */
/*                                                                         */
/***************************************************************************/
/***************************************************************************/
/*                                                                         */
/* &'pos(needle,haystack<,start>):   The  Position function  returns  the  */
/*    character position  of the first  occurrence of 'needle'  in 'hays-  */
/*    tack'.   The  search for a  match starts  at the first  position of  */
/*    'haystack' but  may be  overridden by  adding a  'start' column  in  */
/*    'haystack'.    If the  'needle' string  is  not found,   a zero  is  */
/*    returned.                                                            */
/*      &'pos('c','abcde') ==> 3                                           */
/*      &'pos(x,abcde) ==> 0                                               */
/*      &'pos(abcde,abcde) ==> 1                                           */
/*      &'pos(a,aaaaa) ==> 1                                               */
/*      &'pos('a','aaaaa') ==> 1                                           */
/*      &'pos(a,aaaaa,3) ==> 3                                             */
/*      &'pos(12345678,abc) ==> 0                                          */
/*      &'pos(a) ==> error, too few operands                               */
/*      &'pos(a,abcd,junk) ==> error, 'start' not numeric                  */
/*      &'pos(a,abcd,3,'.') ==> error, too many operands                   */
/*                                                                         */
/***************************************************************************/
/***************************************************************************/
/*                                                                         */
/* &'lastpos(needle,haystack<,start>):    The  Last   Position   function  */
/*    returns the starting  character of the last  occurrence of 'needle'  */
/*    in 'haystack'.  The first position to search from in 'haystack' may  */
/*    be specified  with 'start' and  this defaults  to the start  of the  */
/*    'haystack' string.  If no match for 'needle' can be found in 'hays-  */
/*    tack' then zero is returned.                                         */
/*      &'lastpos('c','abcde') ==> 3                                       */
/*      &'lastpos(x,abcde) ==> 0                                           */
/*      &'lastpos(abcde,abcde) ==> 1                                       */
/*      &'lastpos(a,aaaaa) ==> 5                                           */
/*      &'lastpos('a','aaaaa') ==> 5                                       */
/*      &'lastpos(a,aaaaa,3) ==> 5                                         */
/*      &'lastpos(a,aaaaa,10) ==> 0                                        */
/*      &'lastpos(a) ==> error, too few operands                           */
/*                                                                         */
/***************************************************************************/

condcode    scr_index( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int ressize )
{
    tok_type        pneedle;
    tok_type        phay;
    condcode        cc;
    int             index;
    int             n;
    int             hay_len;
    int             needle_len;
    getnum_block    gn;
    char            *ph;
    char            *pn;

    (void)ressize;

    if( (parmcount < 2) || (parmcount > 3) ) {
        cc = neg;
        return( cc );
    }

    phay = parms[0].arg;
    scr_unquote_parm( &phay );
    hay_len = phay.e - phay.s;          // haystack length

    pneedle = parms[1].arg;
    scr_unquote_parm( &pneedle );
    needle_len = pneedle.e - pneedle.s; // needle length

    n = 0;                              // default start pos
    gn.ignore_blanks = false;

    if( parmcount > 2 ) {               // evalute start pos
        if( parms[2].arg.s < parms[2].arg.e ) {// start pos specified
            gn.arg = parms[2].arg;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_c( ERR_FUNC_PARM, "3 (startpos)" );
                }
                return( cc );
            }
            n = gn.result - 1;
        }
    }

    if( (hay_len == 0) ||               // null string nothing to do
        (needle_len == 0) ||            // needle null nothing to do
        (needle_len > hay_len) ||       // needle longer haystack
        (n + needle_len > hay_len) ) {  // startpos + needlelen > haystack
                                        // ... match impossible

        **result = '0';                 // return index zero
        *result += 1;
        **result = '\0';
        return( pos );
    }

    ph = phay.s + n;                    // startpos in haystack
    pn = pneedle.s;
    index = 0;

    for( ph = phay.s + n; ph < phay.e - needle_len + 1; ph++ ) {
        pn = pneedle.s;
        while( (*ph == *pn) && (pn < pneedle.e)) {
            ph++;
            pn++;
        }
        if( pn >= pneedle.e ) {
            index = ph - phay.s - needle_len + 1; // found, set index
            break;
        }
    }

    *result += sprintf( *result, "%d", index );

    return( pos );
}

/*
 * scr_pos : swap parm1 and parm2, then call scr_index
 *
 */

condcode    scr_pos( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int ressize )
{
    char        *pwk;

    if( parmcount < 2 ) {
        return( neg );
    }

    pwk = parms[0].arg.s;
    parms[0].arg.s = parms[1].arg.s;
    parms[1].arg.s = pwk;

    pwk = parms[0].arg.e;
    parms[0].arg.e = parms[1].arg.e;
    parms[1].arg.e = pwk;

    return( scr_index( parms, parmcount, result, ressize ) );
}

/***************************************************************************/
/*  lastpos                                                                */
/***************************************************************************/

condcode    scr_lpos( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int ressize )
{
    tok_type        pneedle;
    tok_type        phay;
    condcode        cc;
    int             index;
    int             n;
    int             hay_len;
    int             needle_len;
    getnum_block    gn;
    char            *ph;
    char            *pn;

    (void)ressize;

    if( (parmcount < 2) || (parmcount > 3) ) {
        cc = neg;
        return( cc );
    }

    pneedle = parms[0].arg;
    scr_unquote_parm( &pneedle );
    needle_len = pneedle.e - pneedle.s; // needle length

    phay = parms[1].arg;
    scr_unquote_parm( &phay );
    hay_len = phay.e - phay.s;          // haystack length

    n = 0;                              // default start pos
    gn.ignore_blanks = false;

    if( parmcount > 2 ) {               // evalute start pos
        if( parms[2].arg.s != parms[2].arg.e ) {// start pos specified
            gn.arg = parms[2].arg;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_c( ERR_FUNC_PARM, "3 (startpos)" );
                }
                return( cc );
            }
            n = gn.result - 1;
        }
    }

    if( (hay_len == 0)                  // null string nothing to do
      || (needle_len == 0)              // needle null nothing to do
      || (needle_len > hay_len)         // needle longer haystack
      || (n + needle_len > hay_len) ) { // startpos + needlelen > haystack
                                        // ... match impossible

        **result = '0';                 // return index zero
        *result += 1;
        **result = '\0';
        return( pos );
    }

    ph = phay.s + n;                    // startpos in haystack
    pn = pneedle.s;
    index = 0;

    for( ph = phay.e - 1; ph >= phay.s + n ; ph-- ) {
        pn = pneedle.e - 1;
        while( (*ph == *pn) && (pn >= pneedle.s) ) {
            ph--;
            pn--;
        }
        if( pn < pneedle.s ) {
            index = ph - phay.s + 2;          // found, set index
            break;
        }
    }

    *result += sprintf( *result, "%d", index );

    return( pos );
}
