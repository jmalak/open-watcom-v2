/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Declares functions used to manipulate .COP device files:
*                   is_dev_file()
*                   parse_device()
*
****************************************************************************/

#ifndef COPDEV_H_INCLUDED
#define COPDEV_H_INCLUDED

#include <stdio.h>

/* Function declarations */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern bool            is_dev_file( FILE * in_file );
extern cop_device  *   parse_device( FILE * in_file );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* COPDEV_H_INCLUDED */
