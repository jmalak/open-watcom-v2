/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Declares functions used to manipulate .COP directory files:
*                   get_compact_entry()
*                   get_extended_entry()
*                   get_member_name()
*
* Note:         The field names are intended to correspond to the field names
*               shown in the Wiki. The Wiki structs are named when the structs
*               defined here are defined; they are not identical.
*
****************************************************************************/

#ifndef COPDIR_H_INCLUDED
#define COPDIR_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

/* Macro definition. */

#define DEFINED_NAME_MAX 78 // Per documentation, max length of a defined name.

/* Struct declaration. */

/* _MAX_PATH is used for the member_name because gendev will embed member
 * names which exceed the space allowed by NAME_MAX or _MAX_FNAME in DOS if
 * such a member name is present in the source file.
 */

/* To hold the data from either the CompactDirEntry struct or the
 * ExtendedDirEntry struct, since, in either case, only these two fields are used.
 */

typedef struct {
    char    defined_name[DEFINED_NAME_MAX];
    char    member_name[_MAX_PATH];
} directory_entry;

/* Enum declaration. */

typedef enum {
    valid_entry,        // Both defined_name and member_name were found.
    not_valid_entry     // The entry was not valid.
} entry_found;

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern entry_found  get_compact_entry( FILE *fp, directory_entry * entry );
extern entry_found  get_extended_entry( FILE *fp, directory_entry * entry );
extern char         *get_member_name( FILE *fp, const char *in_name );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* COPDIR_H_INCLUDED */
