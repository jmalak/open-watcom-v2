/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
* Description:  Implementation of far _heapmin() and _fheapmin()
*                   (16-bit code only)
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"


#if defined(__BIG_DATA__)

_WCRTLINK int _heapshrink( void )
{
    return( _fheapshrink() );
}

_WCRTLINK int _heapmin( void )
{
    return( _fheapshrink() );
}

#endif

_WCRTLINK int _fheapmin( void )
{
    return( _fheapshrink() );
}

_WCRTLINK int _fheapshrink( void )
{
    __segment   curr_seg;
    __segment   next_seg;
    __segment   prev_seg;
    int         heap_status;
    heapblk     _WCFAR *curr_heap;
    heapblk     _WCFAR *next_heap;
    heapblk     _WCFAR *prev_heap;

    heap_status = __HeapMin( __fheapbeg, 0 );
    _AccessFHeap();
    for( curr_seg = __fheapbeg; curr_seg != _NULLSEG; curr_seg = next_seg) {
        curr_heap = MK_FP( curr_seg, 0 );
        next_seg = curr_heap->nextseg;
        /* we might free this segment so get the next one now */
        if( curr_heap->numalloc == 0 ) {     /* empty heap */
            /* unlink from heap list */
            prev_seg = curr_heap->prevseg;
            if( next_seg != _NULLSEG ) {
                next_heap = MK_FP( next_seg, 0 );
                next_heap->prevseg = prev_seg;
            }
            if( prev_seg == _NULLSEG ) {
                __fheapbeg = next_seg;
            } else {
                prev_heap = MK_FP( prev_seg, 0 );
                prev_heap->nextseg = next_seg;
            }
            __fheapRover = __fheapbeg;
            heap_status = __FreeSeg( curr_seg );
        }
    }
    _ReleaseFHeap();
    return( heap_status );
}
