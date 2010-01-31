/*  XMMS2 Collection parser
 *  Copyright (C) 2009 Raphaël Bois
 *
 *  Mostly copied from the collparser.c module of the XMMS2's xmmsclient
 *  library:
 *  Copyright (C) 2003-2009 XMMS2 Team
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

typedef struct {
	char shortstr;
	const char *longstr;
} xm_coll_prop_t;

static const xm_coll_prop_t
xm_coll_prop_short[] = { { 'a', "artist" },
                         { 'l', "album"},
                         { 't', "title"},
                         { 'n', "tracknr"},
                         { 'y', "year"},
                         { 'g', "genre"},
                         { 'u', "url"},
												 { '\0', NULL } };

static inline char *
xm_strdup_safe(const char *p) {
  char *dup = NULL;
  if (p) {
    dup = strdup(p);
  }
  return dup;
}

xm_context_t *
xm_context_new(xm_parser_mode_t mode)
{
  xm_context_t *ctx;

  ctx = (xm_context_t *) malloc(sizeof (xm_context_t));
  ctx->scanner = NULL;
  ctx->result = NULL;
  ctx->mode = mode;
  ctx->scan_str = NULL;

  return ctx;
}

void
xm_context_store_result(xm_context_t *context, xmmsv_coll_t *result)
{
  context->result = result;
}

void
xm_context_string_init(xm_context_t *context)
{
  xm_string_free(NULL, context->scan_str);
  context->scan_str = xm_string_new(NULL, XM_STRING_TYPE_INTEGER, "");
}

char *
xm_context_string_dup(xm_context_t *ctx)
{
  if (!ctx || !ctx->scan_str) {
    return NULL;
  }
  return xm_strdup_safe(ctx->scan_str->value);
}

void
xm_context_string_append_patchar(xm_context_t *ctx, const char *pat)
{
  xm_string_append(ctx->scan_str, pat);
  ctx->scan_str->type = XM_STRING_TYPE_PATTERN;
}

void
xm_context_string_append_digits(xm_context_t *ctx, const char *digits)
{
  xm_string_append(ctx->scan_str, digits);
}

void
xm_context_string_append_str(xm_context_t *ctx, const char *str)
{
  xm_string_append(ctx->scan_str, str);
  if (ctx->scan_str->type != XM_STRING_TYPE_PATTERN) {
    ctx->scan_str->type = XM_STRING_TYPE_STRING;
  }
}

xmmsv_coll_t *
xm_build_union(xm_context_t *ctx, xmmsv_coll_t *or_op, xmmsv_coll_t *and_op)
{

  /* TODO: implement xm_build_union() */
  return NULL;
}

xmmsv_coll_t *
xm_build_intersection(xm_context_t *ctx, xmmsv_coll_t *and_op,
                      xmmsv_coll_t *expr)
{
  /* TODO: implement xm_build_intersection() */
  return NULL;
}

xmmsv_coll_t *
xm_build_complement(xm_context_t *ctx, xmmsv_coll_t *expr)
{
  /* TODO: implement xm_build_complement() */
  return NULL;
}

xmmsv_coll_t *
xm_build_reference(xm_context_t *ctx, const char *refname)
{
  /* TODO: implement xm_build_reference() */
  return NULL;
}

xmmsv_coll_t *
xm_build_idlist(xm_context_t *ctx, xm_sequence_t *seq)
{
  /* TODO: implement xm_build_idlist() */
  return NULL;
}

xmmsv_coll_t *
xm_build_unary(xm_context_t *ctx, int unary_op, const char *property)
{
 /* TODO: implement xm_build_unary() */ 
  return NULL;
}

xmmsv_coll_t *
xm_build_binary_with_string(xm_context_t *ctx, int binary_op,
                            const char *property, xm_string_t *string)
{
  /* TODO: implement xm_build_binary_with_string() */
  return NULL;
}

xmmsv_coll_t *
xm_build_binary_with_integer(xm_context_t *ctx, int binary_op,
                             const char *property, int integer)
{
  /* TODO: implement xm_build_binary_with_integer() */
  return NULL;
}

char *
xm_property_get_from_short(xm_context_t *ctx, const char *p)
{
  const xm_coll_prop_t *cp;
  const char *prop_name = p;

  for (cp = xm_coll_prop_short; cp->shortstr; cp++) {
    if (p[0] == cp->shortstr) {
      prop_name = cp->longstr;
      break;
    }
  }
  return xm_strdup_safe(prop_name);
}

xm_string_t *
xm_string_new(xm_context_t *ctx, xm_string_type_t type, char *value)
{
  xm_string_t *xstr;

  xstr = (xm_string_t *) malloc(sizeof (xm_string_t));
  xstr->type = type;
  xstr->value = value;
  return xstr;
}

void
xm_string_free(xm_context_t *ctx, xm_string_t *xstr)
{
  if (!xstr) return;
  free(xstr->value);
  free(xstr);
}

xm_string_t *
xm_string_new_from_integer(xm_context_t *ctx, int ivalue)
{
  char sval[24];
  snprintf(sval, sizeof(sval), "%d", ivalue);
  return xm_string_new(ctx, XM_STRING_TYPE_INTEGER, xm_strdup_safe(sval));
}

void
xm_string_append(xm_string_t *xstr, const char *str)
{
  if (!xstr) return;
  if (!xstr->value) {
    xstr->value = xm_strdup_safe(str);
  } else {
    int n = strlen(str);
    xstr->value = realloc(xstr->value, strlen(xstr->value) + n + 1);
    strncat(xstr->value, str, n);
  }
}

xm_sequence_t *
xm_range_new(xm_context_t *ctx, int start_range, int end_range)
{
  xm_sequence_t *newit;

  newit = (xm_sequence_t *) malloc(sizeof (xm_sequence_t));
  newit->start_range = start_range;
  newit->end_range = end_range;
  newit->prev = NULL;
  newit->next = NULL;
  return newit;
}

xm_sequence_t *
xm_sequence_prepend_range(xm_context_t *ctx, xm_sequence_t *seq,
                          xm_sequence_t *range)
{
  if (range) {
    range->next = seq;
    seq = range;
  }
  return seq;
}

xm_sequence_t *
xm_sequence_reverse(xm_context_t *ctx, xm_sequence_t *seq)
{
  xm_sequence_t *iter, *next = seq;
  while (next) {
    iter = next;
    next = iter->next;
    iter->next = iter->prev;
    iter->prev = next;
  }
  return iter;
}

void
xm_sequence_free(xm_context_t *ctx, xm_sequence_t *seq)
{
  xm_sequence_t *iter, *next = seq;
  while(next) {
    iter = next;
    next = iter->next;
    free(iter);
  }
}
