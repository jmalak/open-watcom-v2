/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML init banner layout default values
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  init banners for default layout                                         */
/***************************************************************************/

void    banner_defaults( void )
{
    banner_lay_tag  *   ban;
    banner_lay_tag  *   wk;
    region_lay_tag  *   reg;
    region_lay_tag  *   regwk;

    static  char        z0[2] = "0";
    static  char        n1[2] = "1";
    static  char        n2[2] = "2";
    static  char        n3[2] = "3";
    static  char        n4[2] = "4";
    static  char        leftc[] = "left";
    static  char        extendc[] = "extend";
    static  char        nr[] = "pgnumr";

    ban = mem_alloc( sizeof( banner_lay_tag ) );
    layout_work.banner = ban;

    ban->next = NULL;
    ban->region = NULL;
    ban->by_line = NULL;
    lay_init_su( z0, &(ban->left_adjust) );
    lay_init_su( z0, &(ban->right_adjust) );
    lay_init_su( n3, &(ban->depth) );
    ban->place = bottom_place;
    ban->docsect = head0_ban;
    ban->ban_left_adjust = 0;
    ban->ban_right_adjust = 0;
    ban->ban_depth = 0;
    ban->next_refnum = 2;
    ban->style = no_content;

    reg = mem_alloc( sizeof( region_lay_tag ) );
    ban->region = reg;
    reg->next = NULL;
    reg->reg_indent = 0;
    reg->reg_hoffset = 0;
    reg->reg_width = 0;
    reg->reg_voffset = 0;
    reg->reg_depth = 0;
    lay_init_su( z0, &(reg->indent) );
    lay_init_su( leftc, &(reg->hoffset) );
    lay_init_su( extendc, &(reg->width) );
    lay_init_su( n2, &(reg->voffset) );
    lay_init_su( n1, &(reg->depth) );
    reg->font = 0;
    reg->refnum = 1;
    reg->region_position = pos_left;
    reg->pouring = last_pour;
    reg->script_format = true;
    reg->contents.content_type = string_content;
    strcpy_s( reg->contents.string, sizeof( "/&$htext0.// &$pgnuma./" ),
              "/&$htext0.// &$pgnuma./" );
    reg->script_region[0].len = 0;
    reg->script_region[1].len = 0;
    reg->script_region[2].len = 0;
    reg->script_region[0].string = NULL;
    reg->script_region[1].string = NULL;
    reg->script_region[2].string = NULL;
    reg->final_content[0].len = 0;
    reg->final_content[1].len = 0;
    reg->final_content[2].len = 0;
    reg->final_content[0].hoffset = 0;
    reg->final_content[1].hoffset = 0;
    reg->final_content[2].hoffset = 0;
    reg->final_content[0].string = NULL;
    reg->final_content[1].string = NULL;
    reg->final_content[2].string = NULL;


    wk = ban;
    ban = mem_alloc( sizeof( banner_lay_tag ) );
    memcpy( ban, wk, sizeof( banner_lay_tag ) );
    wk->next = ban;

    ban->docsect = body_ban;

    regwk = mem_alloc( sizeof( region_lay_tag ) );
    memcpy( regwk, reg, sizeof( region_lay_tag ) );
    reg = regwk;
    ban->region = reg;
    reg->next = NULL;
    strcpy_s( reg->contents.string, sizeof( "/&$htext1.// &$pgnuma./" ),
              "/&$htext1.// &$pgnuma./" );
    reg->script_region[0].len = 0;
    reg->script_region[1].len = 0;
    reg->script_region[2].len = 0;
    reg->script_region[0].string = NULL;
    reg->script_region[1].string = NULL;
    reg->script_region[2].string = NULL;


    wk = ban;
    ban = mem_alloc( sizeof( banner_lay_tag ) );
    memcpy( ban, wk, sizeof( banner_lay_tag ) );
    wk->next = ban;

    lay_init_su( n4, &(ban->depth) );
    ban->docsect = abstract_ban;
    ban->style = pgnumr_content;

    regwk = mem_alloc( sizeof( region_lay_tag ) );
    memcpy( regwk, reg, sizeof( region_lay_tag ) );
    reg = regwk;
    ban->region = reg;
    reg->next = NULL;
    lay_init_su( n3, &(reg->voffset) );
    reg->region_position = pos_center;
    reg->script_format = false;
    reg->contents.content_type = pgnumr_content;
    strcpy_s( reg->contents.string, sizeof( reg->contents ), nr );
    reg->script_region[0].len = 0;
    reg->script_region[1].len = 0;
    reg->script_region[2].len = 0;
    reg->script_region[0].string = NULL;
    reg->script_region[1].string = NULL;
    reg->script_region[2].string = NULL;


    wk = ban;
    ban = mem_alloc( sizeof( banner_lay_tag ) );
    memcpy( ban, wk, sizeof( banner_lay_tag ) );
    wk->next = ban;

    ban->docsect = preface_ban;

    regwk = mem_alloc( sizeof( region_lay_tag ) );
    memcpy( regwk, reg, sizeof( region_lay_tag ) );
    reg = regwk;
    ban->region = reg;
    reg->next = NULL;

    wk = ban;
    ban = mem_alloc( sizeof( banner_lay_tag ) );
    memcpy( ban, wk, sizeof( banner_lay_tag ) );
    wk->next = ban;

    lay_init_su( n3, &(ban->depth) );
    ban->place = top_place;
    ban->docsect = toc_ban;
    ban->style = no_content;

    regwk = mem_alloc( sizeof( region_lay_tag ) );
    memcpy( regwk, reg, sizeof( region_lay_tag ) );
    reg = regwk;
    ban->region = reg;
    reg->next = NULL;
    lay_init_su( n1, &(reg->voffset) );
    reg->font = 3;
    reg->contents.content_type = string_content;
    strcpy_s( reg->contents.string, sizeof( "Table of Contents" ),
              "Table of Contents" );
    reg->script_region[0].len = 0;
    reg->script_region[1].len = 0;
    reg->script_region[2].len = 0;
    reg->script_region[0].string = NULL;
    reg->script_region[1].string = NULL;
    reg->script_region[2].string = NULL;


    wk = ban;
    ban = mem_alloc( sizeof( banner_lay_tag ) );
    memcpy( ban, wk, sizeof( banner_lay_tag ) );
    wk->next = ban;

    ban->place = top_place;
    ban->docsect = figlist_ban;

    regwk = mem_alloc( sizeof( region_lay_tag ) );
    memcpy( regwk, reg, sizeof( region_lay_tag ) );
    reg = regwk;
    ban->region = reg;
    reg->next = NULL;
    strcpy_s( reg->contents.string, sizeof( "List of Figures" ),
              "List of Figures" );


    wk = ban;
    ban = mem_alloc( sizeof( banner_lay_tag ) );
    memcpy( ban, wk, sizeof( banner_lay_tag ) );
    wk->next = ban;

    ban->place = top_place;
    ban->docsect = index_ban;

    regwk = mem_alloc( sizeof( region_lay_tag ) );
    memcpy( regwk, reg, sizeof( region_lay_tag ) );
    reg = regwk;
    ban->region = reg;
    reg->next = NULL;
    strcpy_s( reg->contents.string, sizeof( "Index" ), "Index" );


    wk = ban;
    ban = mem_alloc( sizeof( banner_lay_tag ) );
    memcpy( ban, wk, sizeof( banner_lay_tag ) );
    wk->next = ban;

    ban->place = top_place;
    ban->docsect = letter_ban;


    regwk = mem_alloc( sizeof( region_lay_tag ) );
    memcpy( regwk, reg, sizeof( region_lay_tag ) );
    reg = regwk;
    ban->region = reg;
    reg->next = NULL;
    lay_init_su( z0, &(reg->voffset) );
    reg->region_position = pos_left;
    reg->font = 0;
    reg->pouring = no_pour;
    reg->script_format = true;
    strcpy_s( reg->contents.string,
              sizeof( "/&date.// Page &$pgnuma./" ),
              "/&date.// Page &$pgnuma./" );


    wk = ban;
    ban = mem_alloc( sizeof( banner_lay_tag ) );
    memcpy( ban, wk, sizeof( banner_lay_tag ) );
    wk->next = ban;

    ban->place = topodd_place;
    ban->docsect = letlast_ban;


    regwk = mem_alloc( sizeof( region_lay_tag ) );
    memcpy( regwk, reg, sizeof( region_lay_tag ) );
    reg = regwk;
    ban->region = reg;
    reg->next = NULL;

}


