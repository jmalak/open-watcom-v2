/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'translate( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'translate(                                    */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'translate(string<,<tableo><,<tablei><,pad>>>):      To     Translate  */
/*    'string' with characters  in 'tablei' to the  corresponding charac-  */
/*    ters in  'tableo'.   The  'pad' character  will extend  'tableo' to  */
/*    equal the length of 'tablei',  if  required.   If both 'tableo' and  */
/*    'tablei' are omitted then 'string' is converted to uppercase.        */
/*      &'translate('abcdef') ==> ABCDEF                                   */
/*      &'translate('abbc','&','b') ==> a&&c                               */
/*      &'translate('abcdef','12','ec') ==> ab2d1f                         */
/*      &'translate('abcdef','12','abcd','.') ==> 12..ef                   */
/*      &'translate('4321','abcd','1234') ==> dcba                         */
/*      &'translate('123abc',,,'$') ==> $$$$$$                             */
/*                                                                         */
/***************************************************************************/

condcode    scr_translate( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    tok_type        parm1;
    tok_type        ptabo;
    tok_type        ptabi;
    char            padchar;
    char            c;
    char            *iptr;
    char            *optr;
    bool            ifound;
    int             offset;
    bool            padchar_set;

    if( (parmcount < 1) || (parmcount > 4) ) {
        return( neg );
    }

    parm1 = parms[0].arg;
    unquote_if_quoted( &parm1 );

    if( parm1.e - parm1.s + 1 <= 0 ) {      // null string nothing to do
        **result = '\0';
        return( pos );
    }

    ptabo = parms[1].arg;
    if( (parmcount > 1) && (ptabo.s <= ptabo.e) ) {   // tableo is not empty
        unquote_if_quoted( &ptabo );
    } else {
        ptabo.s = NULL;
        ptabo.e = NULL;
    }

    ptabi = parms[2].arg;
    if( (parmcount > 2) && (ptabi.s <= ptabi.e) ) {   // tablei is not empty
        unquote_if_quoted( &ptabi );
    } else {
        ptabi.s = NULL;
        ptabi.e = NULL;
    }

    if( parmcount > 3 ) {               // padchar specified
        tok_type parm = parms[3].arg;
        unquote_if_quoted( &parm );
        padchar = *parm.s;
        padchar_set = true;
    } else {
        padchar = ' ';                  // padchar default is blank
        padchar_set = false;
    }

    if( (ptabi.s == NULL) && (ptabo.s == NULL) && !padchar_set ) {
        while( (parm1.s <= parm1.e) && (ressize > 0) ) {  // translate to upper
            **result = my_toupper( *parm1.s++ );
            *result += 1;
            ressize--;
        }
    } else {                   // translate as specified in tablei and tableo
        for( ; parm1.s <= parm1.e; parm1.s++ ) {
            c = *parm1.s;
            ifound = false;
            if( ptabi.s == NULL ) {
                c = padchar;
            } else {
                for( iptr = ptabi.s; iptr <= ptabi.e; iptr++ ) {
                    if( c == *iptr ) {
                        ifound = true;  // char found in input table
                        offset = iptr - ptabi.s;
                        optr = ptabo.s + offset;
                        if( optr <= ptabo.e ) {
                            **result = *optr;  // take char from output table
                        } else {
                            **result = padchar;// output table too short use padchar
                        }
                        break;
                    }
                }
            }
            if( !ifound ) {
                **result = c;           // not found, leave unchanged
            }
            *result += 1;
            ressize--;
            if( ressize <= 0 ) {
                break;
            }
        }
    }

    **result = '\0';

    return( pos );
}
