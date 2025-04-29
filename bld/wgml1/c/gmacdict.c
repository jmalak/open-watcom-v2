/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Implements script macros (tables and access routines)
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  Private symbol dictionary type                                         */
/***************************************************************************/

#define MAC_HASH_SIZE       241

typedef struct macdict {
    mac_entry   *htbl[MAC_HASH_SIZE];   // hash table
    long        lookups;                // lookup counter
    long        macros;                 // macro counter
    long        compares;               // strcmp counter
} macdict;


/* For macro name hashing, we must limit the number of characters
 * that are considered for the name.
 */
static unsigned hashpjw_mac( const char *s, int max_len )
{
    unsigned    h;
    int         count = 2;  // we only start checking after 2 chars
    char        c;

    h = *s++;
    if( h != 0 ) {
        c = *s++;
        if( c != '\0' ) {
            h = ( h << 4 ) + c;
            for( ;; ) {
                h &= 0x0fff;
                c = *s++;
                count++;
                if( c == '\0' || count == max_len )
                    break;
                h = ( h << 4 ) + c;
                h = ( h ^ (h >> 12) ) & 0x0fff;
                c = *s++;
                count++;
                if( c == '\0' || count == max_len )
                    break;
                h = ( h << 4 ) + c;
                h = h ^ (h >> 12);
            }
        }
    }
    return( h );
}


static int mac_hash( const char *name )
{
    unsigned    hash;

    hash = hashpjw_mac( name, MAC_NAME_LENGTH );
    hash %= MAC_HASH_SIZE;
    return( hash );
}


/***************************************************************************/
/*  init_macro_dict   initialize dictionary pointer                        */
/***************************************************************************/

mac_dict_hdl    init_macro_dict( void )
{
    mac_dict_hdl    dict;

    dict = mem_alloc( sizeof( *dict ) );
    memset( dict->htbl, 0, sizeof( dict->htbl ) );
    dict->lookups  = 0;
    dict->macros   = 0;
    dict->compares = 0;
    return( dict );
}


/***************************************************************************/
/*  add_macro_entry   add macro entry to dictionary                        */
/***************************************************************************/

void    add_macro_entry( mac_dict_hdl dict, mac_entry * me )
{
    mac_entry   *   wk;
    int             hash;

    hash = mac_hash( me->macname );
    wk = dict->htbl[hash];          // find the hash chain

    if( wk ) {
        while( wk->next != NULL ) { // find last entry in chain
            wk = wk->next;
        }
        wk->next = me;
    } else {
        dict->htbl[hash] = me;      // set first entry in chain
    }
    dict->macros++;
}


/***************************************************************************/
/*  free_macro_entry_short  free storage for a macro entry                 */
/*  without chain update                                                   */
/***************************************************************************/

static  void    free_macro_entry_short( mac_entry * me )
{
    inp_line    *   ml;
    inp_line    *   mln;
    labelcb     *   cb;

    if( me != NULL ) {
        cb = me->label_cb;
        if( WgmlFlags.research ) {
            print_labels( cb, me->macname );   // print label info
        }
        while( cb != NULL ) {
            me->label_cb = cb->prev;
            mem_free( cb );
            cb = me->label_cb;
        }
        ml = me->macline;
        while( ml != NULL ) {           // free all macro lines
            mln = ml->next;
            mem_free( ml );
            ml = mln;
        }
        mem_free( me );                 // now the entry itself
    }
    return;
}

/***************************************************************************/
/*  free_macro_entry  delete single macroentry with chain update           */
/***************************************************************************/
void    free_macro_entry( mac_dict_hdl dict, mac_entry * me )
{
    inp_line    *   ml;
    inp_line    *   mln;
    mac_entry   *   wk;
    mac_entry   *   wkn;
    labelcb     *   cb;
    int             hash;

    if( me != NULL ) {
        cb = me->label_cb;
        if( WgmlFlags.research ) {
            print_labels( cb, me->macname );// print label info
        }
        while( cb != NULL ) {
            me->label_cb = cb->prev;
            mem_free( cb );
            cb = me->label_cb;
        }
        ml = me->macline;
        while( ml != NULL ) {           // free all macro lines
             mln = ml->next;
             mem_free( ml );
             ml = mln;
        }

        hash = mac_hash( me->macname );
        wk = dict->htbl[hash];          // find the hash chain

        if( wk == me ) {                // first entry in the chain?
            dict->htbl[hash] = wk->next;
        } else {
            while( wk != NULL ) {       // search the entry in macro dictionary
                wkn = wk->next;
                if( wkn == me ) {
                    wk->next = me->next;// chain update
                    break;
                }
                wk = wkn;
            }
        }
        mem_free( me );                 // now the entry itself
    }
    return;
}

/***************************************************************************/
/*  free_macro_dict   free all macro dictionary entries                    */
/***************************************************************************/

void    free_macro_dict( mac_dict_hdl dict )
{
    int             i;
    mac_entry   *   wk;
    mac_entry   *   wkn;

    for ( i = 0; i < MAC_HASH_SIZE; ++i ) {
        wk = dict->htbl[i];
        while( wk != NULL ) {
            wkn = wk->next;
            free_macro_entry_short( wk );
            wk = wkn;
        }
    }
#if 0
    printf( "macro lookups   : %ld\n", dict->lookups );
    printf( "macro compares  : %ld\n", dict->compares );
#endif
    mem_free( dict );
}


/***************************************************************************/
/*  search macro entry in specified dictionary                             */
/*  returns ptr to macro or NULL if not found                              */
/***************************************************************************/

mac_entry   *find_macro( mac_dict_hdl dict, const char *macname )
{
    int             hash;
    mac_entry   *   wk;
    mac_entry   *   curr;

    wk   = NULL;
    hash = mac_hash( macname );
    dict->lookups++;
    // find the hash chain
    for( curr = dict->htbl[hash]; curr != NULL; curr = curr->next ) {
        dict->compares++;
        if( strncmp( curr->macname, macname, MAC_NAME_LENGTH ) == 0 ) {
            wk = curr;
            break;
        }
    }
    return( wk );
}


/***************************************************************************/
/*  print_macro_dict  output all of the macro dictionary                   */
/***************************************************************************/

void    print_macro_dict( mac_dict_hdl dict, bool with_mac_lines )
{
    mac_entry           *   wk;
    int                     cnt;
    int                     len;
    inp_line            *   ml;
    int                     lc;
    int                     i;
    static  const   char    fill[10] = "         ";

    cnt = 0;
    out_msg( "\nList of defined macros:\n\n" );
    for( i = 0; i < MAC_HASH_SIZE; ++i ) {
        for( wk = dict->htbl[i]; wk != NULL; wk = wk->next ) {

            len =  strlen( wk->macname );
            out_msg( "Macro='%s'%sdefined line %d file '%s'\n", wk->macname,
                    &fill[len], wk->lineno, wk->mac_file_name );
            if( with_mac_lines ) {
                for( ml = wk->macline, lc = 1; ml != NULL; ml = ml->next, lc++ ) {
                    out_msg("%+.3d %s\n", lc, ml->value );
                }
            }
            cnt++;
        }
    }
    out_msg( "\nTotal macros defined: %d\n", cnt );
    return;
}

