/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Declares an enum and a function which are used to parse
*               .COP file headers:
*                   cop_file_type
*                   parse_header()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
****************************************************************************/

#ifndef COPHDR_H_INCLUDED
#define COPHDR_H_INCLUDED

#include <stdio.h>

/* Enum definition. */

/* This enum is used for the return value of function parse_header(). */

typedef enum {
    dir_v4_1_se,        // The file is a same-endian version 4.1 directory file.
    se_v4_1_not_dir,    // The file is a same-endian version 4.1 device, driver, or font file.
    not_se_v4_1,        // The file is not same-endian and/or not version 4.1.
    not_bin_dev,        // The file is not a binary device file at all.
    file_error          // An error occurred while reading the file.
} cop_file_type;

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern cop_file_type    parse_header( FILE * in_file );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* COPHDR_H_INCLUDED */
