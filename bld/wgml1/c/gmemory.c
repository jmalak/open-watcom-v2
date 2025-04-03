/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
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

    static void prt( int * fhandle, const char * buff, size_t len )
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

