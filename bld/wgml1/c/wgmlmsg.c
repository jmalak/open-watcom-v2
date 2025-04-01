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
* Description:  WGML message definition.
*               adapted from wlink file (bld\wl\c\wlnkmsg.c)
*
****************************************************************************/


#include <fcntl.h>
#include "wgml.h"

#include "wreslang.h"
#if defined( USE_WRESLIB )
    #include "wressetr.h"
    #include "wresset2.h"
#else
    #include <windows.h>
#endif
#include "wgmlmsgs.rh"

#include "clibint.h"
#include "clibext.h"


#if defined( USE_WRESLIB )
HANDLE_INFO      Instance = { 0 };
#else
HINSTANCE        Instance;
#endif

static unsigned MsgShift;               // 0 = english, 1000 for japanese


/***************************************************************************/
/*  Special seek routine, because resource file does not start at offset 0 */
/*  of wgml.exe                                                            */
/***************************************************************************/

#if 0
static off_t WGMLResSeek( int handle, off_t position, int where )
/***************************************************************/
/* Workaround wres bug */
{
    if( ( where == SEEK_SET ) && ( handle == WGMLItself ) ) {
        return( lseek( handle, position + WResFileShift, where ) - WResFileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

WResSetRtns( open, close, read, write, WGMLResSeek, tell, mem_alloc, mem_free );
#endif

/***************************************************************************/
/*  initialize messages from resource file                                 */
/***************************************************************************/

bool init_msgs( void )
{
#ifdef USE_WRESLIB
    char        fname[_MAX_PATH];

    Instance.status = 0;
    if( _cmdname( fname ) != NULL && OpenResFile( &Instance, fname ) ) {
        MsgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( get_msg( ERR_DUMMY, fname, sizeof( fname ) ) ) {
            return( true );
        }
    }
    CloseResFile( &Instance );
    xx_simple_err( err_res_not_found );
    return( false );
#else
    Instance = GetModuleHandle( NULL );
    MsgShift = _WResLanguage() * MSG_LANG_SPACING;
    return( true );
#endif
}


/***************************************************************************/
/*  get a msg text string                                                  */
/***************************************************************************/

bool get_msg( msg_ids resid, char *buff, size_t buff_len )
{
#ifdef USE_WRESLIB
    if( Instance.status == 0
      || WResLoadString( &Instance, resid + MsgShift, buff, (int)buff_len ) <= 0 ) {
        buff[0] = '\0';
        return( false );
    }
#else
    if( LoadString( Instance, resid + MsgShift, buff, (int)buff_len ) <= 0 ) {
        buff[0] = '\0';
        return( false );
    }
#endif
    return( true );
}

/***************************************************************************/
/*  end of msg processing                                                  */
/***************************************************************************/

void fini_msgs( void )
{
#ifdef USE_WRESLIB
    CloseResFile( &Instance );
#else
#endif
}
