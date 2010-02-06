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
#include "common.h"
#include "parser.tab.h"

#ifdef YY_BREAK
# undef YY_BREAK
#endif
#define YY_BREAK
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
	break;
}
<INITIAL>"(" {
	return TOKEN_GROUP_OPEN;
}
<INITIAL>")" {
	return TOKEN_GROUP_CLOSE;
}
<INITIAL>"#" {
	BEGIN(idseq);
	return TOKEN_SYMBOL_ID;
}
<INITIAL>"+"/{NSP} {
	BEGIN(has);
	return TOKEN_OPFIL_HAS;
}
<INITIAL>":" {
	BEGIN(string);
	xm_context_string_buffer_init(yyextra);
	return TOKEN_OPFIL_EQ;
}
<INITIAL>"~" {
	BEGIN(string);
	xm_context_string_buffer_init(yyextra);
	return TOKEN_OPFIL_MATCH;
}
<INITIAL>"<=" {
	BEGIN(string);
	xm_context_string_buffer_init(yyextra);
	return TOKEN_OPFIL_LE;
}
<INITIAL>">=" {
	BEGIN(string);
	xm_context_string_buffer_init(yyextra);
	return TOKEN_OPFIL_GE;
}
<INITIAL>"<" {
	BEGIN(string);
	xm_context_string_buffer_init(yyextra);
	return TOKEN_OPFIL_LT;
}
<INITIAL>">" {
	BEGIN(string);
	xm_context_string_buffer_init(yyextra);
	return TOKEN_OPFIL_GT;
}
<INITIAL>"OR"/{SP} {
	return TOKEN_OPSET_OR;
}
<INITIAL>"AND"/{SP} {
	return TOKEN_OPSET_AND;
}
<INITIAL>"NOT"/{SP} {
	return TOKEN_OPSET_NOT;
}
<INITIAL>"in:" {
	BEGIN(string);
	xm_context_string_buffer_init(yyextra);
	return TOKEN_REFERENCE;
}

<INITIAL>[a-z]/{OPFIL} {
	yylval->xstr = xm_context_string_new(yyextra, XM_STRING_TYPE_STRING, yytext);
	return TOKEN_PROP_SHORT;
}
<INITIAL>[[:alnum:]_-]+/{OPFIL} {
	yylval->xstr = xm_context_string_new(yyextra, XM_STRING_TYPE_STRING, yytext);
	return TOKEN_PROP_LONG;
}

<INITIAL>""/. {
	/* Default case: If no token matches, assume it is a string or a pattern. */
	BEGIN(string);
	xm_context_string_buffer_init(yyextra);
	break;
}

<has>[a-z]/{SP} {
	BEGIN(INITIAL);
	yylval->xstr = xm_context_string_new(yyextra, XM_STRING_TYPE_STRING, yytext);
	return TOKEN_PROP_SHORT;
}

<has>[[:alnum:]_-]+/{SP} {
	BEGIN(INITIAL);
	yylval->xstr = xm_context_string_new(yyextra, XM_STRING_TYPE_STRING, yytext);
	return TOKEN_PROP_LONG;
}

<idseq>[[:digit:]]+ {
	yylval->ival = (unsigned int) atoi(yytext);
	return TOKEN_INTEGER;
}
<idseq>"-" {
	return TOKEN_IDSEQ_RANGE;
}
<idseq>"," {
	return TOKEN_IDSEQ_SEP;
}
<idseq>{SP}+ {
	BEGIN(INITIAL);
	break;
}

<string>["] {
	BEGIN(stringQ);
	break;
}
<string>['] {
	BEGIN(stringSQ);
	break;
}
<string>[*?] {
	xm_context_string_buffer_append_patchar(yyextra, yytext);
	break;
}
<string>[[:digit:]]+ {
	xm_context_string_buffer_append_digits(yyextra, yytext);
	break;
}
<string>{STRNQ}+ {
	xm_context_string_buffer_append_str(yyextra, yytext);
	break;
}
<stringQ,stringSQ>[[:digit:]]+/["'] {
	xm_context_string_buffer_append_digits(yyextra, yytext);
	break;
}
<stringQ>["] {
	BEGIN(string);
	break; }
<stringQ>{STRQ}*/["] {
	xm_context_string_buffer_append_str(yyextra, yytext);
	break;
}
<stringSQ>['] {
	BEGIN(string);
	break;
}
<stringSQ>{STRSQ}*/['] {
	xm_context_string_buffer_append_str(yyextra, yytext);
	break;
}
<string,stringQ,stringSQ>{SP}+ {
	BEGIN(INITIAL);
	yylval->xstr = xm_context_string_buffer_ref(yyextra);
	xm_context_string_buffer_init(yyextra);
	switch (xm_string_get_type(yylval->xstr)) {
		case XM_STRING_TYPE_STRING:
			return TOKEN_STRING;
		case XM_STRING_TYPE_PATTERN:
			return TOKEN_PATTERN;
		case XM_STRING_TYPE_INTEGER:
			return TOKEN_NUMERIC;
		default:
			xm_string_unref(yylval->xstr);
			yylval->xstr = NULL;
			return TOKEN_ERR;
	}
}

<*>. { return TOKEN_ERR; }

%%

void
xm_context_init_scanner(xm_context_t *context, const char *input)
{
	if (!context->scanner) {
		yylex_init_extra(context, (yyscan_t *)&(context->scanner));
	}
	context->scanbuf = xm_yy_scan_string(input, (yyscan_t) context->scanner);
}

void
xm_context_destroy_scanner(xm_context_t *context)
{
	if (context->scanner) {
		if (context->scanbuf) {
			xm_yy_delete_buffer((YY_BUFFER_STATE) context->scanbuf,
			                    (yyscan_t *) context->scanner);
			context->scanbuf = NULL;
		}
		xm_yylex_destroy((yyscan_t) context->scanner);
		context->scanner = NULL;
	}
}

