/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Implements the function used to parse .COP headers:
*                   parse_header()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
****************************************************************************/

#include <stdint.h>
#include <string.h>

#include "cophdr.h"


#define VERSION_TEXT    "V4.1 PC/DOS"

/* Function parse_header().
 * Determine if the current position of the input stream points to the
 * start of a valid same-endian version 4.1 binary device file and, if
 * it does, advance the stream to the first byte following the header.
 *
 * Parameter:
 *      in_file points the input stream.
 *
 * Returns:
 *      dir_v4_1_se if the file is a same-endian version 4.1 directory file.
 *      se_v4_1_not_dir if the file is a same-endian version 4.1 device,
 *          driver, or font file.
 *      not_se_v4_1 if the file is not same-endian and/or not version 4.1.
 *      not_bin_dev if the file is not a binary device file at all.
 *      file_error if an error occurred while reading the file.
 */

cop_file_type parse_header( FILE * in_file )
{
    char        count;
    char        text_version[sizeof( VERSION_TEXT )];
    uint16_t    version;

    /* Get the count and ensure it is 0x02. */

    count = fgetc( in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        return( file_error );
    }

    if( count != 0x02 ) {
        return( not_bin_dev );
    }

    /* Get the version. */

    fread( &version, 1, 2, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        return( file_error );
    }

    /* Check for a same_endian version 4.1 header.
    *  Note: checking 0x0c00 would, presumably, identify a different-endian
    *  version 4.1 header, if that ever becomes necessary.
    */

    if( version != 0x000c ) {
        return( not_se_v4_1 );
    }

    /* Get the text_version_length and ensure it is correct value. */

    count = fgetc( in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        return( file_error );
    }

    if( count != sizeof( VERSION_TEXT ) - 1 ) {
        return( not_bin_dev );
    }

    /* Verify the text_version. */

    fread( &text_version, 1, sizeof( VERSION_TEXT ) - 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        return( file_error );
    }

    text_version[sizeof( VERSION_TEXT ) - 1] = '\0';
    if( strcmp( text_version, VERSION_TEXT ) ) {
        return( not_bin_dev );
    }

    /* Get the type. */

    count = fgetc( in_file );

    /* If there is no more data, this is not a valid .COP file. */

    if( ferror( in_file ) || feof( in_file ) ) {
        return( file_error );
    }

    /* Valid header, more data exists, determine the file type. */

    if( count == 0x03 ) {
        return( se_v4_1_not_dir );
    }
    if( count == 0x04 ) {
        return( dir_v4_1_se );
    }

    /* Invalid file type: this cannot be a valid .COP file. */

    return( not_bin_dev );
}


