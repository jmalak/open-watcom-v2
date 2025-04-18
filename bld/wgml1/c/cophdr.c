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
#include "copfunc.h"


#define VERSION_TEXT    "V4.1 PC/DOS"

/* Function parse_header().
 * Determine if the current position of the input stream points to the
 * start of a valid same-endian version 4.1 binary device file and, if
 * it does, advance the stream to the first byte following the header.
 *
 * Parameter:
 *      fp points the input stream.
 *
 * Returns:
 *      file type of file or error.
 */

cop_file_type parse_header( FILE * fp )
{
    char        count;
    char        text_version[sizeof( VERSION_TEXT )];
    uint16_t    version;

    /* Get the count and ensure it is 0x02. */

    count = fread_u8( fp );
    if( ferror( fp ) || feof( fp ) ) {
        return( file_type_error );
    }

    if( count != 0x02 ) {
        return( file_type_unknown );
    }

    /* Get the version. */

    fread_buff( &version, sizeof( version ), fp );
    if( ferror( fp ) || feof( fp ) ) {
        return( file_type_error );
    }

    /* Check for a same_endian version 4.1 header.
    *  Note: checking 0x0c00 would, presumably, identify a different-endian
    *  version 4.1 header, if that ever becomes necessary.
    */

    if( version != 0x000c ) {
        return( file_type_wrong_ver );
    }

    /* Get the text_version_length and ensure it is correct value. */

    count = fread_u8( fp );
    if( ferror( fp ) || feof( fp ) ) {
        return( file_type_error );
    }

    if( count != sizeof( VERSION_TEXT ) - 1 ) {
        return( file_type_unknown );
    }

    /* Verify the text_version. */

    fread_buff( &text_version, sizeof( VERSION_TEXT ) - 1, fp );
    if( ferror( fp ) || feof( fp ) ) {
        return( file_type_error );
    }

    text_version[sizeof( VERSION_TEXT ) - 1] = '\0';
    if( strcmp( text_version, VERSION_TEXT ) ) {
        return( file_type_unknown );
    }

    /* Get the type. */

    count = fread_u8( fp );

    /* If there is no more data, this is not a valid .COP file. */

    if( ferror( fp ) || feof( fp ) ) {
        return( file_type_error );
    }

    /* Valid header, more data exists, determine the file type. */

    if( count == 0x03 ) {
        char    type_name[3];
        fpos_t  posx;

        /* Get the type_name. */

        fgetpos( fp, &posx );
        fread_buff( type_name, 3, fp );
        if( ferror( fp ) || feof( fp ) ) {
            return( file_type_error );
        }

        /* Verify that the type_name is for a .COP device file. */

        if( strncmp( type_name, "DEV", 3 ) == 0 )
            // The file is device.
            return( file_type_dev );
        if( strncmp( type_name, "DRV", 3 ) == 0 )
            // The file is driver.
            return( file_type_drv );
        if( strncmp( type_name, "FON", 3 ) == 0 )
            // The file is font.
            return( file_type_fon );

        fsetpos( fp, &posx );

        return( file_type_unknown );
    }
    if( count == 0x04 ) {
        return( file_type_dir );
    }

    /* Invalid file type: this cannot be a valid .COP file. */

    return( file_type_unknown );
}


