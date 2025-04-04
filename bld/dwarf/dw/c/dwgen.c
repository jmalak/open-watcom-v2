/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Manage compile units and global debug information.
*
****************************************************************************/


#include "dwpriv.h"
#include "dwinfo.h"
#include "dwline.h"
#include "dwmac.h"
#include "dwcliuti.h"
#include "dwabbrev.h"
#include "dwtype.h"
#include "dwdie.h"
#include "dwarange.h"
#include "dwloc.h"
#include "dwdecl.h"
#include "dwrefer.h"
#include "dwmem.h"
#include "dwhandle.h"
#include "dwname.h"


dw_handle DWENTRY DWBeginCompileUnit( dw_client cli, dw_cu_info *cu )
{
    dw_handle                   new;
    uint                        tmp;
    abbrev_code                 abbrev;

    {/* remember where all the sections start */
        dw_sectnum                  sect;

        for( sect = 0; sect < DW_DEBUG_MAX; ++sect ) {
            if( (cli->compiler_options & DW_CM_ABBREV_PRE) && sect == DW_DEBUG_ABBREV ) {
                cli->section_base[sect] = 0;
            } else {
                cli->section_base[sect] = CLISectionAbs( cli, sect );
            }
        }
    }
    cli->offset_size = cu->offset_size;
    cli->segment_size = cu->segment_size;
    cli->dbg_pch = cu->dbg_pch;
    cli->defset = 0;
    DW_InitDebugPubnames( cli );
    DW_InitDebugAranges( cli );
    DW_InitReferences( cli );      // will this screw up AT_WATCOM_ref_start?
    DW_InitDebugInfo( cli );
    DW_InitHandles( cli );
    DW_InitDecl( cli );
    DW_InitDIE( cli );

    /* start the tree */
    DW_StartChildren( cli );

    /* get the compile unit handle */
    new = DW_LabelNewHandle( cli );

    /* emit the TAG_compile_unit */
    abbrev = AB_COMPILE_UNIT | AB_SIBLING;
    if( cu->flags != 0 ) {
        abbrev |= AB_LOWHIGH_PC;
    }
    DW_StartDIE( cli, abbrev );
    if( abbrev & AB_LOWHIGH_PC ) {
        /* AT_low_pc */
        DW_InfoReloc( cli, DW_W_LOW_PC );
        /* AT_high_pc */
        DW_InfoReloc( cli, DW_W_HIGH_PC );
    }
    /* AT_name */
    DW_InfoString( cli, cu->source_filename );
    /* AT_stmt_list */
    CLIReloc3( cli, DW_DEBUG_INFO, DW_W_SECTION_POS, DW_DEBUG_LINE );
    /* AT_language */
    DW_InfoULEB128( cli, cli->language );
    /* AT_comp_dir */
    DW_InfoString( cli, cu->directory );
    /* AT_producer */
    tmp = sizeof( DWARF_WATCOM_PRODUCER ) - 1;
    if( cli->producer_name[0] != '\0' )
        ++tmp;
    DW_InfoBytes( cli, DWARF_WATCOM_PRODUCER " ", tmp );
    DW_InfoString( cli, cli->producer_name );
    /* AT_identifier_case */
    tmp = DW_ID_case_sensitive;
    if( cli->compiler_options & DW_CM_UPPER ) {
        tmp = DW_ID_up_case;
    } else if( cli->compiler_options & DW_CM_LOWER ) {
        tmp = DW_ID_down_case;
    }
    DW_InfoULEB128( cli, tmp );
    /* AT_macro_info */
    CLIReloc3( cli, DW_DEBUG_INFO, DW_W_SECTION_POS, DW_DEBUG_MACINFO );
    /* AT_base_types */
    if( cli->dbg_pch != NULL ) { // want start of ccu in pch
        CLIReloc4( cli, DW_DEBUG_INFO, DW_W_EXT_REF, cli->dbg_pch, sizeof( compuhdr_prologue ) );
    } else {  // 0 no pch
        DW_Info32( cli, 0  );
    }
    /* AT_WATCOM_memory_model */
    DW_Info8( cli, cu->model );
    /* AT_WATCOM_references_start */
    CLIReloc3( cli, DW_DEBUG_INFO, DW_W_SECTION_POS, DW_DEBUG_REF );
    DW_EndDIE( cli );
    DW_StartChildren( cli );
/*
 * these are done now so that the references to their starting point
 * actually point to the start, rather than midway through
*/
    DW_InitDebugLine( cli, cu->source_filename, cu->inc_list, cu->inc_list_len );
    DW_InitDebugMacInfo( cli );
    return( new );
}


void DWENTRY DWEndCompileUnit( dw_client cli )
{
    DW_EndChildren( cli );
    DW_EndChildren( cli );

    DW_FiniDIE( cli );
    DW_FiniDecl( cli );
    DW_FiniReferences( cli );
    DW_FiniHandles( cli );
    DW_FiniDebugInfo( cli );
    DW_FiniDebugMacInfo( cli );
    DW_FiniDebugLine( cli );
    DW_FiniDebugAranges( cli );
    DW_FiniDebugPubnames( cli );
    cli->offset_size = 0;
    cli->segment_size = 0;
}


dw_client DWENTRY DWInit( const dw_init_info *info )
{
    dw_client                   cli;

    if( info == NULL )
        return( NULL );

    cli = CLIAlloc( info, sizeof( struct dw_client ) );

    /* copy some parms */
    cli->funcs = info->funcs;
    memcpy( &cli->exception_handler, &info->exception_handler, sizeof( info->exception_handler ) );
    cli->producer_name = DW_StrDup( cli, info->producer_name );
    cli->compiler_options = info->compiler_options;
    cli->language = info->language;
    cli->abbrev_sym = info->abbrev_sym;

    DW_InitDebugAbbrev( cli );
    DW_InitDebugLoc( cli );
    if( cli->compiler_options & DW_CM_ABBREV_GEN ) {
        DW_GenAllAbbrev( cli );
    }
    return( cli );
}


void DWENTRY DWFini( dw_client cli )
{
    DW_FiniDebugLoc( cli );
    if( (cli->compiler_options & DW_CM_ABBREV_PRE) == 0 ) {
        DW_FiniDebugAbbrev( cli );
    }
    CLIFree( cli, cli->producer_name );
    CLIFree( cli, cli );
}

void DWENTRY DWInitDebugLine( dw_client cli, dw_cu_info *cu )
{
    cli->offset_size = cu->offset_size;
    cli->segment_size = cu->segment_size;
    cli->dbg_pch = cu->dbg_pch;
    cli->defset = 0;
    cli->section_base[DW_DEBUG_LINE] = CLISectionAbs( cli, DW_DEBUG_LINE );
    DW_InitDebugLine( cli, cu->source_filename, cu->inc_list, cu->inc_list_len );
}

void DWENTRY DWFiniDebugLine( dw_client cli )
{
    DW_FiniDebugLine( cli );
}
