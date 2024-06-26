/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Binary patch message output.
*
****************************************************************************/


#include "bdiff.h"
#ifdef __WATCOMC__
    #include <process.h>
#endif
#include "wreslang.h"
#include "msg.h"
#ifdef USE_WRESLIB
    #include "wressetr.h"
    #include "wresset2.h"
#else
    #include <windows.h>
#endif

#include "clibext.h"


#ifdef USE_WRESLIB
static HANDLE_INFO      hInstance = { 0 };
#else
static HINSTANCE        hInstance;
#endif
static unsigned         msgShift;

bool GetMsg( char *buffer, int resourceid )
{
#ifdef USE_WRESLIB
    if( hInstance.status == 0 || WResLoadString( &hInstance, resourceid + msgShift, (lpstr)buffer, MAX_RESOURCE_SIZE ) <= 0 ) {
        buffer[0] = '\0';
        return( false );
    }
#else
    if( LoadString( hInstance, resourceid + msgShift, buffer, MAX_RESOURCE_SIZE ) <= 0 ) {
        buffer[0] = '\0';
        return( false );
    }
#endif
    return( true );
}

bool MsgInit( void )
{
#ifdef USE_WRESLIB
    char        name[_MAX_PATH];
    char        msgbuf[MAX_RESOURCE_SIZE];

    hInstance.status = 0;
    if( _cmdname( name ) != NULL && OpenResFile( &hInstance, name ) ) {
        msgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( GetMsg( msgbuf, MSG_USAGE_BASE ) ) {
            return( true );
        }
    }
    CloseResFile( &hInstance );
    puts( NO_RES_MESSAGE );
    return( false );
#else
    hInstance = GetModuleHandle( NULL );
    msgShift = _WResLanguage() * MSG_LANG_SPACING;
    return( true );
#endif
}

static void OrderMsg( int order[], int num_arg, char *msg_ptr )
{
    int         i = 0;

    while( (msg_ptr = strpbrk( msg_ptr, "%")) != NULL ) {
        if( i >= num_arg )
            break;
        msg_ptr++;
        if( isdigit( *msg_ptr ) ) {
            order[i++] = atoi( msg_ptr ) - 1;
        }
        msg_ptr++;
    }
}

void MsgPrintf( int resourceid, va_list args )
{
    char        msgbuf[MAX_RESOURCE_SIZE];
    int         order[3] = { 0, 0, 0 };
    char        *argbuf[3];
    int         i;

    for( i = 0; i < 3; i++ ) {
        argbuf[i] = va_arg( args, char * );
    }
    GetMsg( msgbuf, resourceid );
    OrderMsg( order, 3, msgbuf );
    printf( msgbuf, argbuf[order[0]], argbuf[order[1]], argbuf[order[2]] );
}

void Message( int format, ... )
{
    va_list     args;

    va_start( args, format );
    MsgPrintf( format, args );
    va_end( args );
}

void MsgFini( void )
{
#ifdef USE_WRESLIB
    CloseResFile( &hInstance );
#else
#endif
}

static void Err( int format, va_list args )
{
    char        msgbuf[MAX_RESOURCE_SIZE];

    GetMsg( msgbuf, MSG_ERROR );
    printf( msgbuf );
    MsgPrintf( format, args);
}

void PatchError( int format, ... )
{
    va_list     args;

    va_start( args, format );
    Err( format, args );
    puts( "" );
    va_end( args );
}

void FilePatchError( int format, ... )
{
    va_list     args;
    int         err;

    va_start( args, format );
    err = errno;
    Err( format, args );
    printf( ": %s\n", strerror( err ) );
    va_end( args );
}

bool FileCheck( FILE *fd, const char *name )
{
    bool    ok;

    ok = ( fd != NULL );
    if( !ok ) {
        FilePatchError( ERR_CANT_OPEN, name );
    }
    return( ok );
}

bool SeekCheck( int ret, const char *name )
{
    bool    ok;

    ok = ( ret == 0 );
    if( !ok ) {
        FilePatchError( ERR_IO_ERROR, name );
    }
    return( ok );
}
