/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Declares the public interface to the output buffer (and so to
*               the output file or device).
*                   ob_flush()
*                   ob_insert_block()
*                   ob_insert_byte()
*                   ob_insert_ps_text_start()
*                   ob_setup()
*                   ob_teardown()
*
****************************************************************************/

#ifndef OUTBUFF_H_INCLUDED
#define OUTBUFF_H_INCLUDED

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern void ob_flush( void );
extern void ob_insert_block( const char *in_block, size_t count, bool out_trans, bool out_text, font_number font );
extern void ob_insert_byte( unsigned char in_char );
extern void ob_insert_ps_text_end( bool htab_done, font_number font );
extern void ob_insert_ps_text_start( void );
extern void ob_setup( void );
extern void ob_teardown( void );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* OUTBUFF_H_INCLUDED */
