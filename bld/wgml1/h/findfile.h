/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Declares everything used by wgml to find and open files:
*                   dirseq
*                   ff_setup()
*                   ff_set_incpath()
*                   ff_set_libpath()
*                   ff_teardown()
*                   search_file_in_dirs()
*                   try_file_name
*
****************************************************************************/

#ifndef FINDFILE_H_INCLUDED
#define FINDFILE_H_INCLUDED

#include <stdio.h>

/* Extern variable declaration. */

extern char         try_file_name[];

/* Extern enum declaration. */

/* Search sequences for various types of files. */

typedef enum {
    ds_opt_file = 1,    // wgml option files:     curdir, gmllib, gmlinc, path
    ds_doc_spec,        // wgml document source:  curdir, gmlinc, gmllib, path
    ds_bin_lib,         // wgml binary library:           gmllib, gmlinc, path
    ds_lib_src          // gendev source:         curdir, gmlinc
} dirseq;

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern  void        ff_teardown( void );
extern  void        ff_set_incpath( const char * );
extern  void        ff_set_libpath( const char * );
extern  void        ff_setup( void );
extern  FILE        *search_file_in_dirs( const char *filename, const char *defext, const char *altext, dirseq seq );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* FINDFILE_H_INCLUDED */
