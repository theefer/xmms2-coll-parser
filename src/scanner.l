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

%option reentrant
%option bison-bridge
%option bison-locations
%option noyywrap
%option nounput
%option noinput
%option prefix="xm_yy"
%option extra-type="xm_context_t *"

%{
#include <string.h>
#include "context.h"
#include "parser.tab.h"

#ifdef YY_BREAK
# undef YY_BREAK
#endif
#define YY_BREAK

static void
xm_set_loc (YYLTYPE *llocp, const char *data);
%}

%x has
%x string stringQ stringSQ
%x idseq

SP        [[:space:]]
NSP       [^[:space:]]
QUOTE     ["]
SQUOTE    [']
STRNQ     ([^*?'"[:space:]]|"\\".)
STRQ      ([^"]|"\\".)*
STRSQ     ([^']|"\\".)*
OPFIL     ([:~<>]|<=|>=)
OPFILSP   ([:~<>[:space:]])
NOPFIL    [^:~<>]
NOPFILSP  [^:~<>[:space:]]

%%

<INITIAL>{SP}+ {
	xm_set_loc(yylloc, yytext);
	break;
}
<INITIAL>"(" {
	xm_set_loc(yylloc, yytext);
	return TOKEN_GROUP_OPEN;
}
<INITIAL>")" {
	xm_set_loc(yylloc, yytext);
	return TOKEN_GROUP_CLOSE;
}
<INITIAL>"#" {
	xm_set_loc(yylloc, yytext);
	BEGIN (idseq);
	return TOKEN_SYMBOL_ID;
}
<INITIAL>"+"/{NSP} {
	xm_set_loc(yylloc, yytext);
	BEGIN (has);
	return TOKEN_OPFIL_HAS;
}
<INITIAL>":" {
	xm_set_loc(yylloc, yytext);
	BEGIN (string);
	xm_context_string_buffer_init (yyextra);
	return TOKEN_OPFIL_EQ;
}
<INITIAL>"~" {
	xm_set_loc(yylloc, yytext);
	BEGIN (string);
	xm_context_string_buffer_init (yyextra);
	return TOKEN_OPFIL_MATCH;
}
<INITIAL>"<=" {
	xm_set_loc(yylloc, yytext);
	BEGIN (string);
	xm_context_string_buffer_init (yyextra);
	return TOKEN_OPFIL_LE;
}
<INITIAL>">=" {
	xm_set_loc(yylloc, yytext);
	BEGIN (string);
	xm_context_string_buffer_init (yyextra);
	return TOKEN_OPFIL_GE;
}
<INITIAL>"<" {
	xm_set_loc(yylloc, yytext);
	BEGIN (string);
	xm_context_string_buffer_init (yyextra);
	return TOKEN_OPFIL_LT;
}
<INITIAL>">" {
	xm_set_loc(yylloc, yytext);
	BEGIN (string);
	xm_context_string_buffer_init (yyextra);
	return TOKEN_OPFIL_GT;
}
<INITIAL>"OR"/{SP} {
	xm_set_loc(yylloc, yytext);
	return TOKEN_OPSET_OR;
}
<INITIAL>"AND"/{SP} {
	xm_set_loc(yylloc, yytext);
	return TOKEN_OPSET_AND;
}
<INITIAL>"NOT"/{SP} {
	xm_set_loc(yylloc, yytext);
	return TOKEN_OPSET_NOT;
}
<INITIAL>"in:" {
	xm_set_loc(yylloc, yytext);
	BEGIN (string);
	xm_context_string_buffer_init (yyextra);
	return TOKEN_REFERENCE;
}

<INITIAL>[a-z]/{OPFIL} {
	xm_set_loc(yylloc, yytext);
	yylval->xstr = xm_context_string_new (yyextra, XM_STRING_TYPE_STRING, yytext);
	return TOKEN_PROP_SHORT;
}
<INITIAL>[[:alnum:]_-]+/{OPFIL} {
	xm_set_loc(yylloc, yytext);
	yylval->xstr = xm_context_string_new (yyextra, XM_STRING_TYPE_STRING, yytext);
	return TOKEN_PROP_LONG;
}

<INITIAL>[*]/{SP} {
	xm_set_loc(yylloc, yytext);
	return TOKEN_UNIVERSE;
}

<INITIAL>""/. {
	xm_set_loc(yylloc, yytext);
	/* Default case: If no token matches, assume it is a string or a pattern. */
	BEGIN (string);
	xm_context_string_buffer_init (yyextra);
	break;
}

<has>[a-z]/{SP} {
	xm_set_loc(yylloc, yytext);
	BEGIN (INITIAL);
	yylval->xstr = xm_context_string_new (yyextra, XM_STRING_TYPE_STRING, yytext);
	return TOKEN_PROP_SHORT;
}

<has>[[:alnum:]_-]+/{SP} {
	xm_set_loc(yylloc, yytext);
	BEGIN (INITIAL);
	yylval->xstr = xm_context_string_new (yyextra, XM_STRING_TYPE_STRING, yytext);
	return TOKEN_PROP_LONG;
}

<idseq>[[:digit:]]+ {
	xm_set_loc(yylloc, yytext);
	yylval->ival = (unsigned int) atoi (yytext);
	return TOKEN_INTEGER;
}
<idseq>"-" {
	xm_set_loc(yylloc, yytext);
	return TOKEN_IDSEQ_RANGE;
}
<idseq>"," {
	xm_set_loc(yylloc, yytext);
	return TOKEN_IDSEQ_SEP;
}
<idseq>""/{SP} {
	BEGIN (INITIAL);
	break;
}

<string>["] {
	xm_set_loc(yylloc, yytext);
	BEGIN (stringQ);
	break;
}
<string>['] {
	xm_set_loc(yylloc, yytext);
	BEGIN (stringSQ);
	break;
}
<string>[*?] {
	xm_set_loc(yylloc, yytext);
	xm_context_string_buffer_append_patchar (yyextra, yytext);
	break;
}
<string>[[:digit:]]+ {
	xm_set_loc(yylloc, yytext);
	xm_context_string_buffer_append_digits (yyextra, yytext);
	break;
}
<string>{STRNQ}+ {
	xm_set_loc(yylloc, yytext);
	xm_context_string_buffer_append_str (yyextra, yytext);
	break;
}
<stringQ,stringSQ>[[:digit:]]+/["'] {
	xm_set_loc(yylloc, yytext);
	xm_context_string_buffer_append_digits (yyextra, yytext);
	break;
}
<stringQ>["] {
	xm_set_loc(yylloc, yytext);
	BEGIN (string);
	break; }
<stringQ>{STRQ}*/["] {
	xm_set_loc(yylloc, yytext);
	xm_context_string_buffer_append_str (yyextra, yytext);
	break;
}
<stringSQ>['] {
	xm_set_loc(yylloc, yytext);
	BEGIN (string);
	break;
}
<stringSQ>{STRSQ}*/['] {
	xm_set_loc(yylloc, yytext);
	xm_context_string_buffer_append_str (yyextra, yytext);
	break;
}
<string,stringQ,stringSQ>""/{SP} {
	BEGIN(INITIAL);
	yylval->xstr = xm_context_string_buffer_ref (yyextra);
	xm_context_string_buffer_init (yyextra);
	switch (xm_string_get_type (yylval->xstr)) {
		case XM_STRING_TYPE_STRING:
			return TOKEN_STRING;
		case XM_STRING_TYPE_PATTERN:
			return TOKEN_PATTERN;
		case XM_STRING_TYPE_INTEGER:
			return TOKEN_NUMERIC;
		default:
			xm_string_unref (yylval->xstr);
			yylval->xstr = NULL;
			return TOKEN_ERR;
	}
}

<*>. { return TOKEN_ERR; }

%%

static void
xm_set_loc (YYLTYPE *llocp, const char *data)
{
	const char *p;
	llocp->first_line = llocp->last_line;
	llocp->first_column = llocp->last_column;
	while ((p = strchr (data, '\n')) != NULL) {
		data = p+1;
		llocp->last_line++;
		llocp->last_column = 1;
	}
	llocp->last_column += strlen(data);
}

void
xm_context_init_scanner (xm_context_t *context, const char *input)
{
	if (!context->scanner) {
		yylex_init_extra (context, (yyscan_t *)&(context->scanner));
	} else if (context->scanbuf) {
		xm_yy_delete_buffer ((YY_BUFFER_STATE) context->scanbuf,
		                     (yyscan_t *) context->scanner);
	}
	context->scanbuf = xm_yy_scan_string (input, (yyscan_t) context->scanner);
}

void
xm_context_destroy_scanner (xm_context_t *context)
{
	if (context->scanner) {
		if (context->scanbuf) {
			xm_yy_delete_buffer ((YY_BUFFER_STATE) context->scanbuf,
			                     (yyscan_t *) context->scanner);
			context->scanbuf = NULL;
		}
		xm_yylex_destroy ((yyscan_t) context->scanner);
		context->scanner = NULL;
	}
}

