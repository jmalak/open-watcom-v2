/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter functions &'subword( )
*                                                     &'word( )
*                                                     &'words( )
*                                                     &'wordpos( )
****************************************************************************/


#include "wgml.h"


static  bool    is_word;          // true if word call, false if subword call

/***************************************************************************/
/*  script string function &'subword(                                      */
/*                         &'word(                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'subword(string,n<,length>):  The Subword function  returns the words  */
/*    of 'string' starting at word number 'n'.   The value of 'n' must be  */
/*    positive.  If 'length' is omitted, it will default to the remainder  */
/*    of the string.  The result will include all blanks between selected  */
/*    words and all leading and trailing blanks are not included.          */
/*      &'subword('The quick brown  fox',2,2) ==> "quick brown"            */
/*      &'subword('The quick brown  fox',3) ==> "brown  fox"               */
/*      &'subword('The quick brown  fox',5) ==> ""                         */
/*      &'subword('The quick brown  fox',0) ==> error, number too small    */
/*      &'subword('The quick brown  fox') ==> error, missing number        */
/*      &'subword('',1) ==> ""                                             */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/* &'word(string,n):  The  Word function returns  only the 'n'th  word in  */
/*    'string'.   The value of 'n' must be positive.   If there are fewer  */
/*    than 'n' blank delimited words in the 'string' then the null string  */
/*    is returned.                                                         */
/*      &'word('The quick brown fox',3) ==> "brown"                        */
/*      &'word('The quick brown fox',5) ==> ""                             */
/*      &'word('The quick brown fox',0) ==> error, too small               */
/*      &'word('The quick brown fox') ==> error, missing number            */
/*      &'word('',1) ==> ""                                                */
/***************************************************************************/

static  condcode    scr_xx_word( parm parms[MAX_FUN_PARMS], size_t parmcount,
                                 char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    char            *   ptok;
    condcode            cc;
    int                 k;
    int                 n;
    int                 len;
    getnum_block        gn;

    if( (parmcount < 2) || (parmcount > 3) ) {
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

    n   = 0;                            // default start pos
    gn.ignore_blanks = false;

    if( parms[1].e >= parms[1].a ) {// start pos specified
        gn.argstart = parms[1].a;
        gn.argstop  = parms[1].e;
        cc = getnum( &gn );
        if( (cc != pos) || (gn.result > len) ) {
            if( !ProcFlags.suppress_msg ) {
                xx_source_err_c( err_func_parm, "2 (startword)" );
            }
            return( cc );
        }
        n = gn.result;
    }

    if( is_word ) {
        len = 1;                        // only one word
    } else {
        if( parmcount > 2 ) {           // evalute word count
            if( parms[2].e >= parms[2].a ) {
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
    }

    scan_start = pval;
    scan_stop  = pend;
    k = 0;
    cc = pos;
    while( (k < n) && (cc != omit) ) {  // find start word
        cc = getarg();
        k++;
    }
    if( cc == omit ) {                  // start word does not exist
        **result = '\0';
        return( pos );
    }

    pval = g_tok_start;                 // start word

    if( len == 0 ) {                    // default word count = to end of string
        ptok = pend;
        for( ; pval <= ptok; pval++ ) { // copy rest of words
            if( ressize <= 0 ) {
                break;
            }
            **result = *pval;
            *result += 1;
            ressize--;
        }
    } else {
        k = 0;
        for( k = 0; k < len; k++ ) {
            ptok = g_tok_start;
            cc = getarg();
            if( cc == omit ) {
                ptok = pend;
                break;
            }
        }
        for( ; pval <= ptok; pval++ ) { // copy rest of words
            if( ressize <= 0 ) {
                break;
            }
            **result = *pval;
            *result += 1;
            ressize--;
        }
        if( pval < g_tok_start && ( *pval != ' ' ) ) {    // copy last word
            for( ; pval < g_tok_start; pval++ ) {
                if( ( *pval == ' ' ) || ( ressize <= 0 ) ) {
                    break;
                }
                **result = *pval;
                *result += 1;
                ressize--;
            }
        }
    }

    **result = '\0';

    return( pos );
}


/*
 * &'word( )
 *
 */

condcode    scr_word( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    is_word = true;
    return( scr_xx_word( parms, parmcount, result, ressize ) );
}


/*
 * &'subword( )
 *
 */

condcode    scr_subword( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    is_word = false;
    return( scr_xx_word( parms, parmcount, result, ressize ) );
}


/***************************************************************************/
/* &'words(string):  The Words function returns the number of words found  */
/*    in 'string'.                                                         */
/*      &'words('The quick brown fox') ==> 4                               */
/*      &'words(' ') ==> 0                                                 */
/*      &'words('') ==> 0                                                  */
/*      &'words('cat dot',1) ==> too many operands                         */
/***************************************************************************/

condcode    scr_words( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    int                 wc;
    int                 len;

    (void)ressize;

    if( parmcount != 1 ) {
        return( neg );
    }

    pval = parms[0].a;
    pend = parms[0].e;

    unquote_if_quoted( &pval, &pend );

    len = pend - pval + 1;

    if( len <= 0 ) {                    // null string nothing to do
        **result = '0';
        *result += 1;
        **result = '\0';
        return( pos );
    }

    wc = 0;
    for( ; pval <= pend; pval++ ) {     // for all chars in string

        for( ; pval <= pend; pval++ ) { // skip leading blanks
            if( *pval != ' ' ) {
                break;
            }
        }
        if( pval > pend ) {             // at end
            break;
        }
        wc++;                           // start of word found

        for( ; pval <= pend; pval++ ) {
            if( *pval == ' ' ) {        // end of word found
                break;
            }
        }
    }

    *result += sprintf( *result, "%d", wc );
    **result = '\0';
    return( pos );
}

/***************************************************************************/
/* &'wordpos(phrase,string<,start>):  The Word  Position function returns  */
/*    the word  position of  the words  in 'phrase'  within the  words of  */
/*    'string'.   The  search begins with the  first word of  'string' by  */
/*    default which may be changed by  specifying a positive 'start' word  */
/*    number.   All interword blanks are treated  as a single blank.   If  */
/*    the 'phrase' cannot be found the result is zero.                     */
/*      &'wordpos('quick brown fox','The quick brown fox') ==> 2           */
/*      &'wordpos('quick    brown','The quick  brown fox') ==> 2           */
/*      &'wordpos('quick  fox ','The quick  brown fox') ==> 0              */
/*      &'wordpos('xyz','The quick  brown fox') ==> 0                      */
/*      &'wordpos('The quick brown fox') ==> error, missing string         */
/***************************************************************************/

condcode    scr_wordpos( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   phrase;
    char            *   phrasend;
    char            *   pstr;
    char            *   pstrend;
    char            *   pp;

    int                 index;
    condcode            cc;
    int                 k;
    int                 n;
    int                 len;
    getnum_block        gn;
    bool                inword;
    bool                found;

    (void)ressize;

    if( (parmcount < 2) || (parmcount > 3) ) {
        return( neg );
    }

    phrase = parms[0].a;
    phrasend = parms[0].e;

    unquote_if_quoted( &phrase, &phrasend );

    len = phrasend - phrase + 1;

    pstr    = parms[1].a;
    pstrend = parms[1].e;

    unquote_if_quoted( &pstr, &pstrend );

    if( (len <= 0) ||                   // null phrase nothing to do
        (pstrend - pstr + 1 <= 0) ) {   // null string nothing to do

        **result = '0';
        *result += 1;
        **result = '\0';
        return( pos );
    }


    n = 0;                              // default start word - 1

    if( parmcount > 2 ) {               // evalute start word
        if( parms[2].e >= parms[2].a ) {
            gn.ignore_blanks = false;
            gn.argstart = parms[2].a;
            gn.argstop  = parms[2].e;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_c( err_func_parm, "3 (startword)" );
                }
                return( cc );
            }
            n = gn.result - 1;
        }
    }
    scan_start = pstr;
    scan_stop  = pstrend;
    k = 0;
    cc = pos;
    g_tok_start = pstr;
    while( ( k <= n ) && ( cc != omit ) ) { // find start word
        cc = getarg();
        k++;
    }
    if( cc == omit ) {                  // start word does not exist
        **result = '0';
        *result += 1;
        **result = '\0';
        return( pos );
    }

    pstr = g_tok_start;                   // start word in string
    index = 0;
    pp = phrase;
    inword = true;
    found = false;
    for( ; pstr <= pstrend; pstr++ ) {
        if( !inword ) {
            n++;
            inword = true;
        }
        if( *pstr == *pp ) {

            /* at end of phrase, but must also be at end of token */

            if( (pp == phrasend) && ((*(pstr + 1) == ' ') || (*(pstr + 1) == '\0')) ) {
                found = true;
                break;
            } else {
                if( *pstr == ' ' ) {
                    inword = false;     // word end
                    for( ; pstr <= pstrend; pstr++ ) {  // find next word
                        if( *pstr != ' ' ) {
                            break;
                        }
                    }
                    pstr--;            // outer for loop will increment again

                    for( ; pp <= phrasend; pp++ ) {
                        if( *pp != ' ' ) {
                            break;
                        }
                    }
                } else {
                    pp++;
                }
            }
        } else {                        // not equal
            pp = phrase;                // start new compare
            for( ; pstr <= pstrend; pstr++ ) {  // with next word
                if( *pstr == ' ' ) {    // word end found
                    break;
                }
            }
            inword = false;
            for( ; pstr <= pstrend; pstr++ ) {  // find next word
                if( *pstr != ' ' ) {
                    break;
                }
            }
            pstr--;                    // outer for loop will increment again
        }
    }
    if( found ) {
        index = n + 1;  // n is current word - 1, need current word
    }
    *result += sprintf( *result, "%d", index );
    **result = '\0';

    return( pos );
}
