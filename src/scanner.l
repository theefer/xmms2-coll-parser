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

%x string stringQ stringSQ
%x idseq

SP        [[:space:]]
NSP       [^[:space:]]
QUOTE     ["]
SQUOTE    [']
STRNQ     ([^*?'"[:space:]]|"\\".)
STRQ      ([^"]|"\\".)*
STRSQ     ([^']|"\\".)*
OPFIL     ([:~<>]||<=|>=)
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
	return TOKEN_OPFIL_HAS;
}
<INITIAL>":" {
	BEGIN(string);
	xm_context_string_init(yyextra);
	return TOKEN_OPFIL_EQ;
}
<INITIAL>"~" {
	BEGIN(string);
	xm_context_string_init(yyextra);
	return TOKEN_OPFIL_MATCH;
}
<INITIAL>"<=" {
	BEGIN(string);
	xm_context_string_init(yyextra);
	return TOKEN_OPFIL_LE;
}
<INITIAL>">=" {
	BEGIN(string);
	xm_context_string_init(yyextra);
	return TOKEN_OPFIL_GE;
}
<INITIAL>"<" {
	BEGIN(string);
	xm_context_string_init(yyextra);
	return TOKEN_OPFIL_LT;
}
<INITIAL>">" {
	BEGIN(string);
	xm_context_string_init(yyextra);
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
	xm_context_string_init(yyextra);
	return TOKEN_REFERENCE;
}

<INITIAL>[a-z]/{OPFILSP} {
	yylval->sval = strdup(yytext);
	return TOKEN_PROP_SHORT;
}
<INITIAL>[[:alnum:]_-]+/{OPFILSP} {
	yylval->sval = strdup(yytext);
	return TOKEN_PROP_LONG;
}

<INITIAL>""/. {
	/* Default case: If no token matches, assume it is a string or a pattern. */
	BEGIN(string);
	xm_context_string_init(yyextra);
	break;
}

<idseq>[[:digit:]]+ {
	yylval->ival = atoi(yytext);
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
	xm_context_string_append_patchar(yyextra, yytext);
	break;
}
<string>[[:digit:]]+ {
	xm_context_string_append_digits(yyextra, yytext);
	break;
}
<string>{STRNQ}+ {
	xm_context_string_append_str(yyextra, yytext);
	break;
}
<stringQ>["] {
	BEGIN(string);
	break; }
<stringQ>{STRQ}*/["] {
	xm_context_string_append_str(yyextra, yytext);
	break;
}
<stringSQ>['] {
  BEGIN(string);
	break;
}
<stringSQ>{STRSQ}*/['] {
	xm_context_string_append_str(yyextra, yytext);
	break;
}
<string,stringQ,stringSQ>{SP}+ {
	BEGIN(INITIAL);
	switch (xm_context_string_get_type(yyextra)) {
		case XM_STRING_TYPE_STRING:
			yylval->sval = xm_context_string_dup(yyextra);
			return TOKEN_STRING;
		case XM_STRING_TYPE_PATTERN:
			yylval->sval = xm_context_string_dup(yyextra);
			return TOKEN_PATTERN;
		case XM_STRING_TYPE_INTEGER:
			yylval->ival = xm_context_string_to_int(yyextra);
			return TOKEN_INTEGER;
		default:
			return TOKEN_ERR;
	}
}

<*>. { return TOKEN_ERR; }

%%

void
xm_scanner_init(xm_context_t *context)
{
	yylex_init_extra(context, (yyscan_t *)&(context->scanner));
}

void
xm_scanner_free(xm_context_t *context)
{
	yylex_destroy((yyscan_t)context->scanner);
}

