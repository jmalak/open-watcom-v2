/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Declares functions used to manipulate .COP font files:
*                   is_fon_file()
*                   parse_font()
*
****************************************************************************/

#ifndef COPFON_H_INCLUDED
#define COPFON_H_INCLUDED

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern bool            is_fon_file( FILE * in_file );
extern cop_font    *   parse_font( FILE * in_file, char const * in_name );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* COPFON_H_INCLUDED */
