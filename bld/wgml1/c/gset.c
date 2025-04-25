/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  GML :SET processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :SET symbol='symbol-name'                                             */
/*        value='character-string'                                         */
/*              delete.                                                    */
/*                                                                         */
/* This tag defines and assigns a value to a symbol name.  The symbol      */
/* attribute must be specified.  The value of this attribute is the name   */
/* of the symbol being defined, and cannot have a length greater than ten  */
/* characters.  The symbol name may only contain letters, numbers, and the */
/* characters @, #, $ and underscore(_).  The value attribute must be      */
/* specified.  The attribute value delete or a valid character string may  */
/* be assigned to the symbol name.  If the attribute value delete is used, */
/* the symbol referred to by the symbol name is deleted.                   */
/***************************************************************************/

extern  void    gml_set( const gmltag * entry )
{
    bool            symbol_found    = false;
    bool            value_found     = false;
    char        *   p;
    char        *   pa;
    int             rc;
    symvar          sym;
    sub_index       subscript;
    sym_dict_hdl    working_dict;
    att_val_type    attr_val;
    char            attname[TAG_ATT_NAME_LENGTH + 1];

    (void)entry;

    subscript = no_subscript;           // not subscripted
    scan_err = false;

    p = scan_start;
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;;) {
            p = get_att_name( p, &pa, attname );
            if( ProcFlags.reprocess_line ) {
                break;
            }
            if( strcmp( "symbol", attname ) == 0 ) {

                /* both get_att_value() and scan_sym() must be used */

                p = get_att_value( p, &attr_val );
                if( attr_val.name == NULL ) {
                    break;
                }
                scan_sym( attr_val.name, &sym, &subscript, NULL, false );
                if( scan_err ) {
                    break;
                }
                symbol_found = true;
            } else if( strcmp( "value", attname ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.name == NULL ) {
                    break;
                }
                value_found = true;
                if( attr_val.len > buf_size - 1 )
                    attr_val.len = buf_size - 1;
                strncpy( token_buf, attr_val.name, attr_val.len );
                token_buf[attr_val.len] = '\0';
            } else if( strnicmp( token_buf, "delete", 6 ) == 0 ) {
                sym.flags |= deleted;
            } else {    // no match = end-of-tag in wgml 4.0
                ProcFlags.tag_end_found = true;
                p = pa; // restore spaces before text
                break;
            }
        }
    }

    if( symbol_found && value_found ) {   // both attributes
        if( sym.flags & local_var ) {
            working_dict = input_cbs->local_dict;
        } else {
            working_dict = global_dict;
        }
        rc = add_symvar( working_dict, sym.name, token_buf, subscript, sym.flags );
    } else {
        xx_err( ERR_ATT_MISSING );
    }

    if( !ProcFlags.reprocess_line && *p != '\0' ) {
        SkipDot( p );                       // possible tag end
        if( *p != '\0' ) {
            post_space = 0;
            ProcFlags.ct = true;
            process_text( p, g_curr_font);  // if text follows
        }
    }
    scan_start = scan_stop;
    return;
}

