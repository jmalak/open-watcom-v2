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
    file_type_dir,          // The file is directory file.
    file_type_dev,          // The file is device.
    file_type_drv,          // The file is driver.
    file_type_fon,          // The file is font.
    file_type_unknown,      // The file is unknown type.
    file_type_wrong_ver,    // The file is not same-endian and/or not version 4.1.
    file_type_error,        // An error occurred while reading the file.
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
