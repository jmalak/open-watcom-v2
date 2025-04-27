/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter functions  &'vecpos( )
*                                                  &'veclastpos( )
*
*               The optional parms START and CASE are not implemented
*               as they are not used in OW doc build system
*
****************************************************************************/


#include "wgml.h"


static  bool    vec_pos;           // true if &'vecpos, false if &'veclastpos

/***************************************************************************/
/*  script string functions &'vecpos(                                      */
/*                          &'lastvecpos(                                  */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'vecpos(needle,haystack<,<start><,case>>):  The Vector Position func-  */
/*    tion returns the subscript number of  the first occurrence of 'nee-  */
/*    dle' in 'haystack'  set symbol.   The search for a  match starts at  */
/*    the first element  of 'haystack' but may be overridden  by adding a  */
/*    'start'  subscript in  'haystack'.    The default  case of 'M'  or  */
/*    'Mixed' searches for  an element which matches exactly,   a case of  */
/*    'U' or  'Upper' will match the  'needle' and 'haystack'  element in  */
/*    uppercase.    If the  'needle'  string is  not  found,   a zero  is  */
/*    returned.                                                            */
/*      .se rodent(1) = 'Gerbil'                                           */
/*      .se rodent(2) = 'Hamster'                                          */
/*      .se rodent(3) = 'Mouse'                                            */
/*      .se rodent(4) = 'Rabbit'                                           */
/*      &'vecpos('Hamster',rodent) ==> 2                                   */
/*      &'vecpos(Dog,rodent) ==> 0                                         */
/*      &'vecpos(Hamster,rodent,3) ==> 0                                   */
/*      &'vecpos(HAMSTER,rodent) ==> 0                                     */
/*      &'vecpos(HAMSTER,rodent,1,U) ==> 2                                 */
/*      &'vecpos(Mouse) ==> error, too few operands                        */
/*                                                                         */
/* ! start and case are NOT implemented                                    */
/*                                                                         */
/***************************************************************************/


/***************************************************************************/
/* &'veclastpos(needle,haystack<,<start><,case>>):  The Vector Last Posi-  */
/*    tion function returns  the subscript number of  the last occurrence  */
/*    of 'needle'  in 'haystack'  set symbol.    The search  for a  match  */
/*    starts at the first element of  'haystack' but may be overridden by  */
/*    adding a 'start' subscript in 'haystack'.   The default case of 'M'  */
/*    or 'Mixed' searches for an element which matches exactly, a case of  */
/*    'U' or  'Upper' will match the  'needle' and 'haystack'  element in  */
/*    uppercase.    If the  'needle'  string is  not  found,   a zero  is  */
/*    returned.                                                            */
/*                                                                         */
/* ! start and case are NOT implemented                                    */
/*                                                                         */
/***************************************************************************/

static  condcode    scr_veclp( parm parms[MAX_FUN_PARMS], int parmcount,
                               char **result, int ressize )
{
    tok_type        pneedle;
    tok_type        phay;
    int             rc;
    int             index;
    int             hay_len;
    int             needle_len;
    char            c;

    sub_index       var_ind;
    symvar          symvar_entry;
    symsub          *symsubval;
    symvar          *psymvar;
    bool            suppress_msg;


    (void)ressize;

    if( parmcount != 2 ) {
        return( neg );
    }

    pneedle = parms[0].arg;
    scr_unquote_parm( &pneedle );
    needle_len = pneedle.e - pneedle.s; // needle length

    phay = parms[1].arg;
    scr_unquote_parm( &phay );
    hay_len = phay.e - phay.s;          // haystack length

    rc = 0;
    scan_err = false;
    index = 0;

    if( (hay_len > 0)               // not null string
      || (needle_len > 0) ) {       // needle not null

        suppress_msg = ProcFlags.suppress_msg;
        ProcFlags.suppress_msg = true;
        scan_err = false;

        c = *phay.e;
        *phay.e = '\0';
        scan_sym( phay.s, &symvar_entry, &var_ind, NULL, false );
        *phay.e = c;

        ProcFlags.suppress_msg = suppress_msg;;

        if( !scan_err ) {
            if( symvar_entry.flags & local_var ) {  // lookup var in dict
                rc = find_symvar_l( input_cbs->local_dict, symvar_entry.name,
                                    var_ind, &symsubval );
            } else {
                rc = find_symvar( global_dict, symvar_entry.name, var_ind,
                                  &symsubval );
            }
            if( rc > 0 ) {              // variable found
                psymvar = symsubval->base;
                if( psymvar->flags & subscripted ) {
                    c = *pneedle.e;
                    *pneedle.e = '\0';   // make nul delimited
                    for( symsubval = psymvar->subscripts;
                         symsubval != NULL;
                         symsubval = symsubval->next ) {

                        if( strcmp( symsubval->value, pneedle.s ) == 0 ) {
                           index = symsubval->subscript;
                           if( vec_pos ) {
                               break;// finished for vec_pos, go for veclastpos
                           }
                        }
                    }
                    *pneedle.e = c;
                }
            }
        }
    }

    *result += sprintf( *result, "%d", index );

    return( pos );
}


/*
 * &'vecpos(
 *
 */

condcode    scr_vecpos( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int ressize )
{
    vec_pos = true;
    return( scr_veclp( parms, parmcount, result, ressize ) );
}


/*
 * &'veclastpos(
 *
 */

condcode    scr_veclastpos( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int ressize )
{
    vec_pos = false;
    return( scr_veclp( parms, parmcount, result, ressize ) );
}
