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
* Description:  Emit OMF object records.
*
****************************************************************************/


#include "_cgstd.h"
#include <time.h>
#include "coderep.h"
#include "data.h"
#include "cgauxcc.h"
#include "cgauxinf.h"
#include "system.h"
#include "cgmem.h"
#include "zoiks.h"
#include "rtrtn.h"
#include "x86objd.h"
#include "utils.h"
#include "objout.h"
#include "cvdbg.h"
#include "dbsyms.h"
#include "cvsyms.h"
#include "dw.h"
#include "dfsyms.h"
#include "dfsupp.h"
#include "wvsyms.h"
#include "wvtypes.h"
#include "objio.h"
#include "onexit.h"
#include "optmain.h"
#include "opttell.h"
#include "intrface.h"
#include "x86obj.h"
#include "i87data.h"
#include "dostimet.h"
#include "cgsegids.h"
#include "feprotos.h"


#define _NIDX_NULL      1   /* lname "" */
#define _NIDX_CODE      2   /* lname "CODE" */
#define _NIDX_DATA      3   /* lname "DATA" */
#define _NIDX_BSS       4   /* lname "BSS" */
#define _NIDX_TLS       5   /* lname "TLS" */

#define MODEST_HDR      50
#define INCREMENT_HDR   50

#define MODEST_INFO     5
#define INCREMENT_INFO  5

#define MODEST_IMP      BUFFSIZE
#define INCREMENT_IMP   50

#define MODEST_LINE     BUFFSIZE
#define INCREMENT_LINE  200

#define MODEST_OBJ      BUFFSIZE
#define INCREMENT_OBJ   256

#define NOMINAL_FIX     20
#define INCREMENT_FIX   50

#define MODEST_EXP      BUFFSIZE
#define INCREMENT_EXP   50

#define MODEST_PAT      10
#define INCREMENT_PAT   10


#define _CopyTrans( src, dst, len )        Copy( src, dst, len )

#define _ARRAY( what, type )    (*(type *)((char *)(what)->array + (what)->used))
#define _ARRAYOF( what, type )  ((type *)(what)->array)
#define _CHGTYPE( what, type )  (*(type *)&(what))

#define altCodeSegId    codeSegId

#define OMFNAMELEN(s)   *(unsigned char *)(s)

typedef struct lname_cache {
    struct lname_cache  *next;
    omf_idx             idx;
    char                name[1];        /* var sized, first byte is length */
} lname_cache;

typedef struct virt_func_ref_list {
    struct virt_func_ref_list   *next;
    void                        *cookie;
} virt_func_ref_list;

typedef struct dbg_seg_info {
    segment_id          *segid;
    const char          *seg_name;
    const char          *class_name;
} dbg_seg_info;


static  bool            GenStaticImports;
static  omf_idx         ImportHdl;
static  array_control   Imports;
static  array_control   SegInfo;
static  abs_patch       *AbsPatches;
static  segment_id      codeSegId = BACKSEGS;
static  segment_id      dataSegId;
static  segdef          *SegDefs;
static  long_offset     CodeSize;
static  long_offset     DataSize;
static  long_offset     DbgTypeSize;
static  index_rec       *CurrSeg;
static  omf_idx         GroupIndex;
static  omf_idx         DGroupIndex;
static  omf_idx         SegmentIndex;
static  omf_idx         PrivateIndexRW;
static  omf_idx         PrivateIndexRO;
static  omf_idx         CodeGroupGIdx;
static  omf_idx         CodeGroupNIdx;
static  char            CodeGroup[80];
static  char            DataGroup[80];
static  offset          SelStart;
static  omf_idx         selIdx;
static  segment_id      backSegId = BACKSEGS;
static  omf_idx         FPPatchImp[FPP_NUMBER_OF_TYPES];
static  int             segmentCount;
static  bool            NoDGroup;
static  short           CurrFNo;
#if _TARGET & _TARG_80386
static  omf_idx         FlatGIndex;
static  omf_idx         FlatNIndex;
#endif
static  omf_idx         TLSGIndex;

static  omf_idx         NameIndex;
static  lname_cache     *NameCache;
static  lname_cache     *NameCacheDumped;

static const char * const FPPatchName[] = {
    #define pick_fp(enum,name,alt_name,win,alt_win,others,alt_others) name,
    #include "fppatche.h"
    #undef pick_fp
};

static const char * const FPPatchAltName[] = {
    #define pick_fp(enum,name,alt_name,win,alt_win,others,alt_others) alt_name,
    #include "fppatche.h"
    #undef pick_fp
};

static const struct dbg_seg_info DbgSegs[] = {
    { &DbgLocals, "$$SYMBOLS", "DEBSYM" },
    { &DbgTypes,  "$$TYPES",   "DEBTYP" },
};


void    InitSegDefs( void )
/*************************/
{
    SegDefs = NULL;
    NameCache = NULL;
    NameCacheDumped = NULL;
    NameIndex = 0;
    GroupIndex = 0;
    DGroupIndex = 0;
    TLSGIndex = 0;
#if _TARGET & _TARG_80386
    FlatGIndex = 0;
#endif
    segmentCount = 0;
    codeSegId = BACKSEGS; /* just so it doesn't match a FE seg_id */
    backSegId = BACKSEGS;
}

static omf_idx GetNameIdx( const char *name, const char *suff )
/*************************************************************/
{
    lname_cache         **owner;
    lname_cache         *curr;
    size_t              len;
    char                idxname[256];

    len = strlen( name ) + strlen( suff );
    assert( len < 256 );
    if( len < 256 ) {
        strcpy( idxname, name );
        strcat( idxname, suff );
        for( owner = &NameCache; (curr = *owner) != NULL; owner = &curr->next ) {
            if( len == OMFNAMELEN( curr->name ) && strcmp( idxname, curr->name + 1 ) == 0 ) {
                return( curr->idx );
            }
        }
        curr = CGAlloc( sizeof( *curr ) + len + 1 );
        *owner = curr;
        curr->next = NULL;
        curr->idx = ++NameIndex;
        curr->name[0] = (unsigned char)len;
        strcpy( curr->name + 1, idxname );
        return( NameIndex );
    }
    return( 0 );
}

static  byte    DoSum( const byte *buff, size_t len )
/***************************************************/
{
    byte        sum;

    sum = 0;
    while( len > 0 ) {
        sum += *buff++;
        --len;
    }
    return( sum );
}

static void PutObjOMFRec( byte class, array_control *actl )
/*********************************************************/
{
    uint_16         blen;
    byte            cksum;

    if( actl->used > 0 ) {
        blen = _TargetShort( actl->used + 1 );
        cksum = class;
        cksum += DoSum( (const void *)&blen, sizeof( blen ) );
        cksum += DoSum( actl->array, actl->used );
        cksum = -cksum;
        PutObjBytes( &class, 1 );
        PutObjBytes( (const byte *)&blen, sizeof( blen ) );
        PutObjBytes( actl->array, actl->used );
        PutObjBytes( &cksum, 1 );
        actl->used = 0;
    }
}

static void PatchObj( objhandle rec, objoffset roffset, const byte *buff, size_t len )
/************************************************************************************/
{
    byte            cksum;
    uint_16         reclen;
    byte            inbuff[80];

    SeekGetObj( rec, 1, (byte *)&reclen, 2 );
    reclen = _HostShort( reclen );
    SeekGetObj( rec, roffset + 3, inbuff, len );
    SeekPutObj( rec, roffset + 3, buff, len );
    SeekGetObj( rec, 2 + reclen, &cksum, 1 );

    cksum += DoSum( inbuff, len );
    cksum -= DoSum( buff, len );

    SeekPutObj( rec, 2 + reclen, &cksum, 1 );
    NeedSeekObj();
}


static void FlushNames( void )
/****************************/
{
    /*
     * don't want to allocate memory because we might be in a low memory
     * situation
     */
    unsigned_8          buff[512];
    unsigned            len;
    lname_cache         *dmp;
    array_control       tmp;

    tmp.array = buff;
    tmp.used = 0;
    dmp = ( NameCacheDumped != NULL ) ? NameCacheDumped->next : NameCache;
    for( ; dmp != NULL; dmp = dmp->next ) {
        len = OMFNAMELEN( dmp->name );
        if( ( tmp.used + len ) > ( sizeof( buff ) - 1 ) ) {
            PutObjOMFRec( CMD_LNAMES, &tmp );
        }
        buff[tmp.used++] = len;
        _CopyTrans( dmp->name + 1, buff + tmp.used, len );
        tmp.used += len;
        NameCacheDumped = dmp;
    }
    if( tmp.used > 0 ) {
        PutObjOMFRec( CMD_LNAMES, &tmp );
    }
}

bool FreeObjCache( void )
/***********************/
{
    lname_cache         *tmp;

    if( NameCache == NULL )
        return( false );
    FlushNames();
    while( NameCache != NULL ) {
        tmp = NameCache->next;
        CGFree( NameCache );
        NameCache = tmp;
    }
    NameCacheDumped = NULL;
    return( true );
}


static  index_rec   *AskSegIndex( segment_id segid )
/**************************************************/
{
    unsigned    i;

    for( i = 0; i < SegInfo.used; ++i ) {
        if( _ARRAYOF( &SegInfo, index_rec )[i].segid == segid ) {
            return( _ARRAYOF( &SegInfo, index_rec ) + i );
        }
    }
    return( NULL );
}


static  void    ReallocArray( array_control *arr, unsigned need )
/***************************************************************/
{
    byte        *p;
    unsigned    new_len;

    new_len = arr->alloc;
    for( ;; ) {
        new_len += arr->inc;
        if( new_len >= need ) {
            break;
        }
    }
    p = CGAlloc( arr->entry * new_len );
    Copy( arr->array, p, arr->entry * arr->used );
    CGFree( arr->array );
    arr->array = p;
    arr->alloc = new_len;
}

static  byte    SegmentAttr( byte align, seg_attr tipe, bool use_16 )
/*******************************************************************/
{
    byte        attr;

    /* unused parameters */ (void)use_16;

    if( align <= 1 ) {
        attr = SEG_ALGN_BYTE;
    } else if( align <= 2 ) {
        attr = SEG_ALGN_WORD;
#if _TARGET & _TARG_80386
    } else if( align <= 4 ) {
        attr = SEG_ALGN_DWORD;
#endif
    } else if( align <= 16 ) {
        attr = SEG_ALGN_PARA;
    } else {
        attr = SEG_ALGN_PAGE;
#if 0
    /*
     * align is a byte - how can it be bigger than 4k - BBB
     */
  #if _TARGET & _TARG_80386
        if( _IsTargetModel( CGSW_X86_EZ_OMF ) ) {
            if( align > 256 ) {
                attr = SEG_ALGN_4K;
            }
        }
  #endif
#endif
    }
    if( tipe & COMMON ) {
        attr |= SEG_COMB_COMMON;
    } else if( (tipe & (PRIVATE | GLOBAL)) == PRIVATE ) {
        attr |= SEG_COMB_PRIVATE;
    } else { /* normal or a kludge for wsl front end ( PRIVATE | GLOBAL )*/
        attr |= SEG_COMB_NORMAL;
    }
#if _TARGET & _TARG_80386
    if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
        if( _IsTargetModel( CGSW_X86_USE_32 ) ) {
            if( !use_16 ) {
                attr |= SEG_USE_32;
            }
        }
    }
#endif
    return( attr );
}

static  void    SegmentClass( index_rec *rec )
/********************************************/
{
    char        *class_name;

    class_name = FEAuxInfo( (pointer)(pointer_uint)rec->segid, FEINF_CLASS_NAME );
    if( class_name != NULL ) {
        rec->cidx = GetNameIdx( class_name, "" );
    }
}

/* Array Control Routines*/

static void FillArray( array_control *res, unsigned size, unsigned starting, unsigned increment )
/***********************************************************************************************/
{
    res->array = CGAlloc( starting * size );
    res->alloc = starting;
    res->used = 0;
    res->entry = size;
    res->inc = increment;
}

static array_control *InitArray( unsigned size, unsigned starting, unsigned increment )
/**************************************************************************************
 * DO NOT call InitArray with size or starting value zero
 */
{
    array_control       *res;

    res = CGAlloc( sizeof( array_control ) );
    FillArray( res, size, starting, increment );
    return( res );
}


static  void    SetMaxWritten( void )
/***********************************/
{
    if( CurrSeg->max_written < CurrSeg->location ) {
        CurrSeg->max_written = CurrSeg->location;
    }
}


static void *allocOut( array_control *dest, unsigned size )
/*********************************************************/
{
    unsigned    need;

    need = dest->used + size;
    if( need > dest->alloc ) {
        ReallocArray( dest, need );
    }
    size = dest->used;
    dest->used = need;
    return( (char *)dest->array + dest->entry * size );
}


static void *allocOutData( unsigned size )
/****************************************/
{
    unsigned        need;
    unsigned        i;
    array_control   *obj_data;

    i = CurrSeg->location - CurrSeg->obj->start + CurrSeg->data_prefix_size;
    IncLocation( size );
    need = i + size;
    obj_data = &CurrSeg->obj->data;
    if( obj_data->used < need ) {
        if( need > obj_data->alloc ) {
            ReallocArray( obj_data, need );
        }
        obj_data->used = need;
    }
    SetMaxWritten();
    return( (char *)obj_data->array + obj_data->entry * i );
}


static  void    OutByte( byte value, array_control *dest )
/********************************************************/
{
    *(byte *)allocOut( dest, sizeof( byte ) ) = value;
}

static  void    OutShort( uint_16 value, array_control *dest )
/************************************************************/
{
    *(uint_16 *)allocOut( dest, sizeof( uint_16 ) ) = _TargetShort( value );
}

static  void    OutLongInt( uint_32 value, array_control *dest )
/**************************************************************/
{
    *(uint_32 *)allocOut( dest, sizeof( uint_32 ) ) = _TargetLongInt( value );
}

static  void    OutOffset( offset value, array_control *dest )
/************************************************************/
{
    *(offset *)allocOut( dest, sizeof( offset ) ) = _TargetOffset( value );
}

#if _TARGET & _TARG_8086
/*
 * 32bit debug seg support dwarf, codeview
 */
static  void    OutLongOffset( long_offset value, array_control *dest )
/*********************************************************************/
{
    *(long_offset *)allocOut( dest, sizeof( long_offset ) ) = _TargetLongInt( value );
}
#endif

static  void    OutIdx( omf_idx value, array_control *dest )
/**********************************************************/
{
    if( value >= 128 ) {
        OutByte( (value >> 8) | 0x80, dest );
    }
    OutByte( value, dest );
}

static  void    OutBuffer( const void *name, unsigned len, array_control *dest )
/******************************************************************************/
{
    _CopyTrans( name, allocOut( dest, len ), len );
}

static object *InitTarg( index_rec *rec )
/***************************************/
{
    object          *obj;

    obj = CGAlloc( sizeof( object ) );
    /*
     * try to bag all the memory we'll need now
     */
    FillArray( &obj->data, sizeof( byte ), MODEST_OBJ, INCREMENT_OBJ );
    FillArray( &obj->fixes, sizeof( byte ), NOMINAL_FIX, INCREMENT_FIX );
    FillArray( &obj->exports, sizeof( byte ), MODEST_EXP, INCREMENT_EXP );
    obj->lines = NULL;
    obj->tpatches = NULL;
    obj->gen_static_exports = false;
    obj->pending_line_number = 0;
    obj->last_line = 0;
    obj->last_offset = 0;
    obj->index = rec->sidx;
    obj->start = rec->location;
    return( obj );
}

static  void    DoASegDef( index_rec *rec, bool use_16 )
/******************************************************/
{
    object          *obj;
    omf_cmd         cmd;
    array_control   *obj_data;

    /* unused parameters */ (void)use_16;

    rec->obj = obj = InitTarg( rec );
    obj_data = &obj->data;
    OutByte( rec->attr, obj_data );
#if _TARGET & _TARG_80386
    OutOffset( 0, obj_data );           /* segment size (for now) */
#else /* SEG32DBG dwarf, codeview */
    if( rec->attr & SEG_USE_32 ) {
        OutLongOffset( 0, obj_data );   /* segment size (for now) */
    } else {
        OutOffset( 0, obj_data );       /* segment size (for now) */
    }
#endif
    OutIdx( rec->nidx, obj_data );      /* segment name index */
    OutIdx( rec->cidx, obj_data );      /* class name index */
    OutIdx( _NIDX_NULL, obj_data );     /* overlay name index */
#if _TARGET & _TARG_80386
    if( _IsTargetModel( CGSW_X86_EZ_OMF ) ) {
        if( _IsntTargetModel( CGSW_X86_USE_32 ) || use_16 ) {
            OutByte( 2, obj_data );     /* to indicate USE16 EXECUTE/READ */
        }
    }
#endif
    FlushNames();
    obj->segfix = AskObjHandle();
    if( ++segmentCount > 32000 ) {
        FatalError( "too many segments" );
    }
#if _TARGET & _TARG_80386
    if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
        cmd = CMD_SEGDEF32;
    } else {
        cmd = CMD_SEGDEF;
    }
#else /* SEG32DBG dwarf, codeview */
    if( (rec->attr & SEG_USE_32)
      && _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
        cmd = CMD_SEGDEF32;
    } else {
        cmd = CMD_SEGDEF;
    }
#endif
    PutObjOMFRec( cmd, obj_data );
    if( rec->exec ) {
        OutShort( LINKER_COMMENT, obj_data );
        OutByte( LDIR_OPT_FAR_CALLS, obj_data );
        OutIdx( rec->sidx, obj_data );
        PutObjOMFRec( CMD_COMENT, obj_data );
    }
    rec->location = 0;
}


static  void    OutCGroup( omf_idx sidx, array_control *dest )
/************************************************************/
{
    FlushNames();
    OutIdx( CodeGroupNIdx, dest );
    OutByte( GRP_SEGIDX, dest );
    OutIdx( sidx, dest );
    PutObjOMFRec( CMD_GRPDEF, dest );
}

static void OutGroup( omf_idx sidx, array_control *group_def, omf_idx *index_p )
/******************************************************************************/
{
    if( *index_p == 0 ) {
        *index_p = ++GroupIndex;
    }
    OutByte( GRP_SEGIDX, group_def );
    OutIdx( sidx, group_def );
}

segment_id  AskOP( void )
/***********************/
{
    segment_id  segid;

    if( CurrSeg == NULL ) {
        segid = UNDEFSEG;
    } else {
        segid = CurrSeg->segid;
    }
    return( segid );
}

segment_id  ChangeOP( segment_id segid )
/**************************************/
{
    segment_id  old_segid;

    if( CurrSeg == NULL ) {
        old_segid = UNDEFSEG;
    } else {
        old_segid = CurrSeg->segid;
    }
    if( segid == UNDEFSEG ) {
        CurrSeg = NULL;
    } else {
        CurrSeg = AskSegIndex( segid );
    }
    return( old_segid );
}

void    SetOP( segment_id segid )
/*******************************/
{
    if( segid == UNDEFSEG ) {
        CurrSeg = NULL;
    } else {
        CurrSeg = AskSegIndex( segid );
    }
}

static  index_rec   *AllocNewSegRec( void )
/*****************************************/
{
    index_rec   *rec;
    segment_id  segid;

    /*
     * CurrSeg pointer might have moved on us by allocOut
     * therefore we need to call AskOP before and SetOP after
     * to refresh CurrSeg pointer
     */
    segid = AskOP();
    rec = allocOut( &SegInfo, 1 );
    SetOP( segid );
    return( rec );
}

static void DoSegment( segdef *seg, array_control *dgroup_def, array_control *tgroup_def, bool use_16 )
/*****************************************************************************************************/
{
    index_rec   *rec;

    rec = AskSegIndex( seg->segid );
    if( rec == NULL ) {
        rec = AllocNewSegRec();
    }
    if( seg->attr & PRIVATE ) {
        rec->private = true;
    } else {
        rec->private = false;
    }
    rec->location = 0;
    rec->big = 0;
    rec->rom = 0;
    rec->need_base_set = true;
    rec->data_in_code = false;
    rec->start_data_in_code = false;
    rec->data_ptr_in_code = false;
    rec->prefix_comdat_state = PCS_OFF;
    rec->max_written = 0;
    rec->max_size = 0;
    rec->comdat_label = NULL;
    rec->comdat_symbol = NULL;
    rec->total_comdat_size = 0;
    rec->comdat_size = 0;
    rec->virt_func_refs = NULL;
    rec->data_prefix_size = 0;
    rec->comdat_prefix_import = NOT_IMPORTED;
    rec->sidx = ++SegmentIndex;
    rec->segid = seg->segid;
    rec->attr = SegmentAttr( seg->align, seg->attr, use_16 );
    if( seg->attr & EXEC ) {
        rec->exec = true;
        rec->rom = true;   /* code is alway ROM */
        rec->cidx = _NIDX_CODE;
        if( seg->attr & GIVEN_NAME ) {
            rec->nidx = GetNameIdx( seg->str, "" );
        } else if( CodeGroupGIdx != 0 ) {
            rec->nidx = GetNameIdx( CodeGroup, seg->str );
        } else if( _IsTargetModel( CGSW_X86_BIG_CODE ) ) {
            rec->nidx = GetNameIdx( FEModuleName(), seg->str );
        } else {
            rec->nidx = GetNameIdx( seg->str, "" );
        }
        if( CodeGroupGIdx != 0 ) {
            rec->base = CodeGroupGIdx;
            rec->btype = BASE_GRP;
        } else {
            rec->base = SegmentIndex;
            rec->btype = BASE_SEG;
        }
    } else {
        rec->exec = false;
        if( seg->attr & ROM )
            rec->rom = true;
        if( seg->attr & PRIVATE ) {
            rec->nidx = GetNameIdx( seg->str, "" );
            if( seg->attr & ROM ) {
                if( PrivateIndexRO == 0 ) {
                    PrivateIndexRO = GetNameIdx( "FAR_CONST", "" );
                }
                rec->cidx = PrivateIndexRO;
            } else {
                if( PrivateIndexRW == 0 ) {
                    PrivateIndexRW = GetNameIdx( "FAR_DATA", "" );
                }
                rec->cidx = PrivateIndexRW;
            }
            rec->base = SegmentIndex;
            rec->btype = BASE_SEG;
        } else {
            if( seg->attr & THREAD_LOCAL ) {
                if( TLSGIndex == 0 ) {
                    OutIdx( _NIDX_TLS, tgroup_def );
                }
                OutGroup( rec->sidx, tgroup_def, &TLSGIndex );
                rec->base = TLSGIndex;
                rec->btype = BASE_GRP;
            } else if( NoDGroup ) {
                rec->btype = BASE_SEG;
                rec->base = SegmentIndex;
            } else {
                OutGroup( rec->sidx, dgroup_def, &DGroupIndex );
                rec->base = DGroupIndex;
                rec->btype = BASE_GRP;
            }
            if( seg->attr & THREAD_LOCAL ) {
                rec->cidx = _NIDX_TLS;
            } else if( seg->attr & COMMON ) {
                rec->cidx = _NIDX_DATA;
            } else if( ( seg->attr & ( BACK | INIT ) ) == 0 ) {
                rec->cidx = _NIDX_BSS;
            } else {
                rec->cidx = _NIDX_DATA;
            }
            rec->nidx = GetNameIdx( DataGroup, seg->str );
        }
    }
    SegmentClass( rec );
    DoASegDef( rec, use_16 );
    if( (seg->attr & EXEC) && CodeGroupGIdx != 0 ) {
        OutCGroup( rec->sidx, &rec->obj->data );
    }
}

static segdef *AllocSegDefs( const char *str )
{
    segdef  *seg;
    segdef  **owner;

    seg = CGAlloc( sizeof( segdef ) );
    seg->next = NULL;
    seg->str = CGAlloc( Length( str ) + 1 );
    CopyStr( str, seg->str );
    for( owner = &SegDefs; *owner != NULL; ) {
        owner = &(*owner)->next;
    }
    *owner = seg;
    return( seg );
}

static void DoSegDefsAndFree( array_control *dgroup_def, array_control *tgroup_def, bool use_16 )
{
    segdef  *seg;

    while( (seg = SegDefs) != NULL ) {
        SegDefs = seg->next;
        DoSegment( seg, dgroup_def, tgroup_def, use_16 );
        CGFree( seg->str );
        CGFree( seg );
    }
}

void    DefSegment( segment_id segid, seg_attr attr, const char *str, uint align, bool use_16 )
/*********************************************************************************************/
{
    segdef              *new_seg;
    segment_id          first_code_segid;

    new_seg = AllocSegDefs( str );
    new_seg->segid = segid;
    new_seg->attr = attr;
    new_seg->align = align;
    first_code_segid = BACKSEGS;
    if( attr & EXEC ) {
        if( codeSegId == BACKSEGS ) {
            codeSegId = segid;
            first_code_segid = segid;
        }
        if( OptForSize == 0 ) {
            if( new_seg->align < 16 ) {
                new_seg->align = 16;
            }
        }
    }
    if( attr & BACK ) {
        dataSegId = segid;
    }
    if( NameIndex != 0 ) {
        /*
         * we are after BEStart processing
         *
         * don't use DGROUP for segments defined after BEStart
         */
        DoSegDefsAndFree( NULL, NULL, use_16 );
    }
    if( first_code_segid != BACKSEGS
      && _IsModel( CGSW_GEN_DBG_DF ) ) {
        DFBegCCU( first_code_segid, NULL );
    }
}

void DoEmptyQueue( void )
/***********************/
{
    EmptyQueue();
    TellUnreachLabels();
}

static  void    FPInitPatches( void )
/***********************************/
{
    int i;

    for( i = FPP_NUMBER_OF_TYPES; i-- > 0; ) {
        FPPatchImp[i] = NOT_IMPORTED;
    }
}

static  void    OutName( const char *name, array_control *dest )
/**************************************************************/
{
    uint            len;

    len = Length( name );
    if( len >= 256 ) {
        len = 255;
        FEMessage( FEMSG_INFO_PROC, "Code generator truncated name, its length exceeds allowed maximum." );
        FEMessage( FEMSG_INFO_PROC, (pointer)name );
    }
    OutByte( len, dest );
    OutBuffer( name, len, dest );
}

static  void    OutObjectName( cg_sym_handle sym, array_control *dest )
/*********************************************************************/
{
    OUTPUT_OBJECT_NAME( sym, OutName, dest, NORMAL );
}

static  void    OutString( const char *name, array_control *dest )
/****************************************************************/
{
    uint    len;

    len = Length( name );
    OutBuffer( name, len, dest );
}

char GetMemModel( void )
/**********************/
{
    char model;

    if( _IsTargetModel( CGSW_X86_BIG_CODE ) ) {
        if( _IsTargetModel( CGSW_X86_BIG_DATA ) ) {
            if( _IsntTargetModel( CGSW_X86_CHEAP_POINTER ) ) {
                model = 'h';
            } else {
                model = 'l';
            }
        } else {
            model = 'm';
        }
    } else if( _IsTargetModel( CGSW_X86_BIG_DATA ) ) {
        model = 'c';
    } else if( _IsTargetModel( CGSW_X86_FLAT_MODEL ) ) {
        model = 'f';
    } else {
        model = 's';
    }
    return( model );
}

static void OutModel( array_control *dest )
/*****************************************/
{
    char                model[6];

    if( _CPULevel( CPU_386 ) ) {
        model[0] = '3';
    } else if( _CPULevel( CPU_286 ) ) {
        model[0] = '2';
    } else {
        model[0] = '0';
    }
    model[1] = GetMemModel();
    model[2] = 'O';     /* Why? Ask microsoft! */
    if( _FPULevel( FPU_87 ) ) {
        if( _IsEmulation() ) {
            model[3] = 'e';
        } else {
            model[3] = 'p';
        }
    } else {
        model[3] = 'c';
    }
    model[4] = 'd';
    if( _IsModel( CGSW_GEN_POSITION_INDEPENDANT ) ) {
        model[4] = 'i';
    }
    model[5] = '\0';
    OutString( model, dest );
}

segment_id DbgSegDef( const char *seg_name, const char *seg_class, int seg_modifier )
/***********************************************************************************/
{
    index_rec   *rec;

    rec = AllocNewSegRec();
    rec->sidx = ++SegmentIndex;
    rec->cidx = GetNameIdx( seg_class, "" );
    rec->nidx = GetNameIdx( seg_name, "" );
    rec->location = 0;
    rec->big = 0;
    rec->need_base_set = true;
    rec->data_ptr_in_code = false;
    rec->data_in_code = false;
    rec->start_data_in_code = false;
    rec->private = true;
    rec->exec = false;
    rec->rom = false;
    rec->prefix_comdat_state = PCS_OFF;
    rec->max_written = 0;
    rec->max_size = 0;
    rec->comdat_label = NULL;
    rec->comdat_symbol = NULL;
    rec->total_comdat_size = 0;
    rec->comdat_nidx = 0;
    rec->comdat_size = 0;
    rec->virt_func_refs = NULL;
    rec->segid = --backSegId;
    rec->attr = SEG_ALGN_BYTE | seg_modifier;
    rec->data_prefix_size = 0;
    rec->comdat_prefix_import = NOT_IMPORTED;
    rec->base = rec->sidx;
    rec->btype = BASE_SEG;
    DoASegDef( rec, true );
    return( rec->segid );
}

static  void    DoSegGrpNames( array_control *dgroup_def, array_control *tgroup_def )
/***********************************************************************************/
{
    char        *dgroup;
    omf_idx     dgroup_idx;

    GetNameIdx( "", "" );       /* _NIDX_NULL */
    GetNameIdx( "CODE", "" );   /* _NIDX_CODE */
    GetNameIdx( "DATA", "" );   /* _NIDX_DATA */
    GetNameIdx( "BSS", "" );    /* _NIDX_BSS */
    GetNameIdx( "TLS", "" );    /* _NIDX_TLS */

#if _TARGET & _TARG_80386
    if( _IsTargetModel( CGSW_X86_FLAT_MODEL )
      && _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
        FlatNIndex = GetNameIdx( "FLAT", "" );
    }
#endif
    SegmentIndex = 0;
    PrivateIndexRO = 0;
    PrivateIndexRW = 0;
    CopyStr( FEAuxInfo( NULL, FEINF_CODE_GROUP ), CodeGroup );
    if( CodeGroup[0] == NULLCHAR ) {
        CodeGroupGIdx = 0;
    } else {
        CodeGroupNIdx = GetNameIdx( CodeGroup, "" );
        CodeGroupGIdx = ++GroupIndex;
    }
    dgroup = FEAuxInfo( NULL, FEINF_DATA_GROUP );
    if( dgroup == NULL ) {
        NoDGroup = true;
    } else {
        CopyStr( dgroup, DataGroup );
    }
    if( DataGroup[0] != NULLCHAR ) {
        TargetModel |= CGSW_X86_FLOATING_SS;
        dgroup_idx = GetNameIdx( DataGroup, "_GROUP" );
    } else {
        dgroup_idx = GetNameIdx( "DGROUP", "" );
    }
    OutIdx( dgroup_idx, dgroup_def );
    FillArray( &SegInfo, sizeof( index_rec ), MODEST_INFO, INCREMENT_INFO );
    /*
     * we are in BEStart processing
     *
     * define all default segments defined by front-end
     * use DGROUP for these segments
     */
    DoSegDefsAndFree( dgroup_def, tgroup_def, false );
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        if( _IsModel( CGSW_GEN_DBG_LOCALS )
          || _IsModel( CGSW_GEN_DBG_TYPES ) ) {
            DFDefSegs();
        }
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        CVDefSegs();
    } else {
        DbgTypeSize = 0;
        if( _IsModel( CGSW_GEN_DBG_LOCALS ) ) {
            DbgLocals = DbgSegDef( DbgSegs[0].seg_name, DbgSegs[0].class_name, SEG_COMB_PRIVATE );
        }
        if( _IsModel( CGSW_GEN_DBG_TYPES ) ) {
            DbgTypes = DbgSegDef( DbgSegs[1].seg_name, DbgSegs[1].class_name, SEG_COMB_PRIVATE );
        }
    }
}


static  void    KillStatic( array_control *arr )
/**********************************************/
{
    CGFree( arr->array );
    arr->array = NULL;
    arr->alloc = 0;
    arr->used = 0;
}

static  void    KillArray( array_control *arr )
/*********************************************/
{
    CGFree( arr->array );
    CGFree( arr );
}

static void initImports( void )
{
    ImportHdl = IMPORT_BASE;
    FillArray( &Imports, sizeof( byte ), MODEST_IMP, INCREMENT_IMP );
    GenStaticImports = false;
}

static void finiImports( void )
{
    KillStatic( &Imports );
}

static omf_idx getImportHdl( void )
{
    if( ImportHdl >= 0x7FFF ) {
        FatalError( "OMF file too many external symbols" );
    }
    return( ImportHdl++ );
}

void    ObjInit( void )
/*********************/
{
    array_control       *names;         /* for LNAMES*/
    array_control       *dgroup_def;
    array_control       *tgroup_def;
    void                *depend;

    FPInitPatches();
    CodeSize = 0;
    DataSize = 0;
    CurrFNo = 0;
    OpenObj();
    names = InitArray( sizeof( byte ), MODEST_HDR, INCREMENT_HDR );
    OutName( FEModuleName(), names );
    PutObjOMFRec( CMD_THEADR, names );
    if( _IsModel( CGSW_GEN_DBG_NUMBERS ) ) {
        OutName( FEAuxInfo( NULL, FEINF_SOURCE_NAME ), names );
        PutObjOMFRec( CMD_THEADR, names );
    }
#if _TARGET & _TARG_80386
    if( _IsTargetModel( CGSW_X86_EZ_OMF )
      || _IsTargetModel( CGSW_X86_FLAT_MODEL ) ) {
        OutShort( PHARLAP_OMF_COMMENT, names );
        if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
            OutString( "OS220", names );
        } else {
            OutString( "80386", names );
        }
        PutObjOMFRec( CMD_COMENT, names );
    }
#else
    OutShort( MS_OMF_COMMENT, names );
    PutObjOMFRec( CMD_COMENT, names );
#endif
    OutShort( MODEL_COMMENT, names );
    OutModel( names );
    PutObjOMFRec( CMD_COMENT, names );
    if( _IsTargetModel( CGSW_X86_FLAT_MODEL )
      && _IsModel( CGSW_GEN_DBG_DF ) ) {
        OutShort( LINKER_COMMENT, names );
        OutByte( LDIR_FLAT_ADDRS, names );
        PutObjOMFRec( CMD_COMENT, names );
    }
    if( _IsntModel( CGSW_GEN_DBG_DF )
      && _IsntModel( CGSW_GEN_DBG_CV ) ) {
        OutShort( LINKER_COMMENT, names );
        OutByte( LDIR_SOURCE_LANGUAGE, names );
        OutByte( DEBUG_MAJOR_VERSION, names );
        if( _IsModel( CGSW_GEN_DBG_TYPES )
          || _IsModel( CGSW_GEN_DBG_LOCALS ) ) {
            OutByte( DEBUG_MINOR_VERSION, names );
        } else {
            OutByte( 0, names );
        }
        OutString( (const char *)FEAuxInfo( NULL, FEINF_SOURCE_LANGUAGE ), names );
        PutObjOMFRec( CMD_COMENT, names );
    }
    for( depend = NULL; (depend = FEAuxInfo( depend, FEINF_NEXT_DEPENDENCY )) != NULL; ) {
        OutShort( DEPENDENCY_COMMENT, names );
        /*
         * OMF use dos time/date format
         */
        OutLongInt( _timet2dos( *(time_t *)FEAuxInfo( depend, FEINF_DEPENDENCY_TIMESTAMP ) ), names );
        OutName( FEAuxInfo( depend, FEINF_DEPENDENCY_NAME ), names );
        PutObjOMFRec( CMD_COMENT, names );
    }
    /*
     * mark end of dependancy list
     */
    OutShort( DEPENDENCY_COMMENT, names );
    PutObjOMFRec( CMD_COMENT, names );
    KillArray( names );

    dgroup_def = InitArray( sizeof( byte ), MODEST_INFO, INCREMENT_INFO );
    tgroup_def = InitArray( sizeof( byte ), MODEST_INFO, INCREMENT_INFO );
    SetOP( UNDEFSEG );
    DoSegGrpNames( dgroup_def, tgroup_def );
    if( dgroup_def->used > 0 ) {
        FlushNames();
        PutObjOMFRec( CMD_GRPDEF, dgroup_def );
    }
    if( tgroup_def->used > 0 ) {
        FlushNames();
        PutObjOMFRec( CMD_GRPDEF, tgroup_def );
    }
    KillArray( tgroup_def );
#if _TARGET & _TARG_80386
    if( _IsTargetModel( CGSW_X86_FLAT_MODEL )
      && _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
        FlatGIndex = ++GroupIndex;
        OutIdx( FlatNIndex, dgroup_def );
        FlushNames();
        PutObjOMFRec( CMD_GRPDEF, dgroup_def );
    }
#endif
    KillArray( dgroup_def );
    SetOP( codeSegId );
    initImports();
    AbsPatches = NULL;
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        if( _IsModel( CGSW_GEN_DBG_LOCALS )
          || _IsModel( CGSW_GEN_DBG_TYPES ) ) {
            DFObjInitDbgInfo();
#if 0 /* save for JimR and linker */
        } else if( _IsModel( CGSW_GEN_DBG_NUMBERS ) ) {
            DFObjLineInitDbgInfo();
#endif
        }
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        CVObjInitDbgInfo();
    } else {
        WVObjInitDbgInfo();
    }
}


offset  AskLocation( void )
/*************************/
{
    return( (offset)CurrSeg->location );
}

void ChkDbgSegSize( offset max, bool typing )
/*******************************************/
{
    const dbg_seg_info  *info;
    long_offset         curr;

    info = DbgSegs;
    if( typing )
        info++;
    PUSH_OP( *info->segid );
        curr = (offset)CurrSeg->location;
        if( curr >= max ) {
            if( typing ) {
                DbgTypeSize += curr;
            }
            *info->segid = DbgSegDef( info->seg_name, info->class_name, SEG_COMB_PRIVATE );
        }
    POP_OP();
}


bool    UseImportForm( fe_attr attr )
/***********************************/
{
    if( attr & (FE_GLOBAL|FE_IMPORT) )
        return( true );
    if( attr & FE_INTERNAL )
        return( false );
    return( true );
}



bool    AskSegIsNear( segment_id segid )
/**************************************/
{
    index_rec   *rec;

    if( segid < 0 )
        return( false );
    rec = AskSegIndex( segid );
    if( rec->btype != BASE_GRP )
        return( false );
    if( rec->base > CodeGroupGIdx )
        return( true );
    return( false );
}


bool    AskSegIsBlank( segment_id segid )
/***************************************/
{
    index_rec *rec;

    if( segid < 0 )
        return( true );
    rec = AskSegIndex( segid );
    return( rec->cidx == _NIDX_BSS );
}


bool    AskSegIsPrivate( segment_id segid )
/*****************************************/
{
    index_rec   *rec;

    if( segid < 0 )
        return( true );
    rec = AskSegIndex( segid );
    return( rec->private || rec->exec );
}


bool    AskSegIsROM( segment_id segid )
/*************************************/
{
    index_rec   *rec;

    if( segid < 0 )
        return( false );
    rec = AskSegIndex( segid );
    return( rec->rom != 0 );
}


segment_id  AskBackSeg( void )
/****************************/
{
    return( dataSegId );
}


segment_id  AskCodeSeg( void )
/****************************/
{
    return( codeSegId );
}


bool    HaveCodeSeg( void )
/*************************/
{
    return( codeSegId != BACKSEGS );
}


segment_id  AskAltCodeSeg( void )
/*******************************/
{
    return( altCodeSegId );
}

segment_id  AskCode16Seg( void )
/******************************/
{
    static segment_id   Code16Seg = BACKSEGS;

    if( Code16Seg == BACKSEGS ) {
        Code16Seg = --backSegId;
        DefSegment( Code16Seg, EXEC | GIVEN_NAME, "_TEXT16", 16, true );
    }
    return( Code16Seg );
}


static  void    EjectImports( void )
/**********************************/
{
    omf_cmd     cmd;

    if( Imports.used > 0 ) {
        if( GenStaticImports ) {
            cmd = CMD_LEXTDEF;
        } else {
            cmd = CMD_EXTDEF;
        }
        PutObjOMFRec( cmd, &Imports );
    }
}


static  void    SetPatches( void )
/********************************/
{
    temp_patch          *tpatch;
    array_control       *lbl_patches;
    obj_patch           *patch;

    while( (tpatch = CurrSeg->obj->tpatches) != NULL ) {
        CurrSeg->obj->tpatches = tpatch->link;
        lbl_patches = AskLblPatches( tpatch->lbl );
        patch = allocOut( lbl_patches, 1 );
        patch->ref = AskObjHandle();
        patch->where = tpatch->patch.where;
        patch->attr = tpatch->patch.attr;
        CGFree( tpatch );
    }
}


static  void    SetAbsPatches( void )
/***********************************/
{
    abs_patch   *apatch;
    object      *obj;

    obj = CurrSeg->obj;
    for( apatch = AbsPatches; apatch != NULL; apatch = apatch->link ) {
        if( apatch->patch.ref == INVALID_OBJHANDLE && apatch->obj == obj ) {
            apatch->patch.ref = AskObjHandle();
            apatch->flags |= AP_HAVE_OFFSET;
        }
    }
}


static void     EjectLEData( void )
/*********************************/
{
    array_control   *obj_data;
    omf_cmd         cmd;

    EjectImports();
    if( CurrSeg->obj->data.used > CurrSeg->data_prefix_size ) {
        SetPatches();
        SetAbsPatches();
        obj_data = &CurrSeg->obj->data;
        if( CurrSeg->comdat_label != NULL ) {
#if _TARGET & _TARG_80386
            if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
                cmd = CMD_COMDAT32;
            } else {
                cmd = CMD_COMDAT;
            }
#else
            cmd = CMD_COMDAT;
#endif
        } else {
#if _TARGET & _TARG_80386
            if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
                cmd = CMD_LEDATA32;
            } else {
                cmd = CMD_LEDATA;
            }
#else /* SEG32DBG dwarf, codeview */
            if( (CurrSeg->attr & SEG_USE_32)
              && _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
                cmd = CMD_LEDATA32;
            } else {
                cmd = CMD_LEDATA;
            }
#endif
        }
        PutObjOMFRec( cmd, obj_data );
        if( CurrSeg->obj->fixes.used > 0 ) {
            if( CurrSeg->data_ptr_in_code ) {
                OutShort( LINKER_COMMENT, obj_data );
                OutByte( LDIR_OPT_UNSAFE, obj_data );
                PutObjOMFRec( CMD_COMENT, obj_data );
            }
#if _TARGET & _TARG_80386
            if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
                cmd = CMD_FIXUPP32;
            } else {
                cmd = CMD_FIXUPP;
            }
#else
            cmd = CMD_FIXUPP;
#endif
            PutObjOMFRec( cmd, &CurrSeg->obj->fixes );
        }
        CurrSeg->data_ptr_in_code = false;
    }
}


static void GetSymLName( const char *name, omf_idx *nidx )
/********************************************************/
{
    *nidx = GetNameIdx( name, "" );
}

static omf_idx NeedComdatNidx( import_kind kind )
/***********************************************/
{
    if( CurrSeg->comdat_nidx == 0 ) {
        OUTPUT_OBJECT_NAME( CurrSeg->comdat_symbol, GetSymLName, &CurrSeg->comdat_nidx, kind );
        FlushNames();
    }
    return( CurrSeg->comdat_nidx );
}


void    OutSelect( bool starts )
/******************************/
{
    array_control   *obj_data;

    if( starts ) {
        selIdx = CurrSeg->sidx;
        SelStart = (offset)CurrSeg->location;
    } else if( selIdx != 0 ) {
        if( !CurrSeg->start_data_in_code ) {
            EjectLEData();
            obj_data = &CurrSeg->obj->data;
            OutShort( DISASM_COMMENT, obj_data );
#if _TARGET & _TARG_80386
            OutByte( DDIR_SCAN_TABLE_32, obj_data );
#else
            OutByte( DDIR_SCAN_TABLE, obj_data );
#endif
            if( CurrSeg->comdat_label != NULL ) {
                OutIdx( 0, obj_data );
                OutIdx( NeedComdatNidx( NORMAL ), obj_data );
            } else {
                OutIdx( selIdx, obj_data );
            }
            OutOffset( SelStart, obj_data );
            OutOffset( (offset)CurrSeg->location, obj_data );
            PutObjOMFRec( CMD_COMENT, obj_data );
        }
        selIdx = 0;
    }
    CurrSeg->start_data_in_code = false;
}

static  void    OutLEDataStart( bool iterated )
/*********************************************/
{
    byte            flag;
    array_control   *obj_data;

    obj_data = &CurrSeg->obj->data;
    if( obj_data->used == 0 ) {
        if( CurrSeg->comdat_label != NULL ) {
            flag = 0;
            if( CurrSeg->location != 0 )
                flag |= 1; /* continued */
            if( iterated )
                flag |= 2;   /* LIDATA form */
            if( (FEAttr( CurrSeg->comdat_symbol ) & FE_GLOBAL) == 0 ) {
                flag |= 0x4;    /* local comdat */
            }
            OutByte( flag, obj_data );
            OutByte( 0x10, obj_data );
            OutByte( 0, obj_data );
            OutOffset( (offset)CurrSeg->location, obj_data );
            OutIdx( 0, obj_data );
            if( CurrSeg->btype == BASE_GRP ) {
                OutIdx( CurrSeg->base, obj_data );  /* group index*/
            } else {
                OutIdx( 0, obj_data );
            }
            OutIdx( CurrSeg->sidx, obj_data );      /* segment index*/
            OutIdx( NeedComdatNidx( NORMAL ), obj_data );
        } else { /* LEDATA */
            OutIdx( CurrSeg->sidx, obj_data );
#if _TARGET & _TARG_80386
            OutOffset( (offset)CurrSeg->location, obj_data );
#else /* SEG32DBG dwarf, codeview */
            if( CurrSeg->attr & SEG_USE_32 ) {
                OutLongOffset( CurrSeg->location, obj_data );
            } else {
                OutOffset( (offset)CurrSeg->location, obj_data );
            }
#endif
        }
        CurrSeg->obj->start = CurrSeg->location;
        CurrSeg->data_prefix_size = obj_data->used;
    }
    if( CurrSeg->start_data_in_code ) {
        OutSelect( true );
    }
}


static  void    CheckLEDataSize( unsigned max_size, bool need_init )
/******************************************************************/
{
    long_offset     start;
    unsigned        used;
    object          *obj;
    long_offset     end_valid;

    obj = CurrSeg->obj;
    start = obj->start;
    used = obj->data.used;
    if( CurrSeg->location < start ) {
        EjectLEData();
    } else if( CurrSeg->location - start + max_size > BUFFSIZE - TOLERANCE - CurrSeg->data_prefix_size ) {
        EjectLEData();
    } else {
        end_valid = start + used - CurrSeg->data_prefix_size;
        if( CurrSeg->max_written > end_valid && CurrSeg->location > end_valid ) {
            EjectLEData();
        }
    }
    if( need_init ) {
        OutLEDataStart( false );
    }
}

void    SetUpObj( bool is_data )
/******************************/
{
    object      *obj;
    bool        old_data;

    obj = CurrSeg->obj;
    if( obj == NULL )
        return;
    if( obj->fixes.used >= BUFFSIZE - TOLERANCE ) {
        EjectLEData();
        return;
    }
    if( Imports.used >= BUFFSIZE - TOLERANCE ) {
         EjectLEData();
         return;
    }
    /* so that a call will always fit */
    CheckLEDataSize( 4 * sizeof( offset ), false );
    if( CurrSeg->exec ) {
        old_data = ( CurrSeg->data_in_code != 0 );
        CurrSeg->data_in_code = is_data;
        if( is_data != old_data ) {
            if( is_data ) {
                CurrSeg->start_data_in_code = true;
            } else {
                OutSelect( false );
                SetUpObj( false );
            }
        }
    }
}


static  void    GenComdef( void )
/*******************************/
{
    array_control       *comdef;
    unsigned            count;
    unsigned_8          ind;
    unsigned            size;
    cg_sym_handle       sym;
    omf_cmd             cmd;

    if( CurrSeg->comdat_label != NULL &&
        CurrSeg->max_written < CurrSeg->comdat_size ) {
        if( CurrSeg->max_written != 0 ) {
            Zoiks( ZOIKS_080 );
        }
        /*
         * have to eject any pending imports here or the ordering
         * gets messed up
         */
        EjectImports();
        size = CurrSeg->comdat_size - CurrSeg->max_written;
        comdef = InitArray( sizeof( byte ), MODEST_EXP, INCREMENT_EXP );
        sym = CurrSeg->comdat_symbol;
        OutObjectName( sym, comdef );
        OutByte( 0, comdef );                  /* type index */
        if( CurrSeg->btype == BASE_GRP && CurrSeg->base == DGroupIndex ) {
            OutByte( COMDEF_NEAR, comdef );    /* common type */
        } else {
            OutByte( COMDEF_FAR, comdef );     /* common type */
            OutByte( 1, comdef );              /* number of elements */
        }
        /*
         * Strictly speaking, this should be <= 0x80. However a number
         * of tools (including our own!) have problems with doing a
         * 128 byte COMDEF size in 1 byte, so we'll waste some space
         * and use 2 bytes, but sleep much sounder at night.
         */
        if( size < 0x80 ) {
            count = 1;
            ind = 0;
        } else if( size < 0x1000 ) {
            count = 2;
            ind = COMDEF_LEAF_2;
        } else if( size < 0x100000 ) {
            count = 3;
            ind = COMDEF_LEAF_3;
        } else {
            count = 4;
            ind = COMDEF_LEAF_4;
        }
        if( ind != 0 ) {
            /*
             * multi-byte indicator
             */
            OutByte( ind, comdef );
        }
        do {
            /*
             * element size
             */
            OutByte( (byte)size, comdef );
            size >>= 8;
            --count;
        } while( count != 0 );
        if( FEAttr( sym ) & FE_GLOBAL ) {
            cmd = CMD_COMDEF;
        } else {
            cmd = CMD_LCOMDEF;
        }
        PutObjOMFRec( cmd, comdef );
        KillArray( comdef );
        TellImportHandle( sym, getImportHdl() );
    }
}

static  void    EjectExports( void )
/**********************************/
{
    omf_cmd         cmd;

    if( CurrSeg->obj->exports.used > 0 ) {
        if( CurrSeg->obj->gen_static_exports ) {
#if _TARGET & _TARG_80386
            if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
                cmd = CMD_LPUBDEF32;
            } else {
                cmd = CMD_LPUBDEF;
            }
#else
            cmd = CMD_LPUBDEF;
#endif
        } else {
#if _TARGET & _TARG_80386
            if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
                cmd = CMD_PUBDEF32;
            } else {
                cmd = CMD_PUBDEF;
            }
#else
            cmd = CMD_PUBDEF;
#endif
        }
        PutObjOMFRec( cmd, &CurrSeg->obj->exports );
    }
}


static  void    FlushLineNum( object *obj )
/*****************************************/
{
    omf_cmd     cmd;

    if( obj->last_line ) {
        OutShort( obj->last_line, obj->lines );
        OutOffset( obj->last_offset, obj->lines );
        if( CurrSeg->comdat_label != NULL ) {
#if _TARGET & _TARG_80386
            if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
                cmd = CMD_LINSYM32;
            } else {
                cmd = CMD_LINSYM;
            }
#else
            cmd = CMD_LINSYM;
#endif
        } else {
#if _TARGET & _TARG_80386
            if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
                cmd = CMD_LINNUM32;
            } else {
                cmd = CMD_LINNUM;
            }
#else
            cmd = CMD_LINNUM;
#endif
        }
        PutObjOMFRec( cmd, obj->lines );
        obj->lines_generated = true;
        obj->last_line = 0;
        obj->last_offset = 0;
    }
}


static  void    FlushObject( void )
/*********************************/
{
    object      *obj;

    SetUpObj( false );
    GenComdef();
    CurrSeg->total_comdat_size += CurrSeg->comdat_size;
    EjectLEData();
    EjectExports();
    obj = CurrSeg->obj;
    if( obj->lines != NULL ) {
        FlushLineNum( obj );
        KillArray( obj->lines );
        obj->lines = NULL;
    }
    FreeObjCache();
}


static  index_rec       *AskIndexRec( uint_16 sidx )
/**************************************************/
{
    unsigned    i;

    for( i = 0; i < SegInfo.used; ++i ) {
        if( _ARRAYOF( &SegInfo, index_rec )[i].sidx == sidx ) {
            return( _ARRAYOF( &SegInfo, index_rec ) + i );
        }
    }
    return( NULL );
}

#if _TARGET & _TARG_8086
#define _TargetInt _TargetShort
#else
#define _TargetInt _TargetBigInt
#endif

static  void    FiniTarg( void )
/******************************/
{
    byte        attr;
    index_rec   *rec;
    object      *obj;
    offset      size_s;
    long_offset size_l;

    FlushObject();
    obj = CurrSeg->obj;
    rec = AskIndexRec( obj->index );
    size_l = rec->max_size;
    if( rec->exec ) {
        CodeSize += size_l + rec->total_comdat_size;
    } else if( rec->cidx == _NIDX_DATA ) {
        DataSize += size_l + rec->total_comdat_size;
    }
    if( rec->big ) {
        attr = rec->attr | SEG_BIG;
        PatchObj( obj->segfix, SEGDEF_ATTR, &attr, sizeof( attr ) );
    } else {
#if _TARGET & _TARG_80386
        size_s = _TargetInt( size_l );
        PatchObj( obj->segfix, SEGDEF_SIZE, (byte *)&size_s, sizeof( size_s ) );
#else /* SEG32DBG dwarf, codeview */
        if( rec->attr & SEG_USE_32 ) {
            size_l = _TargetLongInt( size_l );
            PatchObj( obj->segfix, SEGDEF_SIZE, (byte *)&size_l, sizeof( size_l ) );
        } else {
            size_s = _TargetInt( size_l );
            PatchObj( obj->segfix, SEGDEF_SIZE, (byte *)&size_s, sizeof( size_s ) );
        }
#endif
    }
    KillStatic( &obj->exports );
    KillStatic( &obj->data );
    KillStatic( &obj->fixes );
    CGFree( obj );
}

void    FlushOP( segment_id segid )
/*********************************/
{
    index_rec   *rec;

    PUSH_OP( segid );
        if( segid == codeSegId ) {
            DoEmptyQueue();
        }
        if( _IsModel( CGSW_GEN_DBG_DF ) ) {
            rec = CurrSeg;
            if( rec->exec || rec->cidx == _NIDX_DATA || rec->cidx == _NIDX_BSS ) {
                if( rec->max_size != 0 ) {
                    DFSegRange();
                }
            }
        }
        FiniTarg();
        CurrSeg->obj = NULL;
    POP_OP();
}

static void FiniWVTypes( void )
/*****************************/
{
    long_offset         curr;
    const dbg_seg_info  *info;

    WVTypesEof();
    info = &DbgSegs[1];
    PUSH_OP( *info->segid );
        curr = (offset)CurrSeg->location;
        curr += DbgTypeSize;
        *info->segid = DbgSegDef( info->seg_name, info->class_name, SEG_COMB_PRIVATE );
        SetOP( *info->segid );
        WVDmpCueInfo( curr );
    POP_OP();
}

static void FlushSelect( void )
/*****************************/
{
    if( selIdx != 0 ) {
        OutSelect( false );
    }
}


static  void    FlushData( void )
/*******************************/
{
    GenComdef();
    CurrSeg->total_comdat_size += CurrSeg->comdat_size;
    EjectLEData();
    FlushLineNum( CurrSeg->obj );
    FlushSelect();
}

static  void    NormalData( void )
/********************************/
{
    FlushData();
    CurrSeg->location = CurrSeg->max_written = CurrSeg->max_size;
    CurrSeg->comdat_size = 0;
    CurrSeg->comdat_label = NULL;
    CurrSeg->comdat_symbol = NULL;
    CurrSeg->need_base_set = true;
    CurrSeg->prefix_comdat_state = PCS_OFF;
    KillLblRedirects();
}


static void DoSegARange( offset *codesize, index_rec *rec )
/*********************************************************/
{
    if( rec->exec || rec->cidx == _NIDX_DATA ||rec->cidx == _NIDX_BSS ) {
        if( rec->max_size != 0 ) {
            PUSH_OP( rec->segid );
                if( CurrSeg->comdat_symbol != NULL ) {
                    DFSymRange( rec->comdat_symbol, (offset)rec->comdat_size );
                }
                NormalData();
                DFSegRange();
            POP_OP();
        }
        if( rec->exec ) {
            *codesize += rec->max_size + rec->total_comdat_size;
        }
    }
}

static  void    DoPatch( obj_patch *patch, offset lc )
/****************************************************/
{
    uint_32     lword_val;
    uint_16     word_val;
    byte        byte_val;

    if( patch->attr & LONG_PATCH ) {
        if( patch->attr & ADD_PATCH ) {
            GetFromObj( patch->ref, patch->where, (byte *)&lword_val, sizeof( lword_val ) );
            _TargetAddL( lword_val, lc );
        } else {
            lword_val = _TargetLongInt( lc );
        }
        PatchObj( patch->ref, patch->where, (byte *)&lword_val, sizeof( lword_val ) );
    } else if( patch->attr & WORD_PATCH ) {
        if( patch->attr & ADD_PATCH ) {
            GetFromObj( patch->ref, patch->where, (byte *)&word_val, sizeof( word_val ) );
            _TargetAddW( word_val, lc );
        } else {
            word_val = _TargetShort( lc );
        }
        PatchObj( patch->ref, patch->where, (byte *)&word_val, sizeof( word_val ) );
    } else {
        if( patch->attr & ADD_PATCH ) {
            GetFromObj( patch->ref, patch->where, (byte *)&byte_val, sizeof( byte_val ) );
            byte_val += lc;
        } else {
            byte_val = lc;
        }
        PatchObj( patch->ref, patch->where, (byte *)&byte_val, sizeof( byte_val ) );
    }
}



static  void    FiniAbsPatches( void )
/************************************/
{
    abs_patch   *apatch;

    while( (apatch = AbsPatches) != NULL ) {
        AbsPatches = apatch->link;
        DoPatch( &apatch->patch, (offset)apatch->value );
        CGFree( apatch );
    }
}


static  void    EndModule( void )
/*******************************/
{
    byte          b;
    omf_cmd       cmd;
    array_control tmp;

    /*
     * There is a bug in MS's LINK386 program that causes it not to recognize a
     * MODEND386 record in some situations. We can get around it by only outputing
     * 16-bit MODEND records. This causes us no pain, since we never need any
     * features provided by the 32-bit form anyway.
     */
#if 0
 #if _TARGET & _TARG_80386
    if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
        cmd = CMD_MODEND32;
    } else {
        cmd = CMD_MODEND;
    }
 #else
    cmd = CMD_MODEND;
 #endif
#else
    cmd = CMD_MODEND;
#endif
    b = 0;                     /* non-main module, no start address*/
    tmp.used = sizeof( byte );
    tmp.array = &b;
    PutObjOMFRec( cmd, &tmp );
}


void    ObjFini( void )
/*********************/
{
    unsigned    i;
    pointer     auto_import;
    char        *lib;
    char        *alias;

    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        if( _IsModel( CGSW_GEN_DBG_LOCALS )
          || _IsModel( CGSW_GEN_DBG_TYPES ) ) {
            offset  codesize;

            codesize = 0;
            for( i = 0; i < SegInfo.used; ++i ) {
                if( _ARRAYOF( &SegInfo, index_rec )[i].obj != NULL ) {
                    DoSegARange( &codesize, _ARRAYOF( &SegInfo, index_rec ) + i );
                }
            }
            DFObjFiniDbgInfo( codesize );
#if 0 /* save for Jimr */
        } else if( _IsModel( CGSW_GEN_DBG_NUMBERS ) ) {
            DFObjLineFini( );
#endif
        }
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        CVObjFiniDbgInfo();
    } else {
        if( _IsModel( CGSW_GEN_DBG_TYPES ) ) {
            FiniWVTypes();
        }
        WVObjFiniDbgInfo();
    }
    /*
     * Fini all segments.
     */
    for( i = 0; i < SegInfo.used; ++i ) {
        if( _ARRAYOF( &SegInfo, index_rec )[i].obj != NULL ) {
            CurrSeg = _ARRAYOF( &SegInfo, index_rec ) + i;
            FiniTarg();
        }
    }
    /*
     * if 8087 is used then enable emit default 8087 import records.
     */
    if( Used87 ) {
        (void)FEAuxInfo( NULL, FEINF_USED_8087 );
    }
    /*
     * Emit default import records.
     */
    for( auto_import = NULL; (auto_import = FEAuxInfo( auto_import, FEINF_NEXT_IMPORT )) != NULL; ) {
        OutName( FEAuxInfo( auto_import, FEINF_IMPORT_NAME ), &Imports );
        OutIdx( 0, &Imports );           /* type index*/
        if( Imports.used >= BUFFSIZE - TOLERANCE ) {
            PutObjOMFRec( CMD_EXTDEF, &Imports );
        }
    }
    for( auto_import = NULL; (auto_import = FEAuxInfo( auto_import, FEINF_NEXT_IMPORT_S )) != NULL; ) {
        OutObjectName( FEAuxInfo( auto_import, FEINF_IMPORT_NAME_S ), &Imports );
        OutIdx( 0, &Imports );           /* type index*/
        if( Imports.used >= BUFFSIZE - TOLERANCE ) {
            PutObjOMFRec( CMD_EXTDEF, &Imports );
        }
    }
    if( Imports.used > 0 ) {
        PutObjOMFRec( CMD_EXTDEF, &Imports );
    }
    /*
     * Emit default library search records.
     */
    for( lib = NULL; (lib = FEAuxInfo( lib, FEINF_NEXT_LIBRARY )) != NULL; ) {
        OutShort( LIBNAME_COMMENT, &Imports );
        OutString( (char *)FEAuxInfo( lib, FEINF_LIBRARY_NAME ), &Imports );
        PutObjOMFRec( CMD_COMENT, &Imports );
    }
    /*
     * Emit alias definition records.
     */
    for( alias = NULL; (alias = FEAuxInfo( alias, FEINF_NEXT_ALIAS )) != NULL; ) {
        char    *alias_name;
        char    *subst_name;

        alias_name = FEAuxInfo( alias, FEINF_ALIAS_NAME );
        if( alias_name == NULL ) {
            OutObjectName( FEAuxInfo( alias, FEINF_ALIAS_SYMBOL ), &Imports );
        } else {
            OutName( alias_name, &Imports );
        }
        subst_name = FEAuxInfo( alias, FEINF_ALIAS_SUBST_NAME );
        if( subst_name == NULL ) {
            OutObjectName( FEAuxInfo( alias, FEINF_ALIAS_SUBST_SYMBOL ), &Imports );
        } else {
            OutName( subst_name, &Imports );
        }
        PutObjOMFRec( CMD_ALIAS, &Imports );
    }
    finiImports();
    KillStatic( &SegInfo );
    FiniAbsPatches();
    EndModule();
    CloseObj();
    FEMessage( FEMSG_CODE_SIZE, (pointer)(pointer_uint)CodeSize );
    FEMessage( FEMSG_DATA_SIZE, (pointer)(pointer_uint)DataSize );
}


static  void    FreeAbsPatch( abs_patch *apatch )
/***********************************************/
{
    abs_patch   **owner;

    for( owner = &AbsPatches; *owner != apatch; ) {
        owner = &(*owner)->link;
    }
    *owner = (*owner)->link;
    CGFree( apatch );
}


static void CheckExportSwitch( bool next_is_static )
/**************************************************/
{
    /*
     * are we switching from global to statics or vis-versa
     */
    if( CurrSeg->obj->gen_static_exports != next_is_static ) {
        EjectExports();
        CurrSeg->obj->gen_static_exports = next_is_static;
    }
}


static  void    CheckImportSwitch( bool next_is_static )
/******************************************************/
{
    /*
     * are we switching from static imports to global or vis-versa
     */
    if( GenStaticImports != next_is_static ) {
        EjectImports();
        GenStaticImports = next_is_static;
    }
}


static  void    OutExport( cg_sym_handle sym )
/*****************************************/
{
    array_control       *obj_exports;
    object              *obj;

    obj = CurrSeg->obj;
    obj_exports = &obj->exports;
    if( obj_exports->used >= BUFFSIZE - TOLERANCE ) {
        EjectExports();
    }
    CheckExportSwitch( (FEAttr( sym ) & FE_GLOBAL) == 0 );
    if( obj_exports->used == 0 ) {
        if( CurrSeg->btype == BASE_GRP ) {
            OutIdx( CurrSeg->base, obj_exports );   /* group index*/
        } else {
#if _TARGET & _TARG_80386
            OutIdx( FlatGIndex, obj_exports );      /* will be 0 if we have none */
#else
            OutIdx( 0, obj_exports );
#endif
        }
        OutIdx( obj->index, obj_exports );          /* segment index*/
    }
    OutObjectName( sym, obj_exports );
    OutOffset( (offset)CurrSeg->location, obj_exports );
    OutIdx( 0, obj_exports );                       /* type index*/
}


static void _TellImportHandle( cg_sym_handle sym, import_handle imphdl, bool alt_dllimp )
/*************************************************************************************/
{
    if( alt_dllimp ) {
        FEBack( sym )->imp_alt = imphdl;
    } else {
        TellImportHandle( sym, imphdl );
    }
}

static import_handle _AskImportHandle( cg_sym_handle sym, bool alt_dllimp )
/**********************************************************************/
{
    if( alt_dllimp ) {
        return( FEBack( sym )->imp_alt );
    } else {
        return( AskImportHandle( sym ) );
    }
}

static void GenImportResolve( cg_sym_handle sym, omf_idx idx, omf_idx def_idx )
/*****************************************************************************/
{
    array_control       *cmt;
    omf_idx             nidx;
    pointer             cond;
    import_type         type;

    cmt = InitArray( sizeof( byte ), MODEST_HDR, INCREMENT_HDR );
    type = (import_type)(pointer_uint)FEAuxInfo( sym, FEINF_IMPORT_TYPE );
    switch( type ) {
    case IMPORT_IS_LAZY:
        OutShort( LAZY_EXTRN_COMMENT, cmt );
        OutIdx( idx, cmt );
        OutIdx( def_idx, cmt );
        break;
    case IMPORT_IS_WEAK:
        OutShort( WEAK_EXTRN_COMMENT, cmt );
        OutIdx( idx, cmt );
        OutIdx( def_idx, cmt );
        break;
    case IMPORT_IS_CONDITIONAL_PURE:
        OutShort( WEAK_EXTRN_COMMENT, cmt );
        OutIdx( idx, cmt );
        OutIdx( def_idx, cmt );
        PutObjOMFRec( CMD_COMENT, cmt );
        /* fall through */
    case IMPORT_IS_CONDITIONAL:
        OutShort( LINKER_COMMENT, cmt );
        if( type == IMPORT_IS_CONDITIONAL ) {
            OutByte( LDIR_VF_TABLE_DEF, cmt );
        } else {
            OutByte( LDIR_VF_PURE_DEF, cmt );
        }
        OutIdx( idx, cmt );
        OutIdx( def_idx, cmt );
        for( cond = FEAuxInfo( sym, FEINF_CONDITIONAL_IMPORT ); cond != NULL; cond = FEAuxInfo( cond, FEINF_NEXT_CONDITIONAL ) ) {
            sym = FEAuxInfo( cond, FEINF_CONDITIONAL_SYMBOL );
            OUTPUT_OBJECT_NAME( sym, GetSymLName, &nidx, NORMAL );
            OutIdx( nidx, cmt );
        }
        FlushNames();
        break;
    }
    PutObjOMFRec( CMD_COMENT, cmt );
    KillArray( cmt );
}


static omf_idx  GenImport( cg_sym_handle sym, bool alt_dllimp )
/*************************************************************/
{
    omf_idx         imp_idx;
    fe_attr         attr;
    import_kind     kind;
    cg_sym_handle   def_resolve;
    omf_idx         def_idx;

    imp_idx = _AskImportHandle( sym, alt_dllimp );
    if( imp_idx == NOT_IMPORTED ) {
        attr = FEAttr( sym );
        imp_idx = getImportHdl();
        CheckImportSwitch( (attr & FE_GLOBAL) == 0 );
        _TellImportHandle( sym, imp_idx, alt_dllimp );
        kind = NORMAL;
        if( attr & FE_DLLIMPORT ) {
            if( !alt_dllimp ) {
                kind = DLLIMPORT;
            }
        } else if( _IsModel( CGSW_GEN_POSITION_INDEPENDANT ) ) {
            if( attr & FE_THREAD_DATA ) {
                kind = PIC_RW;
            }
        }
        OUTPUT_OBJECT_NAME( sym, OutName, &Imports, kind );
        OutIdx( 0, &Imports );           /* type index*/
        def_resolve = FEAuxInfo( sym, FEINF_DEFAULT_IMPORT_RESOLVE );
        if( def_resolve != NULL && def_resolve != sym ) {
            def_idx = GenImport( def_resolve, false );
            EjectImports();
            GenImportResolve( sym, imp_idx, def_idx );
        }
    }
    return( imp_idx );
}

static  omf_idx     GenImportComdat( void )
/*****************************************/
{
    CheckImportSwitch( (FEAttr( CurrSeg->comdat_symbol ) & FE_GLOBAL) == 0 );
    OUTPUT_OBJECT_NAME( CurrSeg->comdat_symbol, OutName, &Imports, SPECIAL );
    OutIdx( 0, &Imports );           /* type index*/
    return( getImportHdl() );
}

static  void    ComdatData( label_handle lbl, cg_sym_handle sym )
/************************************************************/
{
    FlushData();
    CurrSeg->obj->lines_generated = false;
    CurrSeg->location = CurrSeg->max_written = 0;
    CurrSeg->comdat_size = 0;
    CurrSeg->comdat_nidx = 0;
    CurrSeg->comdat_label = lbl;
    if( sym != NULL ) {
        CurrSeg->comdat_symbol = sym;
    } else {
        NeedComdatNidx( SPECIAL );
        CurrSeg->comdat_prefix_import = GenImportComdat();
        TellCommonLabel( lbl, CurrSeg->comdat_prefix_import );
    }
    CurrSeg->need_base_set = true;
    CurrSeg->prefix_comdat_state = PCS_OFF;
    KillLblRedirects();
}

static void     OutVirtFuncRef( cg_sym_handle virt )
/***********************************************/
{
    array_control   *obj_data;
    omf_idx         extdef;

    if( virt == NULL ) {
        extdef = NOT_IMPORTED;
    } else {
        extdef = GenImport( virt, false );
    }
    EjectLEData();
    obj_data = &CurrSeg->obj->data;
    OutShort( LINKER_COMMENT, obj_data );
    OutByte( LDIR_VF_REFERENCE, obj_data );
    OutIdx( extdef, obj_data );
    if( CurrSeg->comdat_symbol != NULL ) {
        OutIdx( 0, obj_data );
        OutIdx( NeedComdatNidx( NORMAL ), obj_data );
    } else {
        OutIdx( CurrSeg->sidx, obj_data );
    }
    PutObjOMFRec( CMD_COMENT, obj_data );
}


void    OutDLLExport( uint words, cg_sym_handle sym )
/***************************************************/
{
    array_control   *obj_data;

    /* unused parameters */ (void)words;

    SetUpObj( false );
    EjectLEData();
    obj_data = &CurrSeg->obj->data;
    OutShort( EXPORT_COMMENT, obj_data );
    OutByte( 2, obj_data );
#if _TARGET & _TARG_8086
    OutByte( words, obj_data );
#else
    /*
     * this should be 0 for everything except callgates to
     * 16-bit segments (from MS Knowledge Base)
     */
    OutByte( 0, obj_data );
#endif
    OutObjectName( sym, obj_data );
    OutByte( 0, obj_data );
    PutObjOMFRec( CMD_COMENT, obj_data );
}


void    OutLabel( label_handle lbl )
/**********************************/
{
    temp_patch          **owner;
    temp_patch          *tpatch;
    array_control       *lbl_patches;
    unsigned            i;
    pointer             patchptr;
    object              *obj;
    offset              lc;
    cg_sym_handle       sym;
    fe_attr             attr;
    void                *cookie;
    virt_func_ref_list  *curr;
    virt_func_ref_list  *next;

    sym = AskForLblSym( lbl );
    if( sym != NULL ) {
        attr = FEAttr( sym );
        if( ( attr & FE_PROC ) == 0 ) {
            if( attr & FE_DLLEXPORT ) {
                OutDLLExport( 0, sym );
            }
        }
        if( attr & FE_COMMON ) {
            ComdatData( lbl, sym );
        } else {
            if( CurrSeg->comdat_label != NULL )
                NormalData();
            if( UseImportForm( attr ) ) {
                OutExport( sym );
            }
        }
        for( curr = CurrSeg->virt_func_refs; curr != NULL; curr = next ) {
            next = curr->next;
            for( cookie = curr->cookie; cookie != NULL; cookie = FEAuxInfo( cookie, FEINF_VIRT_FUNC_NEXT_REFERENCE ) ) {
                OutVirtFuncRef( FEAuxInfo( cookie, FEINF_VIRT_FUNC_SYM ) );
            }
            CGFree( curr );
        }
        CurrSeg->virt_func_refs = NULL;
    } else if( CurrSeg->prefix_comdat_state != PCS_OFF ) {
        /*
            We have data coming out before the COMDAT symbol (select table
            before a common procedure). Gen a 'magical' comdat name to
            deal with it.
        */
        if( CurrSeg->prefix_comdat_state == PCS_NEED ) {
            ComdatData( lbl, NULL );
            CurrSeg->prefix_comdat_state = PCS_ACTIVE;
        }
        TellCommonLabel( lbl, CurrSeg->comdat_prefix_import );
    }
    lc = (offset)CurrSeg->location;
    TellAddress( lbl, lc );
    for( i = SegInfo.used; i-- > 0; ) {
        obj = _ARRAYOF( &SegInfo, index_rec )[i].obj;
        if( obj != NULL ) { /* twas flushed and not redefined*/
            owner = &obj->tpatches;
            for( ;; ) {
                tpatch = *owner;
                if( tpatch == NULL )
                    break;
                if( tpatch->lbl == lbl ) {
                    patchptr = &_ARRAYOF( &obj->data, byte )[tpatch->patch.where];
                    if( tpatch->patch.attr & ADD_PATCH ) {
                        if( tpatch->patch.attr & LONG_PATCH ) {
                            _TargetAddL( *(uint_32 *)patchptr, lc );
                        } else if( tpatch->patch.attr & WORD_PATCH ) {
                            _TargetAddW( *(uint_16 *)patchptr, lc );
                        } else {
                            *(byte *)patchptr += lc;
                        }
                    } else {
                        if( tpatch->patch.attr & LONG_PATCH ) {
                            *(uint_32 *)patchptr = _TargetLongInt( lc );
                        } else if( tpatch->patch.attr & WORD_PATCH ) {
                            *(uint_16 *)patchptr = _TargetShort( lc );
                        } else {
                            *(byte *)patchptr = lc;
                        }
                    }
                    *owner = tpatch->link;
                    CGFree( tpatch );
                } else {
                    owner = &tpatch->link;
                }
            }
        }
    }
    lbl_patches = AskLblPatches( lbl );
    for( i = 0; i < lbl_patches->used; i++ ) {
        DoPatch( _ARRAYOF( lbl_patches, obj_patch ) + i, lc );
    }
    KillArray( lbl_patches );
    TellDonePatches( lbl );
}


void    AbsPatch( abspatch_handle patch_handle, offset lc )
/*********************************************************/
{
    abs_patch   *apatch;

    apatch = (abs_patch *)patch_handle;
    if( apatch->flags & AP_HAVE_OFFSET ) {
        DoPatch( &apatch->patch, lc );
        FreeAbsPatch( apatch );
    } else {
        apatch->value = lc;
        apatch->flags |= AP_HAVE_VALUE;
    }
}


void    *InitPatches( void )
/**************************/
{
    return( InitArray( sizeof( obj_patch ),  MODEST_PAT, INCREMENT_PAT ) );
}


static omf_fix_loc getOMFFixLoc( fix_class class )
/************************************************/
{
#if _TARGET & _TARG_80386
    if( class & F_FAR16 ) {
        /* want a 16:16 fixup for a __far16 call */
        return( LOC_BASE_OFFSET );
    }
#endif
    switch( F_CLASS( class ) ) {
    case F_BASE:
        return( LOC_BASE );
#if _TARGET & _TARG_8086
    case F_OFFSET:
        return( LOC_OFFSET );
    case F_BIG_OFFSET:
        return( LOC_OFFSET_32 );
    case F_LDR_OFFSET:
        return( LOC_OFFSET_LOADER );
    case F_PTR:
        return( LOC_BASE_OFFSET );
#else
    case F_OFFSET:
    case F_BIG_OFFSET:
        if( _IsTargetModel( CGSW_X86_EZ_OMF ) ) {
            return( LOC_PHARLAP_OFFSET_32 );
        } else {
            return( LOC_OFFSET_32 );
        }
    case F_LDR_OFFSET:
        if( _IsTargetModel( CGSW_X86_EZ_OMF ) ) {
            return( LOC_PHARLAP_OFFSET_32 );
        } else {
            return( LOC_OFFSET_LOADER_32 );
        }
    case F_PTR:
        if( _IsTargetModel( CGSW_X86_EZ_OMF ) ) {
            return( LOC_PHARLAP_BASE_OFFSET_32 );
        } else {
            return( LOC_BASE_OFFSET_32 );
        }
#endif
    default:
        break;
    }
    /* error */
    return( 0 );
}


static void DoFix( omf_idx idx, bool rel, base_type base, fix_class class, omf_idx sidx )
/***************************************************************************************/
{
    fixup       *cursor;
    int         where;
    object      *obj;
    index_rec   *rec;
    byte        b;

    b = rel ? LOCAT_REL : LOCAT_ABS;
    if( F_CLASS( class ) == F_PTR && CurrSeg->data_in_code ) {
        CurrSeg->data_ptr_in_code = true;
    }
    obj = CurrSeg->obj;
    cursor = allocOut( &obj->fixes, sizeof( fixup ) );
    where = CurrSeg->location - obj->start;
    cursor->locatof = b + ( getOMFFixLoc( class ) << S_LOCAT_LOC ) + ( where >> 8 );
    cursor->fset = where;
    if( base != BASE_IMP ) {
        rec = AskIndexRec( sidx );
        /*
         * fixups to a code segment that is currently in COMDAT mode must be
         * done as imports relative to the comdat symbol.
         */
        if( rec->exec && rec->comdat_symbol != NULL ) {
            idx = GenImport( rec->comdat_symbol, false );
            base = BASE_IMP;
        }
    }
#if _TARGET & _TARG_80386
    if( _IsTargetModel( CGSW_X86_FLAT_MODEL )
      && _IsntTargetModel( CGSW_X86_EZ_OMF )
      && ( F_CLASS( class ) != F_PTR ) ) {
        omf_idx     grp_idx;

  #if 0
        /* only generate a normal style fixup for now */
        if( class & F_TLS ) {
            grp_idx = TLSGIndex;
        } else {
            grp_idx = FlatGIndex;
        }
  #else
        grp_idx = FlatGIndex;
  #endif
        if( base == BASE_GRP ) {
            cursor->fixdat = FIXDAT_FRAME_GROUP + FIXDAT + BASE_SEG;
            OutIdx( grp_idx, &obj->fixes );
            OutIdx( sidx, &obj->fixes );
        } else {
            cursor->fixdat = FIXDAT_FRAME_GROUP + FIXDAT + base;
            OutIdx( grp_idx, &obj->fixes );
            OutIdx( idx, &obj->fixes );
        }
    } else
#endif
    if( base == BASE_GRP ) {
        cursor->fixdat = FIXDAT_FRAME_GROUP + FIXDAT + BASE_SEG;
        OutIdx( idx, &obj->fixes );
        OutIdx( sidx, &obj->fixes );
    } else {
        cursor->fixdat = FIXDAT_FRAME_IMPLIED + FIXDAT + base;
        OutIdx( idx, &obj->fixes );
    }
}


void    SetBigLocation( long_offset loc )
/***************************************/
{
    CurrSeg->location = loc;
    if( CurrSeg->comdat_label != NULL ) {
        if( CurrSeg->comdat_size < loc ) {
            CurrSeg->comdat_size = loc;
        }
    } else {
        if( CurrSeg->max_size < loc ) {
            CurrSeg->max_size = loc;
        }
    }
}

void    IncLocation( offset by )
/******************************/
{
    long_offset     sum;

    CurrSeg->obj->pending_line_number = 0;
    sum = CurrSeg->location + by;
    if( _IsntTargetModel( CGSW_X86_EZ_OMF )
      && (CurrSeg->attr & SEG_USE_32) == 0 ) {
        sum &= 0xFFFF;
    }
    if( sum < CurrSeg->location ) { /* if wrapped*/
        if( sum != 0 || CurrSeg->big == 1 ) {
            FatalError( "segment too large" );
        } else if( CurrSeg->comdat_label == NULL ) {
            CurrSeg->big = 1;
            if( CurrSeg->attr & SEG_USE_32 ) {
                CurrSeg->max_size = (long_offset)(-1);
                CurrSeg->location = (long_offset)(-1);
            } else {
                CurrSeg->max_size = (short_offset)(-1);
                CurrSeg->location =  1L << 8 * sizeof( short_offset );
            }
        }
    } else {
        SetBigLocation( sum );
    }
}


void    SetLocation( offset loc )
/*******************************/
{
    CurrSeg->location = loc;
    if( CurrSeg->comdat_label != NULL ) {
        if( CurrSeg->comdat_size < loc ) {
            CurrSeg->comdat_size = loc;
        }
    } else {
        if( CurrSeg->max_size < loc ) {
            CurrSeg->max_size = loc;
        }
    }
}

static  void    DecLocation( offset by )
/**************************************/
{
    SetLocation( (offset)CurrSeg->location - by );
}

void    OutFPPatch( fp_patches i )
/********************************/
{
    omf_idx     idx;

    idx = FPPatchImp[i];
    if( idx == NOT_IMPORTED ) {
        idx = getImportHdl();
        FPPatchImp[i] = idx;
        if( GenStaticImports ) {
            EjectImports();
            GenStaticImports = false;
        }
        OutName( FPPatchName[i], &Imports );
        OutIdx( 0, &Imports );                   /* type index*/
        if( FPPatchAltName[i] != NULL ) {
            getImportHdl();
            OutName( FPPatchAltName[i], &Imports );
            OutIdx( 0, &Imports );               /* type index*/
        }
    }
    CheckLEDataSize( 2 * sizeof( offset ), true );
    DoFix( idx, false, BASE_IMP, F_OFFSET, 0 );
    if( FPPatchAltName[i] != NULL ) {
        IncLocation( sizeof( byte ) );
        DoFix( idx + 1, false, BASE_IMP, F_OFFSET, 0 );
        DecLocation( sizeof( byte ) );
    }
}


void    OutPatch( label_handle lbl, patch_attr attr )
/***************************************************/
{
    temp_patch  *tpatch;

    /* careful, might be patching offset of seg:off */
    CheckLEDataSize( 3 * sizeof( offset ), true );
    tpatch = CGAlloc( sizeof( *tpatch ) );
    tpatch->link = CurrSeg->obj->tpatches;
    CurrSeg->obj->tpatches = tpatch;
    tpatch->lbl = lbl;
    tpatch->patch.ref = INVALID_OBJHANDLE;
    tpatch->patch.where = CurrSeg->location - CurrSeg->obj->start + CurrSeg->data_prefix_size;
    tpatch->patch.attr = attr;
}

abs_patch       *NewAbsPatch( void )
/**********************************/
{
    abs_patch   *apatch;

    apatch = CGAlloc( sizeof( *apatch ) );
    memset( apatch, 0, sizeof( *apatch ) );
    apatch->link = AbsPatches;
    AbsPatches = apatch;
    return( apatch );
}


static  void    InitLineInfo( object *obj )
/*****************************************/
{
    obj->lines->used = 0;
    if( CurrSeg->comdat_label != NULL ) {
        OutByte( obj->lines_generated, obj->lines );
        OutIdx( NeedComdatNidx( NORMAL ), obj->lines );
    } else {
        if( CurrSeg->btype == BASE_GRP ) {
            OutIdx( CurrSeg->base, obj->lines );
        } else {
            OutIdx( 0, obj->lines );
        }
        OutIdx( obj->index, obj->lines );
    }
    obj->last_line = 0;
    obj->last_offset = 0;
}


static  void    ChangeObjSrc( short fno )
/***************************************/
{
    array_control       *names;
    const char          *fname;

    fname = SrcFNoFind( fno );
    CurrFNo = fno;
    names = InitArray( sizeof( byte ), MODEST_HDR, INCREMENT_HDR );
    OutName( fname, names );
    PutObjOMFRec( CMD_THEADR, names );
    KillArray( names );
}

static  void    AddLineInfo( object *obj, cg_linenum line, offset offs )
/**********************************************************************/
{
    cue_state           info;

    if( _IsModel( CGSW_GEN_DBG_DF )
      || _IsModel( CGSW_GEN_DBG_CV ) ) {
        CueFind( line, &info );
        if( _IsModel( CGSW_GEN_DBG_DF ) ) {
            if( _IsModel( CGSW_GEN_DBG_LOCALS )
              || _IsModel( CGSW_GEN_DBG_TYPES ) ) {
                 DFLineNum( &info, offs );
            }
        } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
            if( info.fno != CurrFNo ) {
                FlushLineNum( obj );
                InitLineInfo( obj );
                ChangeObjSrc( info.fno );
            }
        }
        line = info.line;
    }
    if( obj->lines->used >= BUFFSIZE - TOLERANCE ) {
        FlushLineNum( obj );
        InitLineInfo( obj );
    }
    if( obj->last_line ) {
        OutShort( obj->last_line, obj->lines );
        OutOffset( obj->last_offset, obj->lines );
    }
    obj->last_line = line;
    obj->last_offset = offs;
}

static  void    SetPendingLine( void )
/************************************/
{
    object              *obj;
    cg_linenum          line;
    offset              offs;

    obj = CurrSeg->obj;
    line = obj->pending_line_number;
    if( line == 0 )
        return;
    offs = CurrSeg->location;
    obj->pending_line_number = 0;
    obj->pending_label_line = false;
    if( obj->lines == NULL ) {
        obj->lines = InitArray( sizeof( byte ), MODEST_LINE, INCREMENT_LINE );
    }
    if( obj->lines->used == 0 ) {
        InitLineInfo( obj );
    } else if( obj->last_line ) {
        if( line == obj->last_line )
            return;
        if( offs <= obj->last_offset ) {
            obj->last_line = line;
            return;
        }
    }
    AddLineInfo( obj, line, offs );
}


void    OutDataByte( byte value )
/*******************************/
{
    SetPendingLine();
    CheckLEDataSize( sizeof( byte ), true );
    *(byte *)allocOutData( sizeof( byte ) ) = value;
}

void    OutDataShort( uint_16 value )
/***********************************/
{
    SetPendingLine();
    CheckLEDataSize( sizeof( uint_16 ), true );
    *(uint_16 *)allocOutData( sizeof( uint_16 ) ) = _TargetShort( value );
}


void    OutDataLong( uint_32 value )
/**********************************/
{
    SetPendingLine();
    CheckLEDataSize( sizeof( uint_32 ), true );
    *(uint_32 *)allocOutData( sizeof( uint_32 ) ) = _TargetLongInt( value );
}


void    OutAbsPatch( abs_patch *apatch, patch_attr attr )
/*******************************************************/
{
    long_offset value;

    CheckLEDataSize( 2 * sizeof( offset ), true );
    if( apatch->flags & AP_HAVE_VALUE ) {
        value = apatch->value;
        FreeAbsPatch( apatch );
    } else {
        apatch->obj = CurrSeg->obj;
        apatch->patch.ref = INVALID_OBJHANDLE;
        apatch->patch.where = CurrSeg->location - CurrSeg->obj->start + CurrSeg->data_prefix_size;
        apatch->patch.attr = attr;
        value = 0;
    }
    if( attr & LONG_PATCH ) {
        OutDataLong( value );
    } else if( attr & WORD_PATCH ) {
        OutDataShort( value );
    } else {
        OutDataByte( (byte)value );
    }
}


void    OutReloc( segment_id segid, fix_class class, bool rel )
/*************************************************************/
{
    index_rec   *rec;

    rec = AskSegIndex( segid );
    if( F_CLASS( class ) == F_BIG_OFFSET ) {
        CheckLEDataSize( 3 * sizeof( long_offset ), true );
    } else {
        CheckLEDataSize( 3 * sizeof( offset ), true );
    }
    DoFix( rec->base, rel, rec->btype, class, rec->sidx );
}


void OutSpecialCommon( import_handle imphdl, fix_class class, bool rel )
/**********************************************************************/
{
    CheckLEDataSize( 3 * sizeof( offset ), true );
    DoFix( imphdl, rel, BASE_IMP, class, 0 );
}


void    OutImport( cg_sym_handle sym, fix_class class, bool rel )
/***************************************************************/
{
    fe_attr     attr;

    attr = FEAttr( sym );
#if  _TARGET & _TARG_80386
    if( !rel && F_CLASS( class ) == F_OFFSET && (attr & FE_PROC) ) {
        if( (call_class_target)(pointer_uint)FindAuxInfoSym( sym, FEINF_CALL_CLASS_TARGET ) & FECALL_X86_FAR16_CALL ) {
            class |= F_FAR16;
        }
    }
#endif
    if( attr & FE_THREAD_DATA )
        class |= F_TLS;
    OutSpecialCommon( GenImport( sym, (class & F_ALT_DLLIMP) != 0 ), class, rel );
}


void    OutRTImportRel( rt_class rtindex, fix_class class, bool rel )
/*******************************************************************/
{
    import_handle   imphdl;

    imphdl = AskRTHandle( rtindex );
    if( imphdl == NOT_IMPORTED ) {
        imphdl = getImportHdl();
        CheckImportSwitch( false );
        TellRTHandle( rtindex, imphdl );
        OutName( AskRTName( rtindex ), &Imports );
        OutIdx( 0, &Imports );           /* type index*/
    }
    OutSpecialCommon( imphdl, class, rel );
}


void    OutRTImport( rt_class rtindex, fix_class class )
/******************************************************/
{
    OutRTImportRel( rtindex, class, ( F_CLASS( class ) == F_OFFSET || F_CLASS( class ) == F_LDR_OFFSET ) );
}

void    OutBckExport( const char *name, bool is_export )
/******************************************************/
{
    array_control       *obj_exports;
    object              *obj;

    obj = CurrSeg->obj;
    obj_exports = &obj->exports;
    if( obj_exports->used >= BUFFSIZE - TOLERANCE ) {
        EjectExports();
    }
    CheckExportSwitch( is_export );
    if( obj_exports->used == 0 ) {
        if( CurrSeg->btype == BASE_GRP ) {
            OutIdx( CurrSeg->base, obj_exports );   /* group index*/
        } else {
#if _TARGET & _TARG_80386
            OutIdx( FlatGIndex, obj_exports );      /* will be 0 if we have none */
#else
            OutIdx( 0, obj_exports );
#endif
        }
        OutIdx( obj->index, obj_exports );          /* segment index*/
    }
    OutName( name, obj_exports );
    OutOffset( (offset)CurrSeg->location, obj_exports );
    OutIdx( 0, obj_exports );                       /* type index*/
}

void    OutBckImport( const char *name, back_handle bck, fix_class class )
/************************************************************************/
{
    omf_idx     idx;

    idx = bck->imp;
    if( idx == NOT_IMPORTED ) {
        idx = getImportHdl();
        CheckImportSwitch( false );
        bck->imp = idx;
        OutName( name, &Imports );
        OutIdx( 0, &Imports );               /* type index*/
    }
    OutSpecialCommon( idx, class, false );
}


void    OutLineNum( cg_linenum  line, bool label_line )
/*****************************************************/
{
    object      *obj;

    obj = CurrSeg->obj;
    if( obj->pending_line_number == 0
        || obj->pending_label_line
        || obj->pending_line_number > line ) {
        obj->pending_line_number = line;
        obj->pending_label_line = label_line;
    }
}


unsigned        SavePendingLine( unsigned new_line )
/***************************************************
 * We're about to dump some alignment bytes. Save and restore
 * the pending_line_number field so the that line number info
 * offset is after the alignment.
 */
{
    unsigned    old;

    old = CurrSeg->obj->pending_line_number;
    CurrSeg->obj->pending_line_number = new_line;
    return( old );
}


void    OutDBytes( unsigned len, const byte *src )
/************************************************/
{
    unsigned    max;
    unsigned    n;

    SetPendingLine();
    CheckLEDataSize( sizeof( byte ), true );
    max = (BUFFSIZE - TOLERANCE) - ( CurrSeg->location - CurrSeg->obj->start + CurrSeg->data_prefix_size );
    while( len != 0 ) {
        if( len > max ) {
            n = max;
        } else {
            n = len;
        }
        memcpy( allocOutData( n ), src, n );
        src += n;
        len -= n;
        if( len == 0 )
            break;
        EjectLEData();
        OutLEDataStart( false );
        max = (BUFFSIZE - TOLERANCE);
    }
}


void    OutIBytes( byte pattern, offset len )
/*******************************************/
{
    omf_cmd         cmd;
    array_control   *obj_data;

    SetPendingLine();
    if( len <= TRADEOFF ) {
        for( ; len != 0; --len ) {
            OutDataByte( pattern );
        }
    } else {
        EjectLEData();
        OutLEDataStart( true );
        obj_data = &CurrSeg->obj->data;
#if _TARGET & _TARG_80386
        if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
            OutOffset( len, obj_data );         /* repeat count */
        } else {
            OutShort( len, obj_data );          /* repeat count */
        }
#else
        OutShort( len, obj_data );              /* repeat count */
#endif
        OutShort( 0, obj_data );                /* nesting count */
        OutByte( 1, obj_data );                 /* pattern length */
        OutByte( pattern, obj_data );
        if( CurrSeg->comdat_label != NULL ) {
#if _TARGET & _TARG_80386
            if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
                cmd = CMD_COMDAT32;
            } else {
                cmd = CMD_COMDAT;
            }
#else
            cmd = CMD_COMDAT;
#endif
        } else {
#if _TARGET & _TARG_80386
            if( _IsntTargetModel( CGSW_X86_EZ_OMF ) ) {
                cmd = CMD_LIDATA32;
            } else {
                cmd = CMD_LIDATA;
            }
#else
            cmd = CMD_LIDATA;
#endif
        }
        PutObjOMFRec( cmd, obj_data );
        IncLocation( len );
        SetMaxWritten();
    }
}


bool    NeedBaseSet( void )
/*************************/
{
    bool        need;

    need = ( CurrSeg->need_base_set != 0 );
    CurrSeg->need_base_set = false;
    return( need );
}


offset  AskMaxSize( void )
/************************/
{
    return( (offset)CurrSeg->max_size );
}

long_offset  AskBigLocation( void )
/*********************************/
{
    return( CurrSeg->location );
}

long_offset  AskBigMaxSize( void )
/********************************/
{
    return( CurrSeg->max_size );
}

void    TellObjNewLabel( cg_sym_handle lbl )
/******************************************/
{
    if( lbl == NULL )
        return;
    if( CurrSeg == NULL )
        return;
    if( codeSegId != CurrSeg->segid )
        return;

    /*
       We've got a data label going into a code segment.
       Make sure everything's OK with regards to COMDATs.
    */
    if( FEAttr( lbl ) & FE_COMMON ) {
        DoEmptyQueue();
        if( _IsModel( CGSW_GEN_DBG_DF ) ) {
            if( CurrSeg->comdat_symbol != NULL ) {
                DFSymRange( CurrSeg->comdat_symbol, (offset)CurrSeg->comdat_size );
            }
        }
    } else if( CurrSeg->comdat_symbol != NULL ) {
        DoEmptyQueue();
        SetUpObj( false );
        if( _IsModel( CGSW_GEN_DBG_DF ) ) {
            if( CurrSeg->comdat_symbol != NULL ) {
                DFSymRange( CurrSeg->comdat_symbol, (offset)CurrSeg->comdat_size );
            }
        }
    }
}

void    TellObjNewProc( cg_sym_handle proc )
/******************************************/
{
    segment_id  proc_segid;

    PUSH_OP( codeSegId );
        proc_segid = FESegID( proc );
        if( codeSegId != proc_segid ) {
            if( _IsModel( CGSW_GEN_DBG_DF ) ) {
                if( CurrSeg->comdat_symbol != NULL ) {
                    DFSymRange( CurrSeg->comdat_symbol, (offset)CurrSeg->comdat_size );
                }
            }
            if( CurrSeg->obj != NULL ) {
                DoEmptyQueue();
                FlushObject();
            }
            codeSegId = proc_segid;
            SetOP( codeSegId );
            CurrSeg->need_base_set = true;
            if( !CurrSeg->exec ) {
                Zoiks( ZOIKS_088 );
            }
        }
        if( FEAttr( proc ) & FE_COMMON ) {
            DoEmptyQueue();
            if( _IsModel( CGSW_GEN_DBG_DF ) ) {
                if( CurrSeg->comdat_symbol != NULL ) {
                    DFSymRange( CurrSeg->comdat_symbol, (offset)CurrSeg->comdat_size );
                }
            }
            CurrSeg->comdat_symbol = proc;
            CurrSeg->prefix_comdat_state = PCS_NEED;
        } else if( CurrSeg->comdat_symbol != NULL ) {
            DoEmptyQueue();
            SetUpObj( false );
            if( _IsModel( CGSW_GEN_DBG_DF ) ) {
                if( CurrSeg->comdat_symbol != NULL ) {
                    DFSymRange( CurrSeg->comdat_symbol, (offset)CurrSeg->comdat_size );
                }
            }
            NormalData();
        }
    POP_OP();
}

void     TellObjVirtFuncRef( void *cookie )
/*****************************************/
{
    virt_func_ref_list  *new_vf;

    PUSH_OP( codeSegId );
        new_vf = CGAlloc( sizeof( virt_func_ref_list ) );
        new_vf->cookie = cookie;
        new_vf->next = CurrSeg->virt_func_refs;
        CurrSeg->virt_func_refs = new_vf;
    POP_OP();
}

static bool     InlineFunction( cg_sym_handle sym )
/*************************************************/
{
    if( FEAttr( sym ) & FE_PROC ) {
        if( FindAuxInfoSym( sym, FEINF_CALL_BYTES ) != NULL
          || ((call_class)(pointer_uint)FindAuxInfoSym( sym, FEINF_CALL_CLASS ) & FECALL_GEN_MAKE_CALL_INLINE) ) {
            return( true );
        }
    }
    return( false );
}

segment_id      AskSegID( pointer hdl, cg_class class )
/*****************************************************/
{
    switch( class ) {
    case CG_FE:
        if( InlineFunction( (cg_sym_handle)hdl ) ) {
            return( codeSegId );    /* AskCodeSeg() */
        }
        return( FESegID( (cg_sym_handle)hdl ) );
    case CG_BACK:
        return( ((back_handle)hdl)->segid );
    case CG_TBL:
    case CG_VTB:
        return( codeSegId );        /* AskCodeSeg() */
    case CG_CLB:
        return( altCodeSegId );     /* AskAltCodeSeg() */
    default:
        return( dataSegId );        /* AskBackSeg() */
    }
}

bool    AskNameIsCode( pointer hdl, cg_class class )
/**************************************************/
{
    switch( class ) {
    case CG_FE:
        return( (FEAttr( hdl ) & FE_PROC) != 0 );
    case CG_TBL:
    case CG_VTB:
    case CG_CLB:
        return( true );
    default:
        break;
    }
    return( false );
}

bool    AskNameIsROM( pointer hdl, cg_class class )
/*************************************************/
{
    return( AskSegIsROM( AskSegID( hdl, class ) ) );
}

bool SymIsExported( cg_sym_handle sym )
/*************************************/
{
    bool        exported;

    exported = false;
    if( sym != NULL ) {
        if( FEAttr( sym ) & FE_DLLEXPORT ) {
            exported = true;
        } else if( (call_class)(pointer_uint)FindAuxInfoSym( sym, FEINF_CALL_CLASS ) & FECALL_GEN_DLL_EXPORT ) {
            exported = true;
        }
    }
    return( exported );
}
