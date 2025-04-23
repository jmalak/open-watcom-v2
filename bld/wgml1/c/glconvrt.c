/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :CONVERT LAYOUT tag
*                              and file output for all subtags
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/* Layout attributes as character strings                                  */
/***************************************************************************/

const char    * const lay_att_names[] = {
//   18 is enough for longest attribute name  ( extract_threshold )
    { "DUMMY" },                       // enum zero not used
    #define pick( name, funci, funco, result ) { #name },
    #include "glayutil.h"
    #undef pick
};


/***************************************************************************/
/*                                                                         */
/*  Format: :CONVERT file='file name'.                                     */
/*                                                                         */
/*  alternate, undocumented but used format:                               */
/*                                                                         */
/*          :CONVERT.FILE.EXT                                              */
/*                                                                         */
/* Convert the current layout into the specified file name. The resulting  */
/* file will contain the entire layout in a readable form.                 */
/***************************************************************************/

/***************************************************************************/
/*   output for :ABSTRACT or :PREFACE values                               */
/***************************************************************************/
static  void    put_lay_abspref( FILE * layfile, abspref_lay_tag * ap,
                                 hx_sect_lay_tag * apsect, char * name )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":%s\n", name );

    for( k = 0, lay_attr = abspref_att[k]; lay_attr > 0; k++, lay_attr = abspref_att[k] ) {

        switch( lay_attr ) {
        case e_post_skip:
            o_space_unit( layfile, lay_attr, &apsect->post_skip );
            break;
        case e_pre_top_skip:
            o_space_unit( layfile, lay_attr, &apsect->pre_top_skip );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &apsect->text_font );
            break;
        case e_spacing:
            o_spacing( layfile, lay_attr, &apsect->spacing );
            break;
        case e_header:
            o_yes_no( layfile, lay_attr, &apsect->header );
            break;
        case e_abstract_string:
            if( *name == 'A' ) {        // :Abstract output
                o_xx_string( layfile, lay_attr, ap->string );
            }
            break;
        case e_preface_string:
            if( *name == 'P' ) {        // :Preface output
                o_xx_string( layfile, lay_attr, ap->string );
            }
            break;
        case e_page_eject:
            o_page_eject( layfile, lay_attr, &ap->page_eject );
            break;
        case e_page_reset:
            o_yes_no( layfile, lay_attr, &ap->page_reset );
            break;
        case e_columns:
            o_int8( layfile, lay_attr, &ap->columns );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}

static  void    put_lay_abstract( FILE * layfile, layout_data * lay )
{
    put_lay_abspref( layfile, &(lay->abstract), &(lay->hx.hx_sect[hds_abstract]), "ABSTRACT" );
}

static  void    put_lay_preface( FILE * layfile, layout_data * lay )
{
    put_lay_abspref( layfile, &(lay->preface), &(lay->hx.hx_sect[hds_preface]), "PREFACE" );
}


/***************************************************************************/
/*   :ADDRESS   output address attribute values                            */
/***************************************************************************/
static  void    put_lay_address( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":ADDRESS\n" );

    for( k = 0, lay_attr = address_att[k]; lay_attr > 0; k++, lay_attr = address_att[k] ) {

        switch( lay_attr ) {
        case e_left_adjust:
            o_space_unit( layfile, lay_attr, &lay->address.left_adjust );
            break;
        case e_right_adjust:
            o_space_unit( layfile, lay_attr, &lay->address.right_adjust );
            break;
        case e_page_position:
            o_page_position( layfile, lay_attr, &lay->address.page_position );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->address.font );
            break;
        case e_pre_skip:
            o_space_unit( layfile, lay_attr, &lay->address.pre_skip );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :ALINE     output aline attribute values                              */
/***************************************************************************/
static  void    put_lay_aline( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":ALINE\n" );

    for( k = 0, lay_attr = aline_att[k]; lay_attr > 0; k++, lay_attr = aline_att[k] ) {

        switch( lay_attr ) {
        case e_skip:
            o_space_unit( layfile, lay_attr, &lay->aline.skip );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :APPENDIX  output attribute values                                    */
/***************************************************************************/
static  void    put_lay_appendix( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":APPENDIX\n" );

    for( k = 0, lay_attr = appendix_att[k]; lay_attr > 0; k++, lay_attr = appendix_att[k] ) {

        switch( lay_attr ) {
        case e_indent:
            o_space_unit( layfile, lay_attr, &lay->hx.hx_head[hds_appendix].indent );
            break;
        case e_pre_top_skip:
            o_space_unit( layfile, lay_attr, &lay->hx.hx_sect[hds_appendix].pre_top_skip );
            break;
        case e_pre_skip:
            o_space_unit( layfile, lay_attr, &lay->hx.hx_head[hds_appendix].pre_skip );
            break;
        case e_post_skip:
            o_space_unit( layfile, lay_attr, &lay->hx.hx_sect[hds_appendix].post_skip );
            break;
        case e_spacing:
            o_spacing( layfile, lay_attr, &lay->hx.hx_sect[hds_appendix].spacing );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->hx.hx_sect[hds_appendix].text_font );
            break;
        case e_number_font:
            o_font_number( layfile, lay_attr, &lay->hx.hx_head[hds_appendix].number_font );
            break;
        case e_number_form:
            o_number_form( layfile, lay_attr, &lay->hx.hx_head[hds_appendix].number_form );
            break;
        case e_page_position:
            o_page_position( layfile, lay_attr, &lay->hx.hx_head[hds_appendix].line_position );
            break;
        case e_number_style:
            o_number_style( layfile, lay_attr, &lay->hx.hx_head[hds_appendix].number_style );
            break;
        case e_page_eject:
            o_page_eject( layfile, lay_attr, &lay->hx.hx_head[hds_appendix].page_eject );
            break;
        case e_line_break:
            o_yes_no( layfile, lay_attr, &lay->hx.hx_head[hds_appendix].line_break );
            break;
        case e_display_heading:
            o_yes_no( layfile, lay_attr, &lay->hx.hx_head[hds_appendix].display_heading );
            break;
        case e_number_reset:
            o_yes_no( layfile, lay_attr, &lay->hx.hx_head[hds_appendix].number_reset );
            break;
        case e_case:
            o_case( layfile, lay_attr, &lay->hx.hx_head[hds_appendix].hd_case );
            break;
        case e_align:
            o_space_unit( layfile, lay_attr, &lay->hx.hx_head[hds_appendix].align );
            break;
        case e_header:
            o_yes_no( layfile, lay_attr, &lay->hx.hx_sect[hds_appendix].header );
            break;
        case e_appendix_string:
            o_xx_string( layfile, lay_attr, lay->appendix.string );
            break;
        case e_page_reset:
            o_yes_no( layfile, lay_attr, &lay->appendix.page_reset );
            break;
        case e_section_eject:
            o_page_eject( layfile, lay_attr, &lay->appendix.section_eject );
            break;
        case e_columns:
            o_int8( layfile, lay_attr, &lay->appendix.columns );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :AUTHOR   output author attribute values                              */
/***************************************************************************/
static  void    put_lay_author( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":AUTHOR\n" );

    for( k = 0, lay_attr = author_att[k]; lay_attr > 0; k++, lay_attr = author_att[k] ) {

        switch( lay_attr ) {
        case e_left_adjust:
            o_space_unit( layfile, lay_attr, &lay->author.left_adjust );
            break;
        case e_right_adjust:
            o_space_unit( layfile, lay_attr, &lay->author.right_adjust );
            break;
        case e_page_position:
            o_page_position( layfile, lay_attr, &lay->author.page_position );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->author.font );
            break;
        case e_pre_skip:
            o_space_unit( layfile, lay_attr, &lay->author.pre_skip );
            break;
        case e_skip:
            o_space_unit( layfile, lay_attr, &lay->author.skip );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   output for :BACKM or :BODY values                                     */
/***************************************************************************/
static  void    put_lay_backbod( FILE * layfile, backbod_lay_tag * bb,
                                 hx_sect_lay_tag * bbsect, char * name )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":%s\n", name );

    for( k = 0, lay_attr = backbod_att[k]; lay_attr > 0; k++, lay_attr = backbod_att[k] ) {

        switch( lay_attr ) {
        case e_post_skip:
            o_space_unit( layfile, lay_attr, &bbsect->post_skip );
            break;
        case e_pre_top_skip:
            o_space_unit( layfile, lay_attr, &bbsect->pre_top_skip );
            break;
        case e_header:
            o_yes_no( layfile, lay_attr, &bbsect->header );
            break;
        case e_body_string:
            if( *(name + 1) == 'O') {   // BODY tag
                o_xx_string( layfile, lay_attr, bb->string );
            }
            break;
        case e_backm_string:
            if( *(name + 1) == 'A') {   // BACKM tag
                o_xx_string( layfile, lay_attr, bb->string );
            }
            break;
        case e_page_eject:
            o_page_eject( layfile, lay_attr, &bb->page_eject );
            break;
        case e_page_reset:
            o_yes_no( layfile, lay_attr, &bb->page_reset );
            break;
        case e_columns:
            if( *(name + 1) == 'A') {   // BACKM tag
                o_int8( layfile, lay_attr, &bb->columns );
            }
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &bbsect->text_font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}

static  void    put_lay_backm( FILE * layfile, layout_data * lay )
{
    put_lay_backbod( layfile, &(lay->backm), &(lay->hx.hx_sect[hds_backm]), "BACKM" );
}

static  void    put_lay_body( FILE * layfile, layout_data * lay )
{
    put_lay_backbod( layfile, &(lay->body), &(lay->hx.hx_sect[hds_body]), "BODY" );
}


/***************************************************************************/
/*  output a banner region                                                 */
/***************************************************************************/
static  void    put_lay_region( FILE * layfile, region_lay_tag * reg )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":BANREGION\n" );

    for( k = 0, lay_attr = banregion_att[k]; lay_attr > 0;
         k++, lay_attr = banregion_att[k] ) {

        switch( lay_attr ) {
        case e_indent:
            o_space_unit( layfile, lay_attr, &reg->indent );
            break;
        case e_hoffset:
            o_space_unit( layfile, lay_attr, &reg->hoffset );
            break;
        case e_width:
            o_space_unit( layfile, lay_attr, &reg->width );
            break;
        case e_voffset:
            o_space_unit( layfile, lay_attr, &reg->voffset );
            break;
        case e_depth:
            o_space_unit( layfile, lay_attr, &reg->depth );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &reg->font );
            break;
        case e_refnum:
            o_uint8( layfile, lay_attr, &reg->refnum );
            break;
        case e_region_position:
            o_page_position( layfile, lay_attr, &reg->region_position );
            break;
        case e_pouring:
            o_pouring( layfile, lay_attr, &reg->pouring );
            break;
        case e_script_format:
            o_yes_no( layfile, lay_attr, &reg->script_format );
            break;
        case e_contents:
            o_content( layfile, lay_attr, &reg->contents );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
    fprintf( layfile, ":eBANREGION\n" );
}


/***************************************************************************/
/*  output a single banner with regions                                    */
/***************************************************************************/
static  void    put_lay_single_ban( FILE * layfile, banner_lay_tag * ban )
{
    int                 k;
    lay_att             lay_attr;
    region_lay_tag  *   reg;

    fprintf( layfile, ":BANNER\n" );

    for( k = 0, lay_attr = banner_att[k]; lay_attr > 0; k++, lay_attr = banner_att[k] ) {

        switch( lay_attr ) {
        case e_left_adjust:
            o_space_unit( layfile, lay_attr, &ban->left_adjust );
            break;
        case e_right_adjust:
            o_space_unit( layfile, lay_attr, &ban->right_adjust );
            break;
        case e_depth:
            o_space_unit( layfile, lay_attr, &ban->depth );
            break;
        case e_place:
            o_place( layfile, lay_attr, &ban->place );
            break;
        case e_docsect:
            o_docsect( layfile, lay_attr, &ban->docsect );
            break;
        case e_refplace:
        case e_refdoc:
            /* no action these are only used for input */
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
    reg = ban->region;
    while( reg != NULL ) {
        put_lay_region( layfile, reg );
        reg = reg->next;
    }

    fprintf( layfile, ":eBANNER\n" );
}


/***************************************************************************/
/*   :BANNER   output all banners                                          */
/***************************************************************************/
static  void    put_lay_banner( FILE * layfile, layout_data * lay )
{
    banner_lay_tag      *   ban;

    ban = lay->banner;
    while( ban != NULL ) {
        put_lay_single_ban( layfile, ban );
        ban = ban->next;
    }
}


/***************************************************************************/
/*   :DATE     output date attribute values                                */
/***************************************************************************/
static  void    put_lay_date( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":DATE\n" );

    for( k = 0, lay_attr = date_att[k]; lay_attr > 0; k++, lay_attr = date_att[k] ) {

        switch( lay_attr ) {
        case e_date_form:
            o_date_form( layfile, lay_attr, lay->date.date_form );
            break;
        case e_left_adjust:
            o_space_unit( layfile, lay_attr, &lay->date.left_adjust );
            break;
        case e_right_adjust:
            o_space_unit( layfile, lay_attr, &lay->date.right_adjust );
            break;
        case e_page_position:
            o_page_position( layfile, lay_attr, &lay->date.page_position );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->date.font );
            break;
        case e_pre_skip:
            o_space_unit( layfile, lay_attr, &lay->date.pre_skip );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :DD        output definition data attribute values                    */
/***************************************************************************/
static  void    put_lay_dd( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":DD\n" );

    for( k = 0, lay_attr = dd_att[k]; lay_attr > 0; k++, lay_attr = dd_att[k] ) {

        switch( lay_attr ) {
        case e_line_left:
            o_space_unit( layfile, lay_attr, &lay->dd.line_left );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->dd.font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :DEFAULT   output default attribute values                            */
/***************************************************************************/
static  void    put_lay_default( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":DEFAULT\n" );

    for( k = 0, lay_attr = default_att[k]; lay_attr > 0; k++, lay_attr = default_att[k] ) {

        switch( lay_attr ) {
        case e_spacing:
            o_spacing( layfile, lay_attr, &lay->defaults.spacing );
            break;
        case e_columns:
            o_int8( layfile, lay_attr, &lay->defaults.columns );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->defaults.font );
            break;
        case e_justify:
            o_yes_no( layfile, lay_attr, &lay->defaults.justify );
            break;
        case e_input_esc:
            o_char( layfile, lay_attr, &lay->defaults.input_esc );
            break;
        case e_gutter:
            o_space_unit( layfile, lay_attr, &lay->defaults.gutter );
            break;
        case e_binding:
            o_space_unit( layfile, lay_attr, &lay->defaults.binding );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :DL        output attribute values                                    */
/***************************************************************************/
static  void    put_lay_dl( FILE * layfile, layout_data * lay )
{
    dl_lay_level    *   dl_layout;
    int                 k;
    lay_att             lay_attr;

    dl_layout = lay->dl.first;

    while( dl_layout != NULL ) {
        fprintf( layfile, ":DL\n" );

        for( k = 0, lay_attr = dl_att[k]; lay_attr > 0; k++, lay_attr = dl_att[k] ) {

            switch( lay_attr ) {
            case e_level:
                o_int8( layfile, lay_attr, &dl_layout->level );
                break;
            case e_left_indent:
                o_space_unit( layfile, lay_attr, &dl_layout->left_indent );
                break;
            case e_right_indent:
                o_space_unit( layfile, lay_attr, &dl_layout->right_indent );
                break;
            case e_pre_skip:
                o_space_unit( layfile, lay_attr, &dl_layout->pre_skip );
                break;
            case e_skip:
                o_space_unit( layfile, lay_attr, &dl_layout->skip );
                break;
            case e_spacing:
                o_spacing( layfile, lay_attr, &dl_layout->spacing );
                break;
            case e_post_skip:
                o_space_unit( layfile, lay_attr, &dl_layout->post_skip );
                break;
            case e_align:
                o_space_unit( layfile, lay_attr, &dl_layout->align );
                break;
            case e_line_break:
                o_yes_no( layfile, lay_attr, &dl_layout->line_break );
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
        dl_layout = dl_layout->next;
    }
}


/***************************************************************************/
/*   :DOCNUM    output documentnumber values                               */
/***************************************************************************/
static  void    put_lay_docnum( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":DOCNUM\n" );

    for( k = 0, lay_attr = docnum_att[k]; lay_attr > 0; k++, lay_attr = docnum_att[k] ) {

        switch( lay_attr ) {
        case e_left_adjust:
            o_space_unit( layfile, lay_attr, &lay->docnum.left_adjust );
            break;
        case e_right_adjust:
            o_space_unit( layfile, lay_attr, &lay->docnum.right_adjust );
            break;
        case e_page_position:
            o_page_position( layfile, lay_attr, &lay->docnum.page_position );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->docnum.font );
            break;
        case e_pre_skip:
            o_space_unit( layfile, lay_attr, &lay->docnum.pre_skip );
            break;
        case e_docnum_string:
            o_xx_string( layfile, lay_attr, lay->docnum.string );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FIG       output figur attribute values                              */
/***************************************************************************/
static  void    put_lay_fig( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":FIG\n" );

    for( k = 0, lay_attr = fig_att[k]; lay_attr > 0; k++, lay_attr = fig_att[k] ) {

        switch( lay_attr ) {
        case e_left_adjust:
            o_space_unit( layfile, lay_attr, &lay->fig.left_adjust );
            break;
        case e_right_adjust:
            o_space_unit( layfile, lay_attr, &lay->fig.right_adjust );
            break;
        case e_pre_skip:
            o_space_unit( layfile, lay_attr, &lay->fig.pre_skip );
            break;
        case e_post_skip:
            o_space_unit( layfile, lay_attr, &lay->fig.post_skip );
            break;
        case e_spacing:
            o_spacing( layfile, lay_attr, &lay->fig.spacing );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->fig.font );
            break;
        case e_default_place:
            o_place( layfile, lay_attr, &lay->fig.default_place );
            break;
        case e_default_frame:
            o_default_frame( layfile, lay_attr, &lay->fig.default_frame );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FIGCAP    output figure caption attribute values                     */
/***************************************************************************/
static  void    put_lay_figcap( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":FIGCAP\n" );

    for( k = 0, lay_attr = figcap_att[k]; lay_attr > 0; k++, lay_attr = figcap_att[k] ) {

        switch( lay_attr ) {
        case e_pre_lines:
            o_space_unit( layfile, lay_attr, &lay->figcap.pre_lines );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->figcap.font );
            break;
        case e_figcap_string:
            o_xx_string( layfile, lay_attr, lay->figcap.string );
            break;
        case e_string_font:
            o_font_number( layfile, lay_attr, &lay->figcap.string_font );
            break;
        case e_delim:
            o_char( layfile, lay_attr, &lay->figcap.delim );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FIGDESC   output figure description attribute values                 */
/***************************************************************************/
static  void    put_lay_figdesc( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":FIGDESC\n" );

    for( k = 0, lay_attr = figdesc_att[k]; lay_attr > 0; k++, lay_attr = figdesc_att[k] ) {

        switch( lay_attr ) {
        case e_pre_lines:
            o_space_unit( layfile, lay_attr, &lay->figdesc.pre_lines );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->figdesc.font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FIGLIST   output figure list attribute values                        */
/***************************************************************************/
static  void    put_lay_figlist( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":FIGLIST\n" );

    for( k = 0, lay_attr = figlist_att[k]; lay_attr > 0; k++, lay_attr = figlist_att[k] ) {

        switch( lay_attr ) {
        case e_left_adjust:
            o_space_unit( layfile, lay_attr, &lay->figlist.left_adjust );
            break;
        case e_right_adjust:
            o_space_unit( layfile, lay_attr, &lay->figlist.right_adjust );
            break;
        case e_skip:
            o_space_unit( layfile, lay_attr, &lay->figlist.skip );
            break;
        case e_spacing:
            o_spacing( layfile, lay_attr, &lay->figlist.spacing );
            break;
        case e_columns:
            o_int8( layfile, lay_attr, &lay->figlist.columns );
            break;
        case e_fill_string:
            o_xx_string( layfile, lay_attr, lay->figlist.fill_string );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FLPGNUM  output figlist number attribute values                      */
/***************************************************************************/
static  void    put_lay_flpgnum( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":FLPGNUM\n" );

    for( k = 0, lay_attr = flpgnum_att[k]; lay_attr > 0; k++, lay_attr = flpgnum_att[k] ) {

        switch( lay_attr ) {
        case e_size:
            o_space_unit( layfile, lay_attr, &lay->flpgnum.size );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->flpgnum.font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FN        output footnote attribute values                            */
/***************************************************************************/
static  void    put_lay_fn( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":FN\n" );

    for( k = 0, lay_attr = fn_att[k]; lay_attr > 0; k++, lay_attr = fn_att[k] ) {

        switch( lay_attr ) {
        case e_line_indent:
            o_space_unit( layfile, lay_attr, &lay->fn.line_indent );
            break;
        case e_align:
            o_space_unit( layfile, lay_attr, &lay->fn.align );
            break;
        case e_pre_lines:
            o_space_unit( layfile, lay_attr, &lay->fn.pre_lines );
            break;
        case e_skip:
            o_space_unit( layfile, lay_attr, &lay->fn.skip );
            break;
        case e_spacing:
            o_spacing( layfile, lay_attr, &lay->fn.spacing );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->fn.font );
            break;
        case e_number_font:
            o_font_number( layfile, lay_attr, &lay->fn.number_font );
            break;
        case e_number_style:
            o_number_style( layfile, lay_attr, &lay->fn.number_style );
            break;
        case e_frame:
            o_frame( layfile, lay_attr, &lay->fn.frame );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FNREF     output footnote attribute values                            */
/***************************************************************************/
static  void    put_lay_fnref( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":FNREF\n" );

    for( k = 0, lay_attr = fnref_att[k]; lay_attr > 0; k++, lay_attr = fnref_att[k] ) {

        switch( lay_attr ) {
        case e_font:
            o_font_number( layfile, lay_attr, &lay->fnref.font );
            break;
        case e_number_style:
            o_number_style( layfile, lay_attr, &lay->fnref.number_style );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :GL        output attribute values                                    */
/***************************************************************************/
static  void    put_lay_gl( FILE * layfile, layout_data * lay )
{
    gl_lay_level    *   gl_layout;
    int                 k;
    lay_att             lay_attr;

    gl_layout = lay->gl.first;

    while( gl_layout != NULL ) {
        fprintf( layfile, ":GL\n" );

        for( k = 0, lay_attr = gl_att[k]; lay_attr > 0; k++, lay_attr = gl_att[k] ) {

            switch( lay_attr ) {
            case e_level:
                o_int8( layfile, lay_attr, &gl_layout->level );
                break;
            case e_left_indent:
                o_space_unit( layfile, lay_attr, &gl_layout->left_indent );
                break;
            case e_right_indent:
                o_space_unit( layfile, lay_attr, &gl_layout->right_indent );
                break;
            case e_pre_skip:
                o_space_unit( layfile, lay_attr, &gl_layout->pre_skip );
                break;
            case e_skip:
                o_space_unit( layfile, lay_attr, &gl_layout->skip );
                break;
            case e_spacing:
                o_spacing( layfile, lay_attr, &gl_layout->spacing );
                break;
            case e_post_skip:
                o_space_unit( layfile, lay_attr, &gl_layout->post_skip );
                break;
            case e_align:
                o_space_unit( layfile, lay_attr, &gl_layout->align );
                break;
            case e_delim:
                o_char( layfile, lay_attr, &gl_layout->delim );
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
        gl_layout = gl_layout->next;
    }
}


/***************************************************************************/
/*   :Hx        output header attribute values for :H0 - :H6               */
/***************************************************************************/
static  void    put_lay_hx( FILE * layfile, layout_data * lay )
{
    int                 k;
    int                 lvl;
    lay_att             lay_attr;

    for( lvl = 0; lvl < 7; ++lvl ) {

        fprintf( layfile, ":H%c\n", '0' + lvl );

        for( k = 0, lay_attr = hx_att[k]; lay_attr > 0; k++, lay_attr = hx_att[k] ) {

            switch( lay_attr ) {
            case e_group:
                o_int8( layfile, lay_attr, &lay->hx.group );
                break;
            case e_indent:
                o_space_unit( layfile, lay_attr, &lay->hx.hx_head[lvl].indent );
                break;
            case e_pre_top_skip:
                o_space_unit( layfile, lay_attr, &lay->hx.hx_sect[lvl].pre_top_skip );
                break;
            case e_pre_skip:
                o_space_unit( layfile, lay_attr, &lay->hx.hx_head[lvl].pre_skip );
                break;
            case e_post_skip:
                o_space_unit( layfile, lay_attr, &lay->hx.hx_sect[lvl].post_skip );
                break;
            case e_spacing:
                o_spacing( layfile, lay_attr, &lay->hx.hx_sect[lvl].spacing );
                break;
            case e_font:
                o_font_number( layfile, lay_attr, &lay->hx.hx_sect[lvl].text_font );
                break;
            case e_number_font:
                o_font_number( layfile, lay_attr, &lay->hx.hx_head[lvl].number_font );
                break;
            case e_number_form:
                o_number_form( layfile, lay_attr, &lay->hx.hx_head[lvl].number_form );
                break;
            case e_page_position:
                o_page_position( layfile, lay_attr, &lay->hx.hx_head[lvl].line_position );
                break;
            case e_number_style:
                o_number_style( layfile, lay_attr, &lay->hx.hx_head[lvl].number_style );
                break;
            case e_page_eject:
                o_page_eject( layfile, lay_attr, &lay->hx.hx_head[lvl].page_eject );
                break;
            case e_line_break:
                o_yes_no( layfile, lay_attr, &lay->hx.hx_head[lvl].line_break );
                break;
            case e_display_heading:
                o_yes_no( layfile, lay_attr, &lay->hx.hx_head[lvl].display_heading );
                break;
            case e_number_reset:
                o_yes_no( layfile, lay_attr, &lay->hx.hx_head[lvl].number_reset );
                break;
            case e_case:
                o_case( layfile, lay_attr, &lay->hx.hx_head[lvl].hd_case );
                break;
            case e_align:
                o_space_unit( layfile, lay_attr, &lay->hx.hx_head[lvl].align );
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
    }
}


/***************************************************************************/
/*   :HEADING   output header attribute values                             */
/***************************************************************************/
static  void    put_lay_heading( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":HEADING\n" );

    for( k = 0, lay_attr = heading_att[k]; lay_attr > 0; k++, lay_attr = heading_att[k] ) {
        switch( lay_attr ) {
        case e_delim:
            o_char( layfile, lay_attr, &lay->heading.delim );
            break;
        case e_stop_eject:
            o_yes_no( layfile, lay_attr, &lay->heading.stop_eject );
            break;
        case e_para_indent:
            o_yes_no( layfile, lay_attr, &lay->heading.para_indent );
            break;
        case e_threshold:
            o_threshold( layfile, lay_attr, &lay->heading.threshold );
            break;
        case e_max_group:
            o_int8( layfile, lay_attr, &lay->heading.max_group );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :Ix        output index  attribute values for :I1 - :I3               */
/***************************************************************************/
static  void    put_lay_ix( FILE * layfile, layout_data * lay )
{
    int                 k;
    int                 lvl;
    lay_att             lay_attr;

    for( lvl = 0; lvl < 3; ++lvl ) {

        fprintf( layfile, ":I%c\n", '1' + lvl );

        for( k = 0, lay_attr = ix_att[k]; lay_attr > 0; k++, lay_attr = ix_att[k] ) {
            switch( lay_attr ) {
            case e_pre_skip:
                o_space_unit( layfile, lay_attr, &lay->ix[lvl].pre_skip );
                break;
            case e_post_skip:
                o_space_unit( layfile, lay_attr, &lay->ix[lvl].post_skip );
                break;
            case e_skip:
                o_space_unit( layfile, lay_attr, &lay->ix[lvl].skip );
                break;
            case e_font:
                o_font_number( layfile, lay_attr, &lay->ix[lvl].font );
                break;
            case e_indent:
                o_space_unit( layfile, lay_attr, &lay->ix[lvl].indent );
                break;
            case e_wrap_indent:
                o_space_unit( layfile, lay_attr, &lay->ix[lvl].wrap_indent );
                break;
            case e_index_delim:
                o_xx_string( layfile, lay_attr, lay->ix[lvl].index_delim );
                break;
            case e_string_font:
                if( lvl < 2 ) {         // :I3 has no string font
                    o_font_number( layfile, lay_attr, &lay->ix[lvl].string_font );
                }
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
    }
}


/***************************************************************************/
/*   :INDEX     output index attribute values                              */
/***************************************************************************/
static  void    put_lay_index( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":INDEX\n" );

    for( k = 0, lay_attr = index_att[k]; lay_attr > 0; k++, lay_attr = index_att[k] ) {

        switch( lay_attr ) {
        case e_post_skip:
            o_space_unit( layfile, lay_attr, &lay->hx.hx_sect[hds_index].post_skip );
            break;
        case e_pre_top_skip:
            o_space_unit( layfile, lay_attr, &lay->hx.hx_sect[hds_index].pre_top_skip );
            break;
        case e_left_adjust:
            o_space_unit( layfile, lay_attr, &lay->index.left_adjust );
            break;
        case e_right_adjust:
            o_space_unit( layfile, lay_attr, &lay->index.right_adjust );
            break;
        case e_spacing:
            o_spacing( layfile, lay_attr, &lay->hx.hx_sect[hds_index].spacing );
            break;
        case e_columns:
            o_int8( layfile, lay_attr, &lay->index.columns );
            break;
        case e_see_string:
            o_xx_string( layfile, lay_attr, lay->index.see_string );
            break;
        case e_see_also_string:
            o_xx_string( layfile, lay_attr, lay->index.see_also_string );
            break;
        case e_header:
            o_yes_no( layfile, lay_attr, &lay->hx.hx_sect[hds_index].header );
            break;
        case e_index_string:
            o_xx_string( layfile, lay_attr, lay->index.index_string );
            break;
        case e_page_eject:
            o_page_eject( layfile, lay_attr, &lay->index.page_eject );
            break;
        case e_page_reset:
            o_yes_no( layfile, lay_attr, &lay->index.page_reset );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->hx.hx_sect[hds_index].text_font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :IXHEAD    output index header attribute values                       */
/***************************************************************************/
static  void    put_lay_ixhead( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":IXHEAD\n" );

    for( k = 0, lay_attr = ixhead_att[k]; lay_attr > 0; k++, lay_attr = ixhead_att[k] ) {

        switch( lay_attr ) {
        case e_pre_skip:
            o_space_unit( layfile, lay_attr, &lay->ixhead.pre_skip );
            break;
        case e_post_skip:
            o_space_unit( layfile, lay_attr, &lay->ixhead.post_skip );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->ixhead.font );
            break;
        case e_indent:
            o_space_unit( layfile, lay_attr, &lay->ixhead.indent );
            break;
        case e_ixhead_frame:
            lay_attr = e_frame;             // frame = instead of ixhead_frame =
            o_default_frame( layfile, lay_attr, &lay->ixhead.frame );
            break;
        case e_header:
            o_yes_no( layfile, lay_attr, &lay->ixhead.header );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :LP        output list part attribute values                          */
/***************************************************************************/
static  void    put_lay_lp( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":LP\n" );

    for( k = 0, lay_attr = lp_att[k]; lay_attr > 0; k++, lay_attr = lp_att[k] ) {

        switch( lay_attr ) {
        case e_left_indent:
            o_space_unit( layfile, lay_attr, &lay->lp.left_indent );
            break;
        case e_right_indent:
            o_space_unit( layfile, lay_attr, &lay->lp.right_indent );
            break;
        case e_line_indent:
            o_space_unit( layfile, lay_attr, &lay->lp.line_indent );
            break;
        case e_pre_skip:
            o_space_unit( layfile, lay_attr, &lay->lp.pre_skip );
            break;
        case e_post_skip:
            o_space_unit( layfile, lay_attr, &lay->lp.post_skip );
            break;
        case e_spacing:
            o_spacing( layfile, lay_attr, &lay->lp.spacing );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :LQ        output long quotation attribute values                     */
/***************************************************************************/
static  void    put_lay_lq( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":LQ\n" );

    for( k = 0, lay_attr = lq_att[k]; lay_attr > 0; k++, lay_attr = lq_att[k] ) {

        switch( lay_attr ) {
        case e_left_indent:
            o_space_unit( layfile, lay_attr, &lay->lq.left_indent );
            break;
        case e_right_indent:
            o_space_unit( layfile, lay_attr, &lay->lq.right_indent );
            break;
        case e_pre_skip:
            o_space_unit( layfile, lay_attr, &lay->lq.pre_skip );
            break;
        case e_post_skip:
            o_space_unit( layfile, lay_attr, &lay->lq.post_skip );
            break;
        case e_spacing:
            o_spacing( layfile, lay_attr, &lay->lq.spacing );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->lq.font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :NOTE      output note attribute values                               */
/***************************************************************************/
static  void    put_lay_note( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":NOTE\n" );

    for( k = 0, lay_attr = note_att[k]; lay_attr > 0; k++, lay_attr = note_att[k] ) {

        switch( lay_attr ) {
        case e_left_indent:
            o_space_unit( layfile, lay_attr, &lay->note.left_indent );
            break;
        case e_right_indent:
            o_space_unit( layfile, lay_attr, &lay->note.right_indent );
            break;
        case e_pre_skip:
            o_space_unit( layfile, lay_attr, &lay->note.pre_skip );
            break;
        case e_post_skip:
            o_space_unit( layfile, lay_attr, &lay->note.post_skip );
            break;
        case e_spacing:
            o_spacing( layfile, lay_attr, &lay->note.spacing );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->note.font );
            break;
        case e_note_string:
            o_xx_string( layfile, lay_attr, lay->note.string );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :OL        output attribute values                                    */
/***************************************************************************/
static  void    put_lay_ol( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;
    ol_lay_level    *   ol_layout;

    ol_layout = lay->ol.first;

    while( ol_layout != NULL ) {
        fprintf( layfile, ":OL\n" );

        for( k = 0, lay_attr = ol_att[k]; lay_attr > 0; k++, lay_attr = ol_att[k] ) {

            switch( lay_attr ) {
            case e_level:
                o_int8( layfile, lay_attr, &ol_layout->level );
                break;
            case e_left_indent:
                o_space_unit( layfile, lay_attr, &ol_layout->left_indent );
                break;
            case e_right_indent:
                o_space_unit( layfile, lay_attr, &ol_layout->right_indent );
                break;
            case e_pre_skip:
                o_space_unit( layfile, lay_attr, &ol_layout->pre_skip );
                break;
            case e_skip:
                o_space_unit( layfile, lay_attr, &ol_layout->skip );
                break;
            case e_spacing:
                o_spacing( layfile, lay_attr, &ol_layout->spacing );
                break;
            case e_post_skip:
                o_space_unit( layfile, lay_attr, &ol_layout->post_skip );
                break;
            case e_font:
                o_font_number(layfile, lay_attr, &ol_layout->font );
                break;
            case e_align:
                o_space_unit( layfile, lay_attr, &ol_layout->align );
                break;
            case e_number_style:
                o_number_style( layfile, lay_attr, &ol_layout->number_style );
                break;
            case e_number_font:
                o_font_number( layfile, lay_attr, &ol_layout->number_font );
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
        ol_layout = ol_layout->next;
    }
}


/***************************************************************************/
/*   :PAGE   output  page attribute values                                 */
/***************************************************************************/
static  void    put_lay_page( FILE * layfile, layout_data * lay )
{
    int             k;
    lay_att         lay_attr;
    su          *   units;

    fprintf( layfile, ":PAGE\n" );

    for( k = 0, lay_attr = page_att[k]; lay_attr > 0; k++, lay_attr = page_att[k] ) {

        switch( lay_attr ) {
        case e_top_margin:
            units = &(lay->page.top_margin);
            break;
        case e_left_margin:
            units = &(lay->page.left_margin);
            break;
        case e_right_margin:
            units = &(lay->page.right_margin);
            break;
        case e_depth:
            units = &(lay->page.depth);
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
        o_space_unit( layfile, lay_attr, units );
    }
}


/***************************************************************************/
/*   :P         output paragraph attribute values                          */
/*   :PC        output paragraph continue attribute values                 */
/***************************************************************************/
static  void    put_lay_p_pc( FILE * layfile, p_lay_tag * ap, char * name )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":%s\n", name );

    for( k = 0, lay_attr = p_att[k]; lay_attr > 0; k++, lay_attr = p_att[k] ) {

        switch( lay_attr ) {
        case e_line_indent:
            o_space_unit( layfile, lay_attr, &ap->line_indent );
            break;
        case e_pre_skip:
            o_space_unit( layfile, lay_attr, &ap->pre_skip );
            break;
        case e_post_skip:
            o_space_unit( layfile, lay_attr, &ap->post_skip );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}

static  void    put_lay_p( FILE * layfile, layout_data * lay )
{
    put_lay_p_pc( layfile, &(lay->p), "P" );
}

static  void    put_lay_pc( FILE * layfile, layout_data * lay )
{
    put_lay_p_pc( layfile, &(lay->pc), "PC" );
}

/***************************************************************************/
/*   :SL        output attribute values                                    */
/***************************************************************************/
static  void    put_lay_sl( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;
    sl_lay_level    *   sl_layout;

    sl_layout = lay->sl.first;

    while( sl_layout != NULL ) {
        fprintf( layfile, ":SL\n" );

        for( k = 0, lay_attr = sl_att[k]; lay_attr > 0; k++, lay_attr = sl_att[k] ) {

            switch( lay_attr ) {
            case e_level:
                o_int8( layfile, lay_attr, &sl_layout->level );
                break;
            case e_left_indent:
                o_space_unit( layfile, lay_attr, &sl_layout->left_indent );
                break;
            case e_right_indent:
                o_space_unit( layfile, lay_attr, &sl_layout->right_indent );
                break;
            case e_pre_skip:
                o_space_unit( layfile, lay_attr, &sl_layout->pre_skip );
                break;
            case e_skip:
                o_space_unit( layfile, lay_attr, &sl_layout->skip );
                break;
            case e_spacing:
                o_spacing( layfile, lay_attr, &sl_layout->spacing );
                break;
            case e_post_skip:
                o_space_unit( layfile, lay_attr, &sl_layout->post_skip );
                break;
            case e_font:
                o_font_number(layfile, lay_attr, &sl_layout->font );
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
        sl_layout = sl_layout->next;
    }
}


/***************************************************************************/
/*   :TITLE     output title attribute values                              */
/***************************************************************************/
static  void    put_lay_title( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":TITLE\n" );

    for( k = 0, lay_attr = title_att[k]; lay_attr > 0; k++, lay_attr = title_att[k] ) {

        switch( lay_attr ) {
        case e_left_adjust:
            o_space_unit( layfile, lay_attr, &lay->title.left_adjust );
            break;
        case e_right_adjust:
            o_space_unit( layfile, lay_attr, &lay->title.right_adjust );
            break;
        case e_page_position:
            o_page_position( layfile, lay_attr, &lay->title.page_position );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->title.font );
            break;
        case e_pre_top_skip:
            o_space_unit( layfile, lay_attr, &lay->title.pre_top_skip );
            break;
        case e_skip:
            o_space_unit( layfile, lay_attr, &lay->title.skip );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :TITLEP    output title page attribute values                         */
/***************************************************************************/
static  void    put_lay_titlep( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":TITLEP\n" );

    for( k = 0, lay_attr = titlep_att[k]; lay_attr > 0; k++, lay_attr = titlep_att[k] ) {

        switch( lay_attr ) {
        case e_spacing:
            o_spacing( layfile, lay_attr, &lay->titlep.spacing );
            break;
        case e_columns:
            o_int8( layfile, lay_attr, &lay->titlep.columns );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :TOC       output table of contents attribute values                  */
/***************************************************************************/
static  void    put_lay_toc( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":TOC\n" );

    for( k = 0, lay_attr = toc_att[k]; lay_attr > 0; k++, lay_attr = toc_att[k] ) {

        switch( lay_attr ) {
        case e_left_adjust:
            o_space_unit( layfile, lay_attr, &lay->toc.left_adjust );
            break;
        case e_right_adjust:
            o_space_unit( layfile, lay_attr, &lay->toc.right_adjust );
            break;
        case e_spacing:
            o_spacing( layfile, lay_attr, &lay->toc.spacing );
            break;
        case e_columns:
            o_int8( layfile, lay_attr, &lay->toc.columns );
            break;
        case e_toc_levels:
            o_int8( layfile, lay_attr, &lay->toc.toc_levels );
            break;
        case e_fill_string:
            o_xx_string( layfile, lay_attr, lay->toc.fill_string );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :TOCPGNUM  output table of contents number attribute values           */
/***************************************************************************/
static  void    put_lay_tocpgnum( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":TOCPGNUM\n" );

    for( k = 0, lay_attr = tocpgnum_att[k]; lay_attr > 0; k++, lay_attr = tocpgnum_att[k] ) {

        switch( lay_attr ) {
        case e_size:
            o_space_unit( layfile, lay_attr, &lay->tocpgnum.size );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->tocpgnum.font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :TOCHx     output TOC header attribute values for :TOCH0 - :TOCH6     */
/***************************************************************************/
static  void    put_lay_tochx( FILE * layfile, layout_data * lay )
{
    int                 k;
    int                 lvl;
    lay_att             lay_attr;

    for( lvl = 0; lvl < 7; ++lvl ) {


        fprintf( layfile, ":TOCH%c\n", '0' + lvl );

        for( k = 0, lay_attr = tochx_att[k]; lay_attr > 0; k++, lay_attr = tochx_att[k] ) {

            switch( lay_attr ) {
            case e_group:
                o_int8( layfile, lay_attr, &lay->tochx[lvl].group );
                break;
            case e_indent:
                o_space_unit( layfile, lay_attr, &lay->tochx[lvl].indent );
                break;
            case e_skip:
                o_space_unit( layfile, lay_attr, &lay->tochx[lvl].skip );
                break;
            case e_pre_skip:
                o_space_unit( layfile, lay_attr, &lay->tochx[lvl].pre_skip );
                break;
            case e_post_skip:
                o_space_unit( layfile, lay_attr, &lay->tochx[lvl].post_skip );
                break;
            case e_font:
                o_font_number( layfile, lay_attr, &lay->tochx[lvl].font );
                break;
            case e_align:
                o_space_unit( layfile, lay_attr, &lay->tochx[lvl].align );
                break;
            case e_display_in_toc:
                o_yes_no( layfile, lay_attr, &lay->tochx[lvl].display_in_toc );
                break;
                break;
            default:
                internal_err( __FILE__, __LINE__ );internal_err( __FILE__, __LINE__ );
                break;
            }
        }
    }
}


/***************************************************************************/
/*   :UL        output attribute values                                    */
/***************************************************************************/
static  void    put_lay_ul( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;
    ul_lay_level    *   ul_layout;

    ul_layout = lay->ul.first;

    while( ul_layout != NULL ) {
        fprintf( layfile, ":UL\n" );

        for( k = 0, lay_attr = ul_att[k]; lay_attr > 0; k++, lay_attr = ul_att[k] ) {

            switch( lay_attr ) {
            case e_level:
                o_int8( layfile, lay_attr, &ul_layout->level );
                break;
            case e_left_indent:
                o_space_unit( layfile, lay_attr, &ul_layout->left_indent );
                break;
            case e_right_indent:
                o_space_unit( layfile, lay_attr, &ul_layout->right_indent );
                break;
            case e_pre_skip:
                o_space_unit( layfile, lay_attr, &ul_layout->pre_skip );
                break;
            case e_skip:
                o_space_unit( layfile, lay_attr, &ul_layout->skip );
                break;
            case e_spacing:
                o_spacing( layfile, lay_attr, &ul_layout->spacing );
                break;
            case e_post_skip:
                o_space_unit( layfile, lay_attr, &ul_layout->post_skip );
                break;
            case e_font:
                o_font_number(layfile, lay_attr, &ul_layout->font );
                break;
            case e_align:
                o_space_unit( layfile, lay_attr, &ul_layout->align );
                break;
            case e_bullet:
                o_char( layfile, lay_attr, &ul_layout->bullet );
                break;
            case e_bullet_translate:
                o_yes_no( layfile, lay_attr, &ul_layout->bullet_translate );
                break;
            case e_bullet_font:
                o_font_number( layfile, lay_attr, &ul_layout->bullet_font );
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
        ul_layout = ul_layout->next;
    }
}


/***************************************************************************/
/*   :WIDOW    output widow attribute value                                */
/***************************************************************************/
static  void    put_lay_widow( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":WIDOW\n" );

    for( k = 0, lay_attr = widow_att[k]; lay_attr > 0; k++, lay_attr = widow_att[k] ) {

        switch( lay_attr ) {
        case e_threshold:
            o_threshold( layfile, lay_attr, &lay->widow.threshold );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :XMP       output example attribute values                            */
/***************************************************************************/
static  void    put_lay_xmp( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":XMP\n" );

    for( k = 0, lay_attr = xmp_att[k]; lay_attr > 0; k++, lay_attr = xmp_att[k] ) {

        switch( lay_attr ) {
        case e_left_indent:
            o_space_unit( layfile, lay_attr, &lay->xmp.left_indent );
            break;
        case e_right_indent:
            o_space_unit( layfile, lay_attr, &lay->xmp.right_indent );
            break;
        case e_pre_skip:
            o_space_unit( layfile, lay_attr, &lay->xmp.pre_skip );
            break;
        case e_post_skip:
            o_space_unit( layfile, lay_attr, &lay->xmp.post_skip );
            break;
        case e_spacing:
            o_spacing( layfile, lay_attr, &lay->xmp.spacing );
            break;
        case e_font:
            o_font_number( layfile, lay_attr, &lay->xmp.font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :xx        output for font only value                                 */
/***************************************************************************/
static  void    put_lay_xx( FILE * layfile, font_number *font, char * name )
{
    int                 k;
    lay_att             lay_attr;

    fprintf( layfile, ":%s\n", name );

    for( k = 0, lay_attr = xx_att[k]; lay_attr > 0; k++, lay_attr = xx_att[k] ) {

        switch( lay_attr ) {
        case e_font:
            o_font_number( layfile, lay_attr, font );
            break;
        default:
            out_msg( "WGML logic error glconvrt.c.\n");
            err_count++;
            break;
        }
    }
}

static  void    put_lay_dt( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->dt.font), "DT" );
}

static  void    put_lay_gt( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->gt.font), "GT" );
}

static  void    put_lay_dthd( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->dthd.font), "DTHD" );
}

static  void    put_lay_cit( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->cit.font), "CIT" );
}

static  void    put_lay_gd( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->gd.font), "GD" );
}

static  void    put_lay_ddhd( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->ddhd.font), "DDHD" );
}

static  void    put_lay_ixpgnum( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->ixpgnum.font), "IXPGNUM" );
}

static  void    put_lay_ixmajor( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->ixmajor.font), "IXMAJOR" );
}


/***************************************************************************/
/*   :XXXXX     output for unsupported LETTER tags                         */
/***************************************************************************/
static  void    put_lay_letter_unsupported( FILE * layfile )
{

    fprintf( layfile, ":FROM\n");
    fprintf( layfile, "        left_adjust = 0\n");
    fprintf( layfile, "        page_position = right\n");
    fprintf( layfile, "        pre_top_skip = 6\n");
    fprintf( layfile, "        font = 0\n");

    fprintf( layfile, ":TO\n");
    fprintf( layfile, "        left_adjust = 0\n");
    fprintf( layfile, "        page_position = left\n");
    fprintf( layfile, "        pre_top_skip = 1\n");
    fprintf( layfile, "        font = 0\n");

    fprintf( layfile, ":ATTN\n");
    fprintf( layfile, "        left_adjust = 0\n");
    fprintf( layfile, "        page_position = left\n");
    fprintf( layfile, "        pre_top_skip = 1\n");
    fprintf( layfile, "        font = 1\n");
    fprintf( layfile, "        attn_string = \"Attention: \"\n");
    fprintf( layfile, "        string_font = 1\n");

    fprintf( layfile, ":SUBJECT\n");
    fprintf( layfile, "        left_adjust = 0\n");
    fprintf( layfile, "        page_position = centre\n");
    fprintf( layfile, "        pre_top_skip = 2\n");
    fprintf( layfile, "        font = 1\n");

    fprintf( layfile, ":LETDATE\n");
    fprintf( layfile, "        date_form = \"$ml $dsn, $yl\"\n");
    fprintf( layfile, "        depth = 15\n");
    fprintf( layfile, "        font = 0\n");
    fprintf( layfile, "        page_position = right\n");

    fprintf( layfile, ":OPEN\n");
    fprintf( layfile, "        pre_top_skip = 2\n");
    fprintf( layfile, "        font = 0\n");
    fprintf( layfile, "        delim = ':'\n");

    fprintf( layfile, ":CLOSE\n");
    fprintf( layfile, "        pre_skip = 2\n");
    fprintf( layfile, "        depth = 6\n");
    fprintf( layfile, "        font = 0\n");
    fprintf( layfile, "        page_position = centre\n");
    fprintf( layfile, "        delim = ','\n");
    fprintf( layfile, "        extract_threshold = 2\n");

    fprintf( layfile, ":ECLOSE\n");
    fprintf( layfile, "        pre_skip = 1\n");
    fprintf( layfile, "        font = 0\n");

    fprintf( layfile, ":DISTRIB\n");
    fprintf( layfile, "        pre_top_skip = 3\n");
    fprintf( layfile, "        skip = 1\n");
    fprintf( layfile, "        font = 0\n");
    fprintf( layfile, "        indent = '0.5i'\n");
    fprintf( layfile, "        page_eject = no\n");
}


/***************************************************************************/
/*   output layout data to file                                            */
/***************************************************************************/
static  void    put_layout( char * name, layout_data * lay )
{
    static  FILE    *   layfile;
    symsub          *   sversion;

    layfile = fopen( name, "wt" );
    if( layfile == NULL ) {
        out_msg( "open error %s\n", name );
        return;
    }

    fprintf( layfile, ":LAYOUT\n" );
//  if( WgmlFlags.research ) {
        find_symvar( global_dict, "$version", no_subscript, &sversion );
        fprintf( layfile, ":cmt. Created with %s\n", sversion->value );
//  }

    put_lay_page( layfile, lay );
    put_lay_default( layfile, lay );
    put_lay_widow( layfile, lay );
    put_lay_fn( layfile, lay );
    put_lay_fnref( layfile, lay );
    put_lay_p( layfile, lay );
    put_lay_pc( layfile, lay );
    put_lay_fig( layfile, lay );
    put_lay_xmp( layfile, lay );
    put_lay_note( layfile, lay );
    put_lay_hx( layfile, lay );
    put_lay_heading( layfile, lay );
    put_lay_lq( layfile, lay );
    put_lay_dt( layfile, lay );
    put_lay_gt( layfile, lay );
    put_lay_dthd( layfile, lay );
    put_lay_cit( layfile, lay );
    put_lay_figcap( layfile, lay );
    put_lay_figdesc( layfile, lay );
    put_lay_dd( layfile, lay );
    put_lay_gd( layfile, lay );
    put_lay_ddhd( layfile, lay );
    put_lay_abstract( layfile, lay );
    put_lay_preface( layfile, lay );
    put_lay_body( layfile, lay );
    put_lay_backm( layfile, lay );
    put_lay_lp( layfile, lay );
    put_lay_index( layfile, lay );
    put_lay_ixpgnum( layfile, lay );
    put_lay_ixmajor( layfile, lay );
    put_lay_ixhead( layfile, lay );
    put_lay_ix( layfile, lay );
    put_lay_toc( layfile, lay );
    put_lay_tocpgnum( layfile, lay );
    put_lay_tochx( layfile, lay );
    put_lay_figlist( layfile, lay );
    put_lay_flpgnum( layfile, lay );
    put_lay_titlep( layfile, lay );
    put_lay_title( layfile, lay );
    put_lay_docnum( layfile, lay );
    put_lay_date( layfile, lay );
    put_lay_author( layfile, lay );
    put_lay_address( layfile, lay );
    put_lay_aline( layfile, lay );

    put_lay_letter_unsupported( layfile );  // dummy output

    put_lay_appendix( layfile, lay );
    put_lay_sl( layfile, lay );
    put_lay_ol( layfile, lay );
    put_lay_ul( layfile, lay );
    put_lay_dl( layfile, lay );
    put_lay_gl( layfile, lay );
    put_lay_banner( layfile, lay );

    fprintf( layfile, ":eLAYOUT\n" );

    fclose( layfile );
}


/***************************************************************************/
/*  lay_convert   process :CONVERT tag                                     */
/***************************************************************************/

void    lay_convert( const gmltag * entry )
{
    char        *   p;

    (void)entry;

    p = scan_start;
    scan_start = scan_stop;

    if( !WgmlFlags.firstpass ) {
        ProcFlags.layout = true;
        return;                         // process during first pass only
    }
    SkipSpaces( p );
    *token_buf = '\0';
    if( strnicmp( "file=", p, 5 ) == 0 ) {  // file attribute?
        char    quote;
        char    *fnstart;

        p += 5;
        if( *p == '"' || *p == '\'' ) {
            quote = *p;
            ++p;
        } else {
            quote = '.';                // error?? filename without quotes
        }
        fnstart = p;
        while( *p != '\0' && *p != quote ) {
            ++p;
        }
        *p = '\0';
        strcpy_s( token_buf, buf_size, fnstart );
    } else {                            // try undocumented format
        if( *p != '\0' && *p == '.' ) {
            strcpy_s( token_buf, buf_size, p + 1 );
        }
    }
    if( *token_buf == '\0' ) {           // file name missing
        xx_err( err_att_missing );
    } else {
        put_layout( token_buf, &layout_work );
    }
    return;
}
