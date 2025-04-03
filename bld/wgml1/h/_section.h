/*    enu                text        ban           */
pick( doc_sect_none,     "NONE",     no_ban        ) // nothing so far
pick( doc_sect_gdoc,     "GDOC",     no_ban        ) // gdoc
pick( doc_sect_frontm,   "FRONTM",   no_ban        ) // front matter
pick( doc_sect_titlep,   "TITLEP",   no_ban        ) // title page
pick( doc_sect_etitlep,  "eTITLEP",  no_ban        ) // end title page
pick( doc_sect_abstract, "ABSTRACT", abstract_ban  ) // abstract
pick( doc_sect_preface,  "PREFACE",  preface_ban   ) // preface
pick( doc_sect_toc,      "TOC",      toc_ban       ) // table of contents
pick( doc_sect_figlist,  "FIGLIST",  figlist_ban   ) // figure list
pick( doc_sect_body,     "BODY",     body_ban      ) // body
pick( doc_sect_appendix, "APPENDIX", appendix_ban  ) // appendix
pick( doc_sect_backm,    "BACKM",    backm_ban     ) // back matter
pick( doc_sect_index,    "INDEX",    index_ban     ) // index
pick( doc_sect_egdoc,    "eGDOC",    no_ban        ) // egdoc  has to be last
/*
pick( doc_sect_toce,     "NONE",     no_ban        ) // table of contents (at end of file)
pick( doc_sect_figliste, "NONE",     no_ban        ) // figure list (at end of file)
*/

