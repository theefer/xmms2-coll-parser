/*  XMMS2 Collection parser
 *  Copyright (C) 2009 Raphaël Bois
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 */

%pure-parser

%locations
%defines
%error-verbose

%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"
%}

%parse-param {xm_context_t *context}
%lex-param {void *scanner}

%union {
	int ival;
	char *sval;
	xmmsv_coll_t  *coll;
	xm_string_t   *xstr;
	xm_sequence_t *seq;
}

%token <ival> TOKEN_INTEGER
%token <sval> TOKEN_PROP_SHORT TOKEN_PROP_LONG TOKEN_STRING TOKEN_PATTERN
%token TOKEN_GROUP_OPEN TOKEN_GROUP_CLOSE
%token TOKEN_SYMBOL_ID TOKEN_OPFIL_HAS TOKEN_REFERENCE
%token TOKEN_OPFIL_EQ TOKEN_OPFIL_MATCH TOKEN_OPFIL_LE TOKEN_OPFIL_GE
%token TOKEN_OPFIL_LT TOKEN_OPFIL_GT
%token TOKEN_OPSET_OR TOKEN_OPSET_AND TOKEN_OPSET_NOT
%token TOKEN_IDSEQ_RANGE TOKEN_IDSEQ_SEP

%token TOKEN_ERR

%type <sval> property
%type <coll> operation and_op or_op expr filter unary_filter binary_filter
%type <xstr> string
%type <seq> sequence seqrange sequence_rev

%start s

%{
#define scanner (context->scanner)
%}

%%

s : operation {
	xm_context_store_result(context, $1);
}
    ;

operation : or_op {
	$$ = $1;
}
            ;

or_op : or_op TOKEN_OPSET_OR and_op {
  $$ = xm_build_union(context, $1, $3);
}
      | and_op {
	$$ = $1;
}
        ;

and_op : and_op expr {
  $$ = xm_build_intersection(context, $1, $2);
}
       | and_op TOKEN_OPSET_AND expr {
  $$ = xm_build_intersection(context, $1, $3);
}
       | expr {
	$$ = $1;
}
       ;

expr : TOKEN_SYMBOL_ID sequence {
  $$ = xm_build_idlist(context, $2);
  xm_sequence_free(context, $2);
}
     | filter {
	$$ = $1;
}
     | TOKEN_GROUP_OPEN operation TOKEN_GROUP_CLOSE {
  $$ = $2;
}
     | TOKEN_OPSET_NOT expr {
  $$ = xm_build_complement(context, $2);
}
     | TOKEN_REFERENCE TOKEN_STRING {
  $$ = xm_build_reference(context, $2);
  free($2);
}
       ;

filter : unary_filter {
	$$ = $1;
}
       | binary_filter {
	$$ = $1;
}
       | string {
  /* TODO: use appropriate enum value for filter. */
  /* Behave like  'TOKEN_OPFIL_EQ property' rule in binary_filter. */
  $$ = xm_build_binary_with_string(context, 0, NULL, $1);
  xm_string_free(context, $1);
}

unary_filter : TOKEN_OPFIL_HAS property {
  /* TODO: get appropriate enum value for 'HAS' filter. */
  $$ = xm_build_unary(context, 0, $2);
  free($2); /* Required! */
}
               ;

binary_filter : property TOKEN_OPFIL_EQ string {
  /* TODO: use appropriate enum value for filter. */
  $$ = xm_build_binary_with_string(context, 0, $1, $3);
  free($1);
  xm_string_free(context, $3);
}
              | property TOKEN_OPFIL_MATCH string {
  /* TODO: use appropriate enum value for filter. */
  $$ = xm_build_binary_with_string(context, 0, $1, $3);
  free($1);
  xm_string_free(context, $3);
}
              | property TOKEN_OPFIL_LE TOKEN_INTEGER {
  /* TODO: use appropriate enum value for filter. */
  $$ = xm_build_binary_with_integer(context, 0, $1, $3);
  free($1);
}
              | property TOKEN_OPFIL_GE TOKEN_INTEGER {
  /* TODO: use appropriate enum value for filter. */
  $$ = xm_build_binary_with_integer(context, 0, $1, $3);
  free($1);
}
              | property TOKEN_OPFIL_LT TOKEN_INTEGER {
  /* TODO: use appropriate enum value for filter. */
  $$ = xm_build_binary_with_integer(context, 0, $1, $3);
  free($1);
}
              | property TOKEN_OPFIL_GT TOKEN_INTEGER {
  /* TODO: use appropriate enum value for filter. */
  $$ = xm_build_binary_with_integer(context, 0, $1, $3);
  free($1);
}
              | TOKEN_OPFIL_EQ string {
  /* TODO: use appropriate enum value for filter. */
  $$ = xm_build_binary_with_string(context, 0, NULL, $2);
  xm_string_free(context, $2);
}
              | TOKEN_OPFIL_MATCH string {
  /* TODO: use appropriate enum value for filter. */
  $$ = xm_build_binary_with_string(context, 0, NULL, $2);
  xm_string_free(context, $2);
}
                ;

property : TOKEN_PROP_SHORT {
  $$ = xm_property_get_from_short(context, $1);
  free($1);
}
         | TOKEN_PROP_LONG {
  $$ = $1;
}
           ;

string : TOKEN_STRING {
  $$ = xm_string_new(context, XM_STRING_TYPE_STRING, $1);
}
       | TOKEN_PATTERN {
  $$ = xm_string_new(context, XM_STRING_TYPE_PATTERN, $1);
}
       | TOKEN_INTEGER {
  $$ = xm_string_new_from_integer(context, $1);
}
         ;

sequence : sequence_rev {
  $$ = xm_sequence_reverse(context, $1);
}
           ;
         
sequence_rev : sequence_rev TOKEN_IDSEQ_SEP seqrange {
  /* It is faster to build the sequence in reverse order. */
  $$ = xm_sequence_prepend_range(context, $1, $3);
}
               | seqrange {
	$$ = $1;
}
                 ;

seqrange : TOKEN_INTEGER TOKEN_IDSEQ_RANGE TOKEN_INTEGER {
  $$ = xm_range_new(context, $1, $3);
}        | TOKEN_INTEGER {
  $$ = xm_range_new(context, $1, $1);
}

%%
