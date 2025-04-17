/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Declares structs and functions used to manipulate .COP
*               FunctionsBlocks:
*                   p_buffer
*                   functions_block
*                       code_block
*                   get_code_blocks()
*                   get_p_buffer()
*                   parse_functions_block()
*
* Note:         The field names are intended to correspond to the field names
*               shown in the Wiki. The Wiki structs are named when the structs
*               defined here are defined; they are not identical.
*
****************************************************************************/

#ifndef COPFUNC_H_INCLUDED
#define COPFUNC_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

#define SEEK_POSBACK(x)     (-(long)(x))

/* Struct declarations. */

/* This holds the raw contents of one or more contiguous P-buffers. The
 * buffer is to be interpreted as an array of count uint8_t value. The
 * value of count should always be a multiple of 80.
 */

typedef struct {
    uint16_t        count;
    const char *    buffer;
} p_buffer;

/* To hold the data extracted from the CodeBlock struct. This is the CodeBlock
 * discussed in the Wiki with the flag fields omitted and the field
 * cumulative_index added for use when parsing the DeviceFile struct.
 */

typedef struct {
    uint8_t     designator;
    uint16_t    line_pass;
    uint16_t    count;
    uint16_t    cumulative_index;
    const char  *text;
} code_block;

/* To hold the data extracted from the Variant A FunctionsBlock struct. */

typedef struct {
    uint16_t        count;
    code_block *    code_blocks;
} functions_block;

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern code_block       *get_code_blocks( const char **position, uint16_t count, const char *base );
extern p_buffer         *get_p_buffer( FILE *file );
extern functions_block  *parse_functions_block( const char **position, const char *base );

extern unsigned char    fread_u8( FILE *fp );
extern unsigned short   fread_u16( FILE *fp );
extern unsigned long    fread_u32( FILE *fp );
extern size_t           fread_buff( void *buff, size_t len, FILE *fp );
extern unsigned char    get_u8( const char **buff );
extern unsigned short   get_u16( const char **buff );
extern unsigned long    get_u32( const char **buff );
extern void             get_buff( void *obuff, size_t len, const char **buff );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* COPFUNC_H_INCLUDED */
