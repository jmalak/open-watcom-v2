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

static tok_type *skipSpaces( tok_type *p )
{
    while( p->s <= p->e && *(p->s) == ' ' ) {
        p->s++;
    }
    return( p );
}

static tok_type *skipWord( tok_type *p )
{
    while( p->s <= p->e && *(p->s) != ' ' ) {
        p->s++;
    }
    return( p );
}

static tok_type *trimSpaces( tok_type *p )
{
    while( p->s <= p->e && *(p->s) == ' ' ) {
        p->s++;
    }
    while( p->s <= p->e && *(p->e) == ' ' ) {
        p->e--;
    }
    return( p );
}

static bool cmpWord( tok_type *p1, tok_type *p2 )
{
    bool ok1 = ( p1->s <= p1->e );
    bool ok2 = ( p2->s <= p2->e );
    while( ok1 && ok2 ) {
        char c1 = *(p1->s);
        char c2 = *(p2->s);
        if( c1 != c2 )
            return( false );
        if( c2 == ' ' ) {
            return( cmpWord( skipSpaces( p1 ), skipSpaces( p2 ) ) );
        }
        p1->s++;
        p2->s++;
    }
    if( ok1 )
        return( true );
    return( false );
}

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

static  condcode    scr_xx_word( parm parms[MAX_FUN_PARMS], int parmcount,
                                 char **result, int32_t ressize )
{
    tok_type        parm1;
    char            *ptok;
    condcode            cc;
    int                 k;
    int                 n;
    int                 len;
    getnum_block        gn;

    if( parmcount < 2
      || parmcount > 3 ) {
        return( neg );
    }

    parm1 = parms[0].arg;
    unquote_if_quoted( &parm1 );
    len = parm1.e - parm1.s + 1;              // default length

    if( len <= 0 ) {                    // null string nothing to do
        **result = '\0';
        return( pos );
    }

    n   = 0;                            // default start pos
    gn.ignore_blanks = false;

    if( parms[1].arg.s <= parms[1].arg.e ) {// start pos specified
        gn.arg = parms[1].arg;
        cc = getnum( &gn );
        if( cc != pos
          || gn.result > len ) {
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
            if( parms[2].arg.s <= parms[2].arg.e ) {
                gn.arg = parms[2].arg;
                cc = getnum( &gn );
                if( cc != pos
                  || gn.result == 0 ) {
                    if( !ProcFlags.suppress_msg ) {
                        xx_source_err_c( err_func_parm, "3 (length)" );
                    }
                    return( cc );
                }
                len = gn.result;
            }
        }
    }

    scan_start = parm1.s;
    scan_stop  = parm1.e;
    k = 0;
    cc = pos;
    while( k < n && cc != omit ) {      // find start word
        cc = getarg();
        k++;
    }
    if( cc == omit ) {                  // start word does not exist
        **result = '\0';
        return( pos );
    }

    parm1.s = g_tok_start;                 // start word

    if( len == 0 ) {                    // default word count = to end of string
        ptok = parm1.e;
        for( ; parm1.s <= ptok && ressize > 0; parm1.s++ ) { // copy rest of words
            **result = *parm1.s;
            *result += 1;
            ressize--;
        }
    } else {
        k = 0;
        for( k = 0; k < len; k++ ) {
            ptok = g_tok_start;
            cc = getarg();
            if( cc == omit ) {
                ptok = parm1.e;
                break;
            }
        }
        for( ; parm1.s <= ptok && ressize > 0; parm1.s++ ) { // copy rest of words
            **result = *parm1.s;
            *result += 1;
            ressize--;
        }
        if( parm1.s < g_tok_start
          && *parm1.s != ' ' ) {        // copy last word
            for( ; parm1.s < g_tok_start && ressize > 0; parm1.s++ ) {
                if( *parm1.s == ' ' ) {
                    break;
                }
                **result = *parm1.s;
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

condcode    scr_word( parm parms[MAX_FUN_PARMS], int parmcount, char * * result, int32_t ressize )
{
    is_word = true;
    return( scr_xx_word( parms, parmcount, result, ressize ) );
}


/*
 * &'subword( )
 *
 */

condcode    scr_subword( parm parms[MAX_FUN_PARMS], int parmcount, char * * result, int32_t ressize )
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

condcode    scr_words( parm parms[MAX_FUN_PARMS], int parmcount, char * * result, int32_t ressize )
{
    tok_type        parm1;
    int             wc;
    int             len;

    (void)ressize;

    if( parmcount != 1 ) {
        return( neg );
    }

    parm1 = parms[0].arg;
    unquote_if_quoted( &parm1 );
    len = parm1.e - parm1.s + 1;

    if( len <= 0 ) {                    // null string nothing to do
        **result = '0';
        *result += 1;
        **result = '\0';
        return( pos );
    }

    wc = 0;
    for( ; parm1.s <= parm1.e; parm1.s++ ) {     // for all chars in string

        for( ; parm1.s <= parm1.e; parm1.s++ ) { // skip leading blanks
            if( *parm1.s != ' ' ) {
                break;
            }
        }
        if( parm1.s > parm1.e ) {             // at end
            break;
        }
        wc++;                           // start of word found

        for( ; parm1.s <= parm1.e; parm1.s++ ) {
            if( *parm1.s == ' ' ) {        // end of word found
                break;
            }
        }
    }

    *result += sprintf( *result, "%d", wc );

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

condcode    scr_wordpos( parm parms[MAX_FUN_PARMS], int parmcount, char * * result, int32_t ressize )
{
    tok_type        phrase;
    tok_type        pstr;
    char            *pp;

    int             index;
    condcode        cc;
    int             k;
    int             n;
    int             len;
    getnum_block    gn;
    bool            inword;
    bool            found;

    (void)ressize;

    if( parmcount < 2
      || parmcount > 3 ) {
        return( neg );
    }

    phrase = parms[0].arg;
    unquote_if_quoted( &phrase );
    len = phrase.e - phrase.s + 1;

    pstr = parms[1].arg;
    unquote_if_quoted( &pstr );

    if( len <= 0                      // null phrase nothing to do
      || pstr.e - pstr.s + 1 <= 0 ) { // null string nothing to do

        **result = '0';
        *result += 1;
        **result = '\0';
        return( pos );
    }


    n = 0;                              // default start word - 1

    if( parmcount > 2 ) {               // evalute start word
        if( parms[2].arg.s <= parms[2].arg.e ) {
            gn.arg = parms[2].arg;
            gn.ignore_blanks = false;
            cc = getnum( &gn );
            if( cc != pos
              || gn.result == 0 ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_c( err_func_parm, "3 (startword)" );
                }
                return( cc );
            }
            n = gn.result - 1;
        }
    }
    scan_start = pstr.s;
    scan_stop  = pstr.e;
    k = 0;
    cc = pos;
    g_tok_start = pstr.s;
    while( k <= n && cc != omit ) {     // find start word
        cc = getarg();
        k++;
    }
    if( cc == omit ) {                  // start word does not exist
        **result = '0';
        *result += 1;
        **result = '\0';
        return( pos );
    }

    pstr.s = g_tok_start;                   // start word in string
    index = 0;
    pp = phrase.s;
    inword = true;
    found = false;
    for( ; pstr.s <= pstr.e; pstr.s++ ) {
        if( !inword ) {
            n++;
            inword = true;
        }
        if( *pstr.s == *pp ) {

            /* at end of phrase, but must also be at end of token */

            if( pp == phrase.e
              && ( *(pstr.s + 1) == ' '
              || *(pstr.s + 1) == '\0' ) ) {
                found = true;
                break;
            } else {
                if( *pstr.s == ' ' ) {
                    inword = false;     // word end
                    for( ; pstr.s <= pstr.e; pstr.s++ ) {  // find next word
                        if( *pstr.s != ' ' ) {
                            break;
                        }
                    }
                    pstr.s--;           // outer for loop will increment again

                    for( ; pp <= phrase.e; pp++ ) {
                        if( *pp != ' ' ) {
                            break;
                        }
                    }
                } else {
                    pp++;
                }
            }
        } else {                        // not equal
            pp = phrase.s;              // start new compare
            for( ; pstr.s <= pstr.e; pstr.s++ ) {  // with next word
                if( *pstr.s == ' ' ) {  // word end found
                    break;
                }
            }
            inword = false;
            for( ; pstr.s <= pstr.e; pstr.s++ ) {  // find next word
                if( *pstr.s != ' ' ) {
                    break;
                }
            }
            pstr.s--;                   // outer for loop will increment again
        }
    }
    if( found ) {
        index = n + 1;  // n is current word - 1, need current word
    }
    *result += sprintf( *result, "%d", index );

    return( pos );
}

/***************************************************************************
 *
 * &'find(string,phrase):  The Find function returns the word position of
 *    the words in 'phrase' within the words of 'string'.   All interword
 *    blanks are treated  as a single blank.   If the  'phrase' cannot be
 *    found the result is zero.
 *      &'find('The quick brown fox','quick brown fox') ==> 2
 *      &'find('The quick  brown fox','quick    brown') ==> 2
 *      &'find('The quick  brown fox','quick  fox ') ==> 0
 *      &'find('The quick  brown fox','xyz') ==> 0
 *      &'find('The quick brown fox') ==> error, missing phrase
 *
 ***************************************************************************/

condcode    scr_find( parm parms[MAX_FUN_PARMS], int parmcount, char **result, int32_t ressize )
{
    condcode        cc;
    int             index;
    int             index1;
    tok_type        phrase;
    tok_type        string;
    char            *phrase_start;
    char            *string_start;

    (void)ressize;

    if( parmcount != 2 ) {
        cc = neg;
        return( cc );
    }

    index = 0;

    phrase = parms[1].arg;
    unquote_if_quoted( &phrase );
    trimSpaces( &phrase );

    if( phrase.s <= phrase.e ) {

        string = parms[0].arg;
        unquote_if_quoted( &string );
        trimSpaces( &string );

        string_start = string.s;
        phrase_start = phrase.s;
        index1 = 1;
        while( string_start <= string.e ) {
            string.s = string_start;
            phrase.s = phrase_start;
            if( cmpWord( &string, &phrase ) ) {
                index = index1;
                break;
            } else {
                /*
                 * skip single word in "string"
                 */
                string.s = string_start;
                skipSpaces( skipWord( &string ) );
                string_start = string.s;
                index1++;
            }
        }
    }
    *result += sprintf( *result, "%d", index );
    return( pos );
}
