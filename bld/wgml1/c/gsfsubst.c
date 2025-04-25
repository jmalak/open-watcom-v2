/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'substr( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'substr(                                       */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'substr(string,n<,length<,pad>>):  The Substring function returns the  */
/*    portion of 'string' starting at  character number 'n'.   The number  */
/*    of  characters  to return  may  be  specified  in 'length'  and  it  */
/*    defaults from  character 'n' to the  end of the 'string'.    If the  */
/*    'length' is  present and it  would exceed  the total length  of the  */
/*    'string' then  string is  extended with  blanks or  the user  'pad'  */
/*    character.                                                           */
/*      "&'substr('123456789*',5)" ==> "56789*"                            */
/*      "&'substr('123456789*',5,9)" ==> "56789*   "                       */
/*      "&'substr('123456789*',5,9,'.')" ==> "56789*..."                   */
/*      "&'substr('123456789*',1,3,':')" ==> "123"                         */
/*      .se alpha = 'abcdefghijklmnopqrstuvwxyz'                           */
/*      "&'substr(&alpha,24)" ==> "xyz"                                    */
/*      "&'substr(&alpha,24,1)" ==> "x"                                    */
/*      "&'substr(&alpha,24,5)" ==> "xyz  "                                */
/*      "&'substr(&alpha,24,5,':')" ==> "xyz::"                            */
/*      "&'substr(&alpha,30,5,':')" ==> ":::::"                            */
/*      "&'substr(abcde,0,5)" ==> start column too small                   */
/*      "&'substr(abcde,1,-1)" ==> length too small                        */
/*                                                                         */
/***************************************************************************/

condcode    scr_substr( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int32_t ressize )
{
    tok_type        parm1;
    condcode        cc;
    int             k;
    int             n;
    int             stringlen;
    int             len;
    getnum_block    gn;
    char            padchar;

    if( (parmcount < 2) || (parmcount > 4) ) {
        return( neg );
    }

    parm1 = parms[0].arg;
    scr_unquote_parm( &parm1 );
    stringlen = parm1.e - parm1.s;      // length of string
    padchar = ' ';                      // default padchar
    len = 0;

    n = 0;                              // default start pos
    gn.ignore_blanks = false;

    if( parmcount > 1 ) {               // evalute start pos
        if( parms[1].arg.s != parms[1].arg.e ) {// start pos specified
            gn.arg = parms[1].arg;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_c( ERR_FUNC_PARM, "2 (startpos)" );
                }
                return( cc );
            }
            n = gn.result - 1;
        }
    }

    if( parmcount > 2 ) {               // evalute length
        if( parms[2].arg.s != parms[2].arg.e ) {// length specified
            gn.arg = parms[2].arg;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_c( ERR_FUNC_PARM, "3 (length)" );
                }
                return( cc );
            }
            len = gn.result;
        }
    }

    if( parmcount > 3 ) {               // isolate padchar
        if( parms[3].arg.s != parms[3].arg.e ) {
            tok_type parm = parms[3].arg;
            scr_unquote_parm( &parm );
            if( parm.s != parm.e ) {
                padchar = *parm.s;
            }
        }
    }

    parm1.s += n;                       // position to startpos
    if( len == 0 ) {                    // no length specified
        len = parm1.e - parm1.s;        // take rest of string
    }
    for( k = 0; k < len && (parm1.s < parm1.e) && (ressize > 0); k++ ) {
        **result = *parm1.s++;
        *result += 1;
        ressize--;
    }
    for( ; k < len && (ressize > 0); k++ ) {
        **result = padchar;
        *result += 1;
        ressize--;
    }

    **result = '\0';

    return( pos );
}
