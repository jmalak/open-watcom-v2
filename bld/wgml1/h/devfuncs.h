/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Declares the functions used to interpret function blocks:
*                   df_increment_pages()
*                   df_interpret_device_functions()
*                   df_interpret_driver_functions()
*                   df_new_section()
*                   df_populate_device_table()
*                   df_populate_driver_table()
*                   df_setup()
*                   df_teardown()
*                   fb_binclude_support()
*                   fb_empty_text_line()
*                   fb_enterfont()
*                   fb_first_text_line_pass()
*                   fb_graphic_support()
*                   fb_init()
*                   fb_line_block()
*                   fb_lineproc_endvalue()
*                   fb_subsequent_text_line_pass()
****************************************************************************/

#ifndef DEVFUNCS_H_INCLUDED
#define DEVFUNCS_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif
extern void df_start_page( void );
extern void df_interpret_device_functions( const char *in_function );
extern void df_interpret_driver_functions( const char *in_function );
extern void df_new_section( uint32_t v_start );
extern void df_populate_device_table( void );
extern void df_populate_driver_table( void );
extern void df_start_page( void );
extern void df_setup( void );
extern void df_teardown( void );
extern void fb_binclude_support( binclude_element * in_el );
extern void fb_empty_text_line( text_line * out_line );
extern void fb_enterfont( void );
extern void fb_first_text_line_pass( text_line * out_line );
extern void fb_graphic_support( graphic_element * in_el );
extern void fb_init( init_block * in_block );
extern void fb_line_block( line_block * in_line_block, uint32_t h_start, uint32_t v_start, uint32_t h_len, uint32_t v_len, bool twice );
extern void fb_lineproc_endvalue( void );
extern void fb_subsequent_text_line_pass( text_line * out_line, uint16_t line_pass );
#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* DEVFUNCS_H_INCLUDED */
