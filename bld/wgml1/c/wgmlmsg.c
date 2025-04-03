/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
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
