/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'strip( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'strip(                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'strip(string<,<type><,char>>):   To remove  leading and/or  trailing  */
/*    'char's from a 'string'.  The 'type' specified may be 'L' to remove  */
/*    leading characters,  'T' for trailing  characters,  or 'B' for both  */
/*    leading and trailing characters.   If omitted the default 'type' is  */
/*    'B'.   All leading  or trailing characters matching  'char' will be  */
/*    removed, the default 'char' being a blank.                           */
/*      "&'strip('  the dog  ')" ==> "the dog"                             */
/*      "&'strip('  the dog  ','L')" ==> "the dog  "                       */
/*      "&'strip('  the dog  ','t')" ==> "  the dog"                       */
/*      "&'strip('a-b--',,'-')" ==> "a-b"                                  */
/*      "&'strip(--a-b--,,-)" ==> "a-b"                                    */
/*                                                                         */
/***************************************************************************/

condcode    scr_strip( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int32_t ressize )
{
    tok_type        parm1;
    int             len;
    char            stripchar;
    char            type;

    if( (parmcount < 1) || (parmcount > 3) ) {
        return( neg );
    }

    parm1 = parms[0].arg;
    unquote_if_quoted( &parm1 );
    len = parm1.e - parm1.s + 1;        // default length

    if( len <= 0 ) {                    // null string nothing to do
        **result = '\0';
        return( pos );
    }

    stripchar = ' ';                    // default char to delete
    type      = 'b';                    // default strip both ends

    if( parmcount > 1 ) {               // evalute type
        if( parms[1].arg.s <= parms[1].arg.e ) {// type
            tok_type parm = parms[1].arg;
            unquote_if_quoted( &parm );
            if( parm.s <= parm.e ) {
                type = my_tolower( *parm.s );
            }

            switch( type ) {
            case   'b':
            case   'l':
            case   't':
                // type value is valid do nothing
                break;
            default:
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_c( err_func_parm, "2 (type)" );
                }
                return( neg );
                break;
            }
        }
    }

    if( parmcount > 2 ) {               // stripchar
        if( parms[2].arg.s <= parms[2].arg.e ) {
            tok_type parm = parms[2].arg;
            unquote_if_quoted( &parm );
            if( parm.s <= parm.e ) {
                stripchar = *parm.s;
            }
        }
    }

    if( type != 't' ) {                 // strip leading requested
        for( ; parm1.s <= parm1.e; parm1.s++ ) {
            if( *parm1.s != stripchar ) {
                break;
            }
        }
    }

    while( parm1.s <= parm1.e && ressize > 0 ) {
        **result = *parm1.s++;
        *result += 1;
        ressize--;
    }

    if( type != 'l' ) {                 // strip trailing requested
        while( *(*result - 1) == stripchar ) {
            *result -= 1;
        }
    }

    **result = '\0';

    return( pos );
}
