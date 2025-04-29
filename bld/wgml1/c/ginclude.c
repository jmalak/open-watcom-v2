/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  GML :include   processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  :IMBED   file='abc.gml'                                                */
/*  :INCLUDE file="abc.gml"                                                */
/*                                                                         */
/* :IMBED and :INCLUDE are eqivalent.                                      */
/*                                                                         */
/* The value of the required attribute file is used as the name of the     */
/* file to include.  The content of the included file is processed by      */
/* WATCOM Script/GML as if the data was in the original file.  This tag    */
/* provides the means whereby a document may be specified using a          */
/* collection of separate files.  Entering the source text into separate   */
/* files, such as one file for each chapter, may help in managing the      */
/* document.  if( the specified file does not have a file type, the        */
/* default document file type is used.  For example, if the main document  */
/* file is manual.doc, doc is the default document file type.  If the file */
/* is not found, the alternate extension supplied on the command line is   */
/* used.  If the file is still not found, the file type GML is used.  When */
/* working on a PC/DOS system, the DOS environment symbol GMLINC may be    */
/* set with an include file list.  This symbol is defined in the same way  */
/* as a library definition list, and provides a list of alternate          */
/* directories for file inclusion.  If an included file is not defined in  */
/* the current directory, the directories specified by the include path    */
/* list are searched for the file.  If the file is still not found, the    */
/* directories specified by the DOS environment symbol PATH are searched.  */
/*                                                                         */
/* NOTE: The attribute turns out to not, in face, be needed: the token     */
/*       after the tag is treated as a filename if it is not "file"        */
/*                                                                         */
/***************************************************************************/

void    gml_include( const gmltag *entry )
{
    char            *p;
    char            *pa;
    att_val_type    attr_val;
    char            attname[TAG_ATT_NAME_LENGTH + 1];

    (void)entry;

    *token_buf = '\0';
    p = scan_start;
    p++;
    SkipSpaces( p );
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        p = get_att_name( p, &pa, attname );
        if( !ProcFlags.reprocess_line ) {
            if( strcmp( "file", attname ) == 0 ) {
                p = get_att_value( p, &attr_val );
            } else {
                p = get_tag_value( pa, &attr_val ); // reset for possible file name
            }
            if( attr_val.name != NULL ) {
                if( attr_val.len > _MAX_PATH - 1 )
                    attr_val.len = _MAX_PATH - 1;
                strncpy( token_buf, attr_val.name, attr_val.len );
                token_buf[attr_val.len] = '\0';
                ProcFlags.newLevelFile = 1;     // start new include level
                scan_start = scan_stop;         // .. and ignore remaining line
            }
        } else {                                // wgml 4.0 appears to mark "" as the filename
            ProcFlags.newLevelFile = 1;         // start new include level
            scan_start = scan_stop;             // .. and ignore remaining line
        }
    }

    return;
}

