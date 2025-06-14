/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2007 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WGML memory management routines
*               uses project bld\trmem memory tracker code  if TRMEM defined
*
****************************************************************************/


#include "wgml.h"


#ifdef TRMEM

    #include "trmem.h"

    #define CRLF            "\n"

    static  _trmem_hdl  handle;             // memory tracker anchor block

    /***********************************************************************/
    /*  Memory tracker output function                                     */
    /***********************************************************************/

    static void prt( void * fhandle, const char * buff, size_t len )
    {
        size_t i;

        fhandle = fhandle;
        for( i = 0; i < len; ++i ) {
    //      fputc( *buff++, stderr );
            fputc( *buff++, stdout );   // use stdout for now (easier redirection)
        }
    }

#endif


/***************************************************************************/
/*  init memorytracker                                                     */
/***************************************************************************/

void mem_init( void )
{
#ifdef TRMEM
    handle = _trmem_open( &malloc, &free, &realloc, NULL, NULL, &prt,
                          _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
                          _TRMEM_REALLOC_NULL | _TRMEM_FREE_NULL |
                          _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
#endif
}

/***************************************************************************/
/*  display current memory usage                                      */
/***************************************************************************/

void mem_prt_curr_usage( void )
{
#ifdef  TRMEM
    _trmem_prt_usage( handle );
#endif
}

/***************************************************************************/
/*  display peak memory usage                                      */
/***************************************************************************/

unsigned long mem_get_peak_usage( void )
{
#ifdef  TRMEM
    return( _trmem_get_peak_usage( handle ) );
#else
    return( 0 );
#endif
}


/***************************************************************************/
/*  get banner text if trmem is compiled in wgml tool                      */
/***************************************************************************/

void mem_banner( void )
{
#ifdef  TRMEM
    out_msg( "Compiled with TRMEM memory tracker (trmem)" CRLF );
#endif
}


/***************************************************************************/
/*   memorytracker end processing                                          */
/***************************************************************************/

void mem_fini( void )
{
#ifdef TRMEM
    _trmem_prt_list( handle );
    _trmem_close( handle );
#endif
}

/***************************************************************************/
/*  Allocate some storage                                                  */
/***************************************************************************/

void *mem_alloc( size_t size )
{
    void    *p;

#ifdef TRMEM
    p = _trmem_alloc( size, _trmem_guess_who(), handle );
#else
    p = malloc( size );
#endif
    if( p == NULL ) {
        xx_simple_err( err_nomem_avail );
    }
    return( p );
}

/***************************************************************************/
/*  Re-allocate some storage                                               */
/***************************************************************************/

void *mem_realloc( void * oldp, size_t size )
{
    void    *   p;

#ifdef TRMEM
    p = _trmem_realloc( oldp, size, _trmem_guess_who(), handle );
#else
    p = realloc( oldp, size );
#endif
    if( p == NULL ) {
        xx_simple_err( err_nomem_avail );
    }
    return( p );
}


/***************************************************************************/
/*  duplicate string                                                  */
/***************************************************************************/

char *mem_strdup( const char *str )
{
    size_t  size;
    char    *p;

    if( str == NULL )
        str = "";
    size = strlen( str );
#ifdef TRMEM
    p = _trmem_alloc( size + 1, _trmem_guess_who(), handle );
#else
    p = malloc( size + 1 );
#endif
    if( p == NULL ) {
        xx_simple_err( err_nomem_avail );
    }
    return( strcpy( p, str ) );
}

/***************************************************************************/
/*  duplicate token                                                        */
/***************************************************************************/

char *mem_tokdup( const char *str, int size )
{
    char    *p;

#ifdef TRMEM
    p = _trmem_alloc( size + 1, _trmem_guess_who(), handle );
#else
    p = malloc( size + 1 );
#endif
    if( p == NULL ) {
        xx_simple_err( err_nomem_avail );
    }
    strncpy( p, str, size );
    p[size] = '\0';
    return( p );
}

/***************************************************************************/
/*  Free storage                                                           */
/***************************************************************************/

void mem_free( void * p )
{
#ifdef TRMEM
    _trmem_free( p, _trmem_guess_who(), handle );
#else
    free( p );
#endif
    p = NULL;
}

/* These functions were added for use in debugging */

/***************************************************************************/
/*  memorytracker validate allocated storage                               */
/***************************************************************************/

int mem_validate( void )
{
#ifdef TRMEM
    return(_trmem_validate_all( handle ));
#endif
    return 1;   // always succeed if trmem not in use
}


/***************************************************************************/
/*   memorytracker print list                                              */
/***************************************************************************/

void mem_prt_list( void )
{
#ifdef TRMEM
    _trmem_prt_list( handle );
#endif
}

