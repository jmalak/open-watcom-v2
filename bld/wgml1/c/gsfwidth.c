/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'width( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'width(                                        */
/*                                                                         */
/*  some logic has to be in sync with scr_singele_func_w in gsfunelu.c     */
/***************************************************************************/

/***************************************************************************/
/*&'width(string<,type>):   The  Width function  returns  the  formatted   */
/*   width of 'string' in units based  on the current Character Per Inch   */
/*   setting.   The returned width is rounded  up if the actual width is   */
/*   not an  exact multiple of a  CPI unit.   An optional  second 'type'   */
/*   operand may be specified to define the  units in which the width is   */
/*   to be  defined:   a  'type' of  "C" or  "CPI" is  default,  "N"  or   */
/*   "Number" is the number of  characters not including overstrikes and   */
/*   escape sequences,  and  "U" or "Units" returns the  width in output   */
/*   Device Units.                                                         */
/*     &'width(abc) ==> 3                                                  */
/*     &'width('A&SYSBS._') ==> 1                                          */
/*     &'width(ABCDEFG,C) ==> width in CPI units                           */
/*     &'width(ABCDEFG,N) ==> number of characters                         */
/*     &'width(ABCDEFG,U) ==> width in Device Units                        */
/*     &'width(AAA,X) ==> invalid 'type' operand                           */
/***************************************************************************/

condcode    scr_width( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int32_t ressize )
{
    tok_type        parm1;
    tok_type        parm2;
    int             len;
    char            type;
    uint32_t        width;

    (void)ressize;

    if( (parmcount < 1) || (parmcount > 2) ) {
        return( neg );
    }

    parm1 = parms[0].arg;
    unquote_if_quoted( &parm1 );
    len = parm1.e - parm1.s + 1;

    if( len <= 0 ) {                    // null string width 0
        **result = '0';
        *result += 1;
        **result = '\0';
        return( pos );
    }

    if( parmcount > 1 ) {               // evalute type
        if( parms[1].arg.s <= parms[1].arg.e ) {// type
            parm2 = parms[1].arg;
            unquote_if_quoted( &parm2 );
            type = my_tolower( *parm2.s );
            switch( type ) {
            case 'c':                 // CPI
                width = cop_text_width( parm1.s, len, g_curr_font );
                width = (width * CPI + g_resh / 2) / g_resh;
                break;
            case 'u':                 // Device Units
                width = cop_text_width( parm1.s, len, g_curr_font );
                break;
            case 'n':                 // character count
                width = len;
                break;
            default:
                xx_source_err_c( err_func_parm, "2 (type)" );
            }
        }
    } else {                            // default type c processing
        width = cop_text_width( parm1.s, len, g_curr_font );
        width = (width * CPI + g_resh / 2) / g_resh;
    }

    *result  += sprintf( *result, "%d", width );

    return( pos );
}
