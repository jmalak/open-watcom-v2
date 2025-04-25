/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Implements reference dictionary and helper functions
*                   for :Hx, :HDREF, :FIG, :FN, :FIGREF, :FNREF tags
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  get_refid_value        parse reference id                              */
/*                                                                         */
/*  Note: parameter refid should be a pointer to char[ID_LEN]              */
/***************************************************************************/

char    *get_refid_value( char *p, att_val_type *attr_val, char *refid )
{
    size_t  k;

    p = get_att_value( p, attr_val );

    if( attr_val->name == NULL ) {       // no valid id
        return( p );
    }
    if( attr_val->len > ID_LEN )
        attr_val->len = ID_LEN;
    for( k = 0; k < attr_val->len; k++ ) {
        refid[k] = my_tolower( attr_val->name[k] );
    }
    refid[attr_val->len] = '\0';
    if( attr_val->len > 7 ) {                 // wgml 4 warning level
        xx_warn_c_info( WNG_ID_XXX, refid, INF_ID_LEN );
    }
    return( p );
}


/***************************************************************************/
/*  init_ref_dict   initialize dictionary pointer                          */
/***************************************************************************/

void    init_ref_dict( ref_dict *dict )
{
    *dict = NULL;
    return;
}


/***************************************************************************/
/*  add_ref_entry   add ref entry to dictionary                            */
/***************************************************************************/

void    add_ref_entry( ref_dict *dict, ref_entry *re )
{
    ref_entry   *wk;

    if( *dict == NULL ) {               // empty dictionary
        *dict = re;
    } else {
        wk = *dict;
        while( wk->next != NULL ) {     // search last entry in dictionary
            wk = wk->next;
        }
        wk->next = re;
    }
}


/***************************************************************************/
/*  free_ref_entry  delete single refentry                                 */
/***************************************************************************/

static void    free_ref_entry( ref_dict *dict, ref_entry *me )
{
    ref_entry   *wk;
    ref_entry   *wkn;

    if( me != NULL ) {
        if( *dict == me ) {             // delete first entry
            *dict = me->next;
        } else {
            wk = *dict;
            while( wk != NULL ) {       // search the entry in refdictionary
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
/*  free_ref_dict   free all ref dictionary entries                        */
/***************************************************************************/

void    free_ref_dict( ref_dict *dict )
{
    ref_entry   *wk;
    ref_entry   *wkn;

    wk = *dict;
    while( wk != NULL ) {
        wkn = wk->next;
        free_ref_entry( dict, wk );
        wk = wkn;
    }
    *dict = NULL;                       // dictionary is empty
    return;
}


/***************************************************************************/
/* init ref entry with some values    assumes :hx :fig variant             */
/***************************************************************************/

void init_ref_entry( ref_entry *re, const char *refid, ffh_entry *ffh )
{
    re->next = NULL;
    strcpy( re->refid, refid );
    if( ffh != NULL ) {
        re->flags = rf_ffh;
        re->u.ffh.entry = ffh;
    } else {
        re->flags = rf_ix;
        re->u.ix.hblk = NULL;
        re->u.ix.base = NULL;
    }
}


/***************************************************************************/
/*  search ref   entry in specified dictionary                             */
/*  name is assumed to be lowercase                                        */
/*  returns ptr to entry or NULL if not found                              */
/***************************************************************************/

ref_entry   *find_refid( ref_dict dict, const char *refid )
{
    ref_entry   *wk;
    ref_entry   *curr;

    wk = NULL;
    if( *refid != '\0' ) {          // don't match case with no id
        for( curr = dict; curr != NULL; curr = curr->next ) {
            if( strcmp( curr->refid, refid ) == 0 ) {
                wk = curr;
                break;
            }
        }
    }
    return( wk );
}


/***************************************************************************/
/*  print_ref_dict  output all ref dictionary entries                      */
/*  type can be HDREF, FIGREF, FNREF                                       */
/*  and INDEX                                                              */
/***************************************************************************/

void    print_ref_dict( ref_dict dict, const char *type )
{
    ref_entry           *wk;
    int                 cnt;
    size_t              len;
    char                fill[ID_LEN + 1];
    bool                withnumber;
    char                *text;

    cnt = 0;
    if( dict != NULL ) {
        memset( fill, ' ', ID_LEN );
        fill[ID_LEN] = '\0';
        out_msg( "\nList of %s entries:\n\n", type );
        if( strcmp( "INDEX", type ) ) {
            withnumber = strcmp( "HDREF", type );   // true for :FIG and :FN
            for( wk = dict; wk != NULL; wk = wk->next ) {
                len = strlen( wk->refid );
                if( withnumber ) {
                    out_msg( "%4ld ID='%s'%s page %-3ld text='%s'\n", wk->u.ffh.entry->number,
                             wk->refid, &fill[len], wk->u.ffh.entry->pageno, wk->u.ffh.entry->text );
                } else {
                    out_msg( "ID='%s'%s page %-3ld text='%s'\n", wk->refid,
                            &fill[len], wk->u.ffh.entry->pageno, wk->u.ffh.entry->text );
                }
                cnt++;
            }
        } else {                        // "INDEX"
            for( wk = dict; wk != NULL; wk = wk->next ) {
                len = strlen( wk->refid );
                if( wk->u.ix.hblk->prt_term != NULL ) {
                    text = wk->u.ix.hblk->prt_term;
                } else {
                    text = wk->u.ix.hblk->ix_term;
                }
                out_msg( "ID='%s'%s '%s'\n", wk->refid,
                         &fill[len], text );
                cnt++;
            }
        }
    }
    out_msg( "\nTotal %s entries: %d\n", type, cnt );
    return;
}

ref_entry *add_new_refid( ref_dict *dict, const char *refid, ffh_entry *ffh )
{
    ref_entry   *ref;

    ref = (ref_entry *)mem_alloc( sizeof( ref_entry ) ) ;
    init_ref_entry( ref, refid, ffh );
    add_ref_entry( dict, ref );
    return( ref );
}
