/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'insert( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'insert(                                       */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'insert(new,target<,<n><,<length><,pad>>>):   To  Insert   the  'new'  */
/*    string with  length 'length' into  the 'target' string  after char-  */
/*    acter 'n'.   If 'n' is omitted then  'new' is inserted at the start  */
/*    of 'target'.   The 'pad' character may  be used to extend the 'tar-  */
/*    get' string to length 'n' or the 'new' string to length 'length'.    */
/*      "&'insert(' ','abcdef',3)" ==> "abc def"                           */
/*      "&'insert('123','abc',5,6)" ==> "abc  123   "                      */
/*      "&'insert('123','abc',5,6,'+')" ==> "abc++123+++"                  */
/*      "&'insert('123','abc')" ==> "123abc"                               */
/*      "&'insert('123','abc',5,,'-')" ==> "abc--123"                      */
/*      "&'insert('123','abc',,,'-')" ==> "123abc"                         */
/*                                                                         */
/* ! optional parms LENGTH and PAD are NOT implemented                     */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

condcode    scr_insert( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int32_t ressize )
{
    tok_type        parm1;
    tok_type        parm2;
    condcode        cc;
    int             k;
    int             n;
    int             len;
    getnum_block    gn;

    if( (parmcount < 2) || (parmcount > 3) ) {
        cc = neg;
        return( cc );
    }

    parm1 = parms[0].arg;           // string to insert
    unquote_if_quoted( &parm1 );
    len = parm1.e - parm1.s + 1;    // length to insert

    parm2 = parms[1].arg;           // string to be modified
    unquote_if_quoted( &parm2 );

    if( len <= 0 ) {                // null string insert nothing to do
        **result = '\0';
        return( pos );
    }

    n = 0;                          // default start pos
    gn.ignore_blanks = false;
    if( parmcount > 2 ) {           // evalute startpos
        if( parms[2].arg.s <= parms[2].arg.e ) {
            gn.arg = parms[2].arg;
            gn.arg.e++;
            cc = getnum( &gn );
            if( cc != pos ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_c( err_func_parm, "3 (startpos)" );
                }
                return( cc );
            }
            n = gn.result;
        }
    }

    k = 0;
    while( (k < n) && (parm2.s <= parm2.e) && (ressize > 0) ) { // copy up to startpos
        k++;
        **result = *parm2.s++;
        *result += 1;
        ressize--;
    }
    if( (n > k) && (ressize > 0) ) {                // startpos > target length, insert one extra blank
        **result = ' ';
        *result += 1;
        ressize--;
    }

    while( (parm1.s <= parm1.e) && (ressize > 0) ) { // insert new string
        **result = *parm1.s++;
        *result += 1;
        ressize--;
    }

    while( (parm2.s <= parm2.e) && (ressize > 0) ) { // copy rest (if any)
        **result = *parm2.s++;
        *result += 1;
        ressize--;
    }

    **result = '\0';

    return( pos );
}

