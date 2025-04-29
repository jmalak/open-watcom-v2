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

condcode    scr_strip( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    char            *   pa;
    char            *   pe;
    int                 len;
    char                stripchar;
    char                type;

    if( (parmcount < 1) || (parmcount > 3) ) {
        return( neg );
    }

    pval = parms[0].a;
    pend = parms[0].e;

    unquote_if_quoted( &pval, &pend );

    len = pend - pval + 1;              // default length

    if( len <= 0 ) {                    // null string nothing to do
        **result = '\0';
        return( pos );
    }

    stripchar = ' ';                    // default char to delete
    type      = 'b';                    // default strip both ends

    if( parmcount > 1 ) {               // evalute type
        if( parms[1].a <= parms[1].e ) {// type
            pa  = parms[1].a;
            pe  = parms[1].e;

            unquote_if_quoted( &pa, &pe );
            type = my_tolower( *pa );

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
        if( parms[2].a <= parms[2].e ) {
            pa  = parms[2].a;
            pe  = parms[2].e;

            unquote_if_quoted( &pa, &pe );
            stripchar = *pa;
        }
    }

    if( type != 't' ) {                 // strip leading requested
        for( ; pval <= pend; pval++ ) {
            if( *pval != stripchar ) {
                break;
            }
        }
    }

    for( ; pval <= pend; pval++ ) {
        if( ressize <= 0 ) {
            break;
        }
        **result = *pval;
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
