/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Declares functions used to manipulate .COP driver files:
*                   is_drv_file()
*                   parse_driver()
*
****************************************************************************/

#ifndef COPDRV_H_INCLUDED
#define COPDRV_H_INCLUDED

/* Function declarations */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern bool            is_drv_file( FILE * in_file );
extern cop_driver  *   parse_driver( FILE * in_file );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* COPDRV_H_INCLUDED */

