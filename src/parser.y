/*  XMMS2 Collection parser
 *  Copyright (C) 2010 Raphaël Bois
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

%{
#include <stdio.h> /* snprintf */
#include <string.h> /* strdup */

#include "coll-builder.h"
%}

%define api.pure
%name-prefix="xm_yy"

%locations
%defines
%error-verbose

%parse-param {xm_context_t *context}
%parse-param {void *scanner}
%lex-param {void *scanner}

%union {
	unsigned int ival;
	xm_boxed_t *coll;
	xm_string_t *xstr;
	xm_sequence_t *seq;
}

%token <ival> TOKEN_INTEGER
%token <xstr> TOKEN_PROP_SHORT TOKEN_PROP_LONG
%token <xstr> TOKEN_STRING TOKEN_PATTERN TOKEN_NUMERIC
%token TOKEN_GROUP_OPEN TOKEN_GROUP_CLOSE
%token TOKEN_SYMBOL_ID TOKEN_OPFIL_HAS TOKEN_REFERENCE
%token TOKEN_OPFIL_EQ TOKEN_OPFIL_MATCH TOKEN_OPFIL_LE TOKEN_OPFIL_GE
%token TOKEN_OPFIL_LT TOKEN_OPFIL_GT
%token TOKEN_OPSET_OR TOKEN_OPSET_AND TOKEN_OPSET_NOT
%token TOKEN_IDSEQ_RANGE TOKEN_IDSEQ_SEP

%token TOKEN_ERR

%type <xstr> property string integer
%type <coll> operation and_op or_op expr filter unary_filter binary_filter
%type <seq> sequence seqrange sequence_rev

%start s

%{
void
xm_yyerror(YYLTYPE *locp, xm_context_t *ctx, void *scanner, const char *msg);

int
xm_yylex(YYSTYPE *lval, YYLTYPE *lloc, void*scanner);

#define XM_BOX(coll) \
	(xm_context_boxed_new(context, (coll), (xm_destroy_func) xmmsv_coll_unref))

#define COLL(boxed) ((xmmsv_coll_t *)((boxed)->data))
#define COLLREF(boxed) (xmmsv_coll_ref(COLL(boxed)))
%}

%%

s :
	operation {
		xm_context_store_result(context, COLL($1));
		xm_boxed_unref($1);
	}
;

operation :
	or_op {
		$$ = $1;
	}
;

or_op :
	or_op TOKEN_OPSET_OR and_op {
		$$ = XM_BOX(xm_build_union(context, COLLREF($1), COLLREF($3)));
		xm_boxed_unref($1);
		xm_boxed_unref($3);
	}
|	and_op {
		$$ = $1;
	}
;

and_op :
	and_op expr {
		$$ = XM_BOX(xm_build_union(context, COLLREF($1), COLLREF($2)));
		xm_boxed_unref($1);
		xm_boxed_unref($2);
	}
|	and_op TOKEN_OPSET_AND expr {
		$$ = XM_BOX(xm_build_intersection(context, COLLREF($1), COLLREF($3)));
		xm_boxed_unref($1);
		xm_boxed_unref($3);
	}
|	expr {
		$$ = $1;
	}
;

expr :
	sequence {
		$$ = XM_BOX(xm_build_idlist(context, $1));
		xm_sequence_destroy($1);
	}
|	filter {
		$$ = $1;
	}
|	TOKEN_GROUP_OPEN operation TOKEN_GROUP_CLOSE {
		$$ = $2;
	}
|	TOKEN_OPSET_NOT expr {
		$$ = XM_BOX(xm_build_complement(context, COLLREF($2)));
		xm_boxed_unref($2);
	}
|	TOKEN_REFERENCE string {
		$$ = XM_BOX(xm_build_reference(context, $2->value));
		xm_string_unref($2);
	}
;

filter :
	unary_filter {
		$$ = $1;
	}
|	binary_filter {
		$$ = $1;
	}
|	string {
		$$ = XM_BOX(xm_build_binary(context, XMMS_COLLECTION_TYPE_EQUALS, NULL, $1));
		xm_string_unref($1);
	}
;

unary_filter :
	TOKEN_OPFIL_HAS property {
		$$ = XM_BOX(xm_build_unary(context, XMMS_COLLECTION_TYPE_HAS, $2->value));
		xm_string_unref($2);
	}
;

binary_filter :
	property TOKEN_OPFIL_EQ string {
		$$ = XM_BOX(xm_build_binary(context, XMMS_COLLECTION_TYPE_EQUALS, $1->value, $3));
		xm_string_unref($1);
		xm_string_unref($3);
	}
|	property TOKEN_OPFIL_MATCH string {
		$$ = XM_BOX(xm_build_binary(context, XMMS_COLLECTION_TYPE_MATCH, $1->value, $3));
		xm_string_unref($1);
		xm_string_unref($3);
	}
|	property TOKEN_OPFIL_LE integer {
		$$ = XM_BOX(xm_build_binary_e(context, XMMS_COLLECTION_TYPE_SMALLER, $1->value, $3));
		xm_string_unref($1);
		xm_string_unref($3);
	}
|	property TOKEN_OPFIL_GE integer {
		$$ = XM_BOX(xm_build_binary_e(context, XMMS_COLLECTION_TYPE_GREATER, $1->value, $3));
		xm_string_unref($1);
		xm_string_unref($3);
	}
|	property TOKEN_OPFIL_LT integer {
		$$ = XM_BOX(xm_build_binary(context, XMMS_COLLECTION_TYPE_SMALLER, $1->value, $3));
		xm_string_unref($1);
		xm_string_unref($3);
	}
|	property TOKEN_OPFIL_GT integer {
		$$ = XM_BOX(xm_build_binary(context, XMMS_COLLECTION_TYPE_GREATER, $1->value, $3));
		xm_string_unref($1);
		xm_string_unref($3);
	}
|	TOKEN_OPFIL_EQ string {
		$$ = XM_BOX(xm_build_binary(context, XMMS_COLLECTION_TYPE_EQUALS, NULL, $2));
		xm_string_unref($2);
	}
|	TOKEN_OPFIL_MATCH string {
		/* TODO: use appropriate enum value for filter. */
		$$ = XM_BOX(xm_build_binary(context, XMMS_COLLECTION_TYPE_MATCH, NULL, $2));
		xm_string_unref($2);
	}
;

property :
	TOKEN_PROP_SHORT {
		$$ = xm_property_get_from_short(context, $1);
		xm_string_unref($1);
	}
|	TOKEN_PROP_LONG {
		$$ = $1;
	}
;

integer :
	TOKEN_STRING {
		xm_yyerror(&xm_yylloc, context, scanner, XM_ERROR_SEMANTIC_NOT_INTEGER);
		xm_string_unref($1);
		YYERROR;
	}
|	TOKEN_PATTERN {
		xm_yyerror(&xm_yylloc, context, scanner, XM_ERROR_SEMANTIC_NOT_INTEGER);
		xm_string_unref($1);
		YYERROR;
	}
|	TOKEN_NUMERIC {
		$$ = $1;
	}

string :
	TOKEN_STRING {
		$$ = $1;
	}
|	TOKEN_PATTERN {
		$$ = $1;
	}
|	TOKEN_NUMERIC {
		$$ = $1;
	}
;

sequence :
	TOKEN_SYMBOL_ID sequence_rev {
		$$ = xm_sequence_reverse($2);
	}
;

sequence_rev :
	sequence_rev TOKEN_IDSEQ_SEP seqrange {
		/* It is faster to build the sequence in reverse order. */
		$$ = xm_sequence_prepend($1, $3);
	}
|	seqrange {
		$$ = $1;
	}
;

seqrange :
	TOKEN_INTEGER TOKEN_IDSEQ_RANGE TOKEN_INTEGER {
		$$ = xm_context_sequence_new_range(context, $1, $3);
	}
|	TOKEN_INTEGER {
		$$ = xm_context_sequence_new_range(context, $1, $1);
	}
;

%%

/* directly parser-related functions. */

void
xm_yyerror(YYLTYPE *locp, xm_context_t *ctx, void *scanner, const char *msg)
{
	int l;
	char test;

	if (ctx->error) {
		return;
	}

	l = snprintf(&test, 1, XM_ERROR_DEFAULT_FORMAT,
	             locp->last_line, locp->last_column, msg);
	if (l > 0) {
		ctx->error = malloc(l + 1);
		snprintf(ctx->error, l + 1, XM_ERROR_DEFAULT_FORMAT,
		         locp->last_line, locp->last_column, msg);
	} else { /* We need to report an error no matter what. */
		ctx->error = strdup("Unexpected error");
	}
}

int
xm_context_parse(xm_context_t *ctx, const char *pat)
{
	xm_context_prepare(ctx);
	xm_context_init_scanner(ctx, pat);
	xm_yyparse(ctx, ctx->scanner);
	return !(ctx->error);
}

