/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Implements the external functions declared in cfdir.h:
*                   get_compact_entry()
*                   get_extended_entry()
*                   get_member_name()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
*
****************************************************************************/


#include "wgml.h"
#include "copdir.h"
#include "cophdr.h"

#include "clibext.h"


#define DEFINED_NAME_MAX 78 // Per documentation, max length of a defined name.

/* _MAX_PATH is used for the member_name because gendev will embed member
 * names which exceed the space allowed by NAME_MAX or _MAX_FNAME in DOS if
 * such a member name is present in the source file.
 */

/* To hold the data from either the CompactDirEntry struct or the
 * ExtendedDirEntry struct, since, in either case, only these two fields are used.
 */

typedef struct {
    char    defined_name[DEFINED_NAME_MAX + 1];
    char    member_name[_MAX_PATH];
} directory_entry;

typedef enum {
    valid_entry,        // Both defined_name and member_name were found.
    not_valid_entry     // The entry was not valid.
} entry_found;

/* Global function definitions. */

/* Function get_compact_entry().
 * Reads and returns the current (compact) entry.
 *
 * Parameters:
 *      fp points to directory file.
 *      entry is intended to contain the current entry.
 *
 * Modified Parameter:
 *      entry will be modified as indicated below.
 *
 * Returns:
 *      valid_entry if both values of entry were updated.
 *      not_valid_entry if only one of the values or neither of the values
 *          of entry was updated.
 *
 * Notes:
 *      The defined_name and the embedded_name fields will be terminated
 *          with '\0' if updated.
 *      A file error may have occurred if not_valid_entry is returned.
 */

static entry_found get_compact_entry( FILE *fp, directory_entry *entry )
{
    int     count;

    /* Get the defined_name_length. */

    count = fgetc( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        return( not_valid_entry );
    }

    /* Ensure the defined_name_length is not too long for the buffer. */

    if( count > DEFINED_NAME_MAX ) {
        return( not_valid_entry );
    }

    /* Get the defined_name. An empty value is allowed; see the Wiki. */

    if( count > 0 ) {
        fread( entry->defined_name, 1, count, fp );
        if( ferror( fp )
          || feof( fp ) ) {
            return( not_valid_entry );
        }
    }
    entry->defined_name[count] = '\0';

    /* Get the member_name_length. */

    count = fgetc( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        return( not_valid_entry );
    }

    /* Ensure the member_name_length is not zero or too long for the buffer. */

    if( (count == 0)
      || (count >= _MAX_PATH) ) {
        return( not_valid_entry );
    }

    /* Get the member_name. */

    fread( entry->member_name, 1, count, fp );
    if( ferror( fp )
      || feof( fp ) ) {
        return( not_valid_entry );
    }
    entry->member_name[count] = '\0';

    return( valid_entry );
}

/* Function get_extended_entry().
 * Reads and returns the current (extended) entry.
 *
 * Parameters:
 *      fp points to the directory file.
 *      entry is intended to contain the current entry.
 *
 * Modified Parameter:
 *      entry will be modified as indicated below.
 *
 * Returns:
 *      valid_entry if both values of entry were updated.
 *      not_valid_entry if only one of the values or neither of the values
 *          of entry was updated.
 *
 * Notes:
 *      The defined_name and the embedded_name fields will be terminated
 *          with '\0' if updated.
 *      A file error may have occurred even if valid_entry is returned.
 */

static entry_found get_extended_entry( FILE *fp, directory_entry *entry )
{
    int     count;

    /* Get the defined_name_length. */

    count = fgetc( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        return( not_valid_entry );
    }

    /* Ensure the defined_name_length is not too long for the buffer. */

    if( count > DEFINED_NAME_MAX ) {
        return( not_valid_entry );
    }

    /* Get the defined_name. An empty value is allowed; see the Wiki. */

    if( count > 0 ) {
        fread( entry->defined_name, 1, count, fp );
        if( ferror( fp )
          || feof( fp ) ) {
            return( not_valid_entry );
        }
    }
    entry->defined_name[count] = '\0';

    /* Skip the marker. */

    fseek( fp, sizeof( uint16_t ), SEEK_CUR );
    if( ferror( fp )
      || feof( fp ) ) {
        return( not_valid_entry );
    }

    /* Get the the member_name_length. */

    count = fgetc( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        return( not_valid_entry );
    }

    /* Ensure the member_name_length is not zero or too long for the buffer. */

    if( (count == 0)
      || (count > _MAX_PATH) ) {
        return( not_valid_entry );
    }

    /* Get the member_name. */

    fread( entry->member_name, 1, count, fp );
    if( ferror( fp )
      || feof( fp ) ) {
        return( not_valid_entry );
    }
    entry->member_name[count] = '\0';

    /* Skip the preview. */

    fseek( fp, sizeof( uint16_t ), SEEK_CUR );
    if( ferror( fp )
      || feof( fp ) ) {
        return( valid_entry );
    }
    return( valid_entry );
}

/* Function get_member_name().
 * Searches the given directory file for the defined name. If the defined name
 * is found, returns the corresponding member name.
 *
 * Parameter:
 *      fp contains the FILE * for the directory file.
 *      in_name points to the defined name to match.
 *
 * Globals Used:
 *      try_file_name contains the name of the directory file.
 *
 * Returns:
 *      on success, the corresponding member name.
 *      on failure, a NULL pointer.
 *
 * Note:
 *      the comparison is not case-sensitive for compatability with wgml 4.0.
 */

char *get_member_name( FILE *fp, char const *in_name )
{
    char            *member_name = NULL;
    cop_file_type   file_type;
    directory_entry current_entry;
    entry_found     entry_status;
    uint16_t        entry_type;

    /* See if in_name is found in try_file_name. */

    file_type = parse_header( fp );
    switch( file_type ) {
    case file_error:

        /* File error, including premature eof. */

        xx_simple_err_c( err_dev_lib_file, try_file_name );
        break;

    case not_se_v4_1:

        /* File was created by a different version of gendev. */

        xx_simple_err( err_wrong_gendev );
        break;

    case not_bin_dev:
    case se_v4_1_not_dir:

        /* Wrong type of file: something is wrong with the device library. */

        xx_simple_err_c( err_dev_lib_data, try_file_name );
        break;

    case dir_v4_1_se:

        /* fp was a same-endian version 4.1 directory file. */

        /* Skip the number of entries. */

        fseek( fp, sizeof( uint32_t ), SEEK_CUR );
        if( ferror( fp )
          || feof( fp ) ) {
            break;
        }

        for( ;; ) {

            /* Get the entry_type. This is either the type or the metatype,
             * depending on whether this is a CompactDirEntry or an
             * ExtendedDirEntry.
             */

            fread( &entry_type, 1, sizeof( entry_type ), fp );

            /* Exit the loop when the final entry has been processed. */

            if( feof( fp )
              || ferror( fp ) ) {
                break;
            }

            switch( entry_type) {
            case 0x0000:

                /* This should only happen when the end-of-file padding is
                 * reached, but continue in case there is more data.
                 */

                continue;

            case 0x0001:

            /* This will be an ExtendedDirEntry. */

                for( ;; ) {

                    /* Get the entry_type. This is always the type, since the
                     * metatype has already been read.
                     */

                    fread( &entry_type, 1, sizeof( entry_type ), fp );

                    /* Exit the loop when the final entry has been processed. */

                    if( feof( fp )
                      || ferror( fp ) ) {
                        break;
                    }

                    switch( entry_type ) {
                    case 0x0000:

                        /* This should only happen when the end-of-file padding is
                         * reached, but continue in case there is more data.
                         */

                        continue;

                    case 0x0001:

                        /* This should never actually occur; however, continue
                         * in case there is more data.
                         */

                        continue;

                    case 0x0101:
                    case 0x0201:
                    case 0x0401:

                        /* For any type, check the defined name. */

                        entry_status = get_extended_entry( fp, &current_entry );
                        switch( entry_status ) {
                        case valid_entry:

                            /* Return the member name, if found. */

                            if( stricmp( in_name, current_entry.defined_name ) == 0 ) {
                                return( mem_strdup( current_entry.member_name ) );
                            }

                            break;

                        case not_valid_entry:

                            break;

                        default:

                            /* The entry_status is an unknown value. */

                            internal_err( __FILE__, __LINE__ );
                            break;
                        }
                        break;

                    default:

                        /* The entry_type is an unknown value. */

                        internal_err( __FILE__, __LINE__ );
                        break;
                    }
                    break;
                }
                break;

            case 0x0101:
            case 0x0201:
            case 0x0401:

                /* For any type, check the defined name. */

                entry_status = get_compact_entry( fp, &current_entry );
                switch( entry_status ) {
                case valid_entry:

                    /* Return the member name, if found. */

                    if( stricmp( in_name, current_entry.defined_name ) == 0 ) {
                        return( mem_strdup( current_entry.member_name ) );
                    }
                    break;
                case not_valid_entry:
                    break;
                default:

                    /* The entry_status is an unknown value. */

                    internal_err( __FILE__, __LINE__ );
                    break;
                }
                break;

            default:

                /* The entry_type is an unknown value. */

                internal_err( __FILE__, __LINE__ );
                break;
            }
        }

        break;

    default:

        /* The file_type is an unknown value. */

        internal_err( __FILE__, __LINE__ );
        break;
    }

    return( member_name );
}
