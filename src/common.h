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

#ifndef COMMON_H
#define COMMON_H

#include "xmmsclient/xmmsclient.h"
#include "xmmsclientpriv/xmmsclient.h"
#include "xmmsc/xmmsc_idnumbers.h"

#define XMMS_COLLECTION_PARSER_DEFAULT_NAMESPACE "Collections"

#ifndef NULL
# define NULL ((void)0)
#endif

typedef struct _xm_context xm_context_t;
typedef struct _xm_sequence xm_sequence_t;
typedef struct _xm_string xm_string_t;

typedef enum {
  XM_PARSER_CHECK_ONLY,
  XM_PARSER_COMPILE
} xm_parser_mode_t;

typedef enum {
  XM_STRING_TYPE_STRING,
  XM_STRING_TYPE_PATTERN,
  XM_STRING_TYPE_INTEGER
} xm_string_type_t;


struct _xm_context
{
  void *scanner;
  xmmsv_coll_t *result;
  xm_parser_mode_t mode;
};

struct _xm_sequence
{
  int start_range;
  int end_range;
  xm_sequence_t *prev;
  xm_sequence_t *next;
};

struct _xm_string
{
  xm_string_type_t type;
  char *value;
};


xm_context_t *
xm_context_new();

void
xm_context_store_result(xm_context_t *context, xmmsv_coll_t *result);

xmmsv_coll_t *
xm_build_union(xm_context_t *ctx, xmmsv_coll_t *or_op, xmmsv_coll_t *and_op);

xmmsv_coll_t *
xm_build_intersection(xm_context_t *ctx, xmmsv_coll_t *and_op,
                      xmmsv_coll_t *expr);

xmmsv_coll_t *
xm_build_complement(xm_context_t *ctx, xmmsv_coll_t *expr);

xmmsv_coll_t *
xm_build_reference(xm_context_t *ctx, const char *refname);

xmmsv_coll_t *
xm_build_idlist(xm_context_t *ctx, xm_sequence_t *seq);

xmmsv_coll_t *
xm_build_unary(xm_context_t *ctx, int unary_op, const char *property);

xmmsv_coll_t *
xm_build_binary_with_string(xm_context_t *ctx, int binary_op,
                            const char *property, const char *string);

xmmsv_coll_t *
xm_build_binary_with_integer(xm_context_t *ctx, int binary_op,
                             const char *property, int integer);

char *
xm_property_get_from_short(xm_context_t *ctx, const char *p);

xm_string_t *
xm_string_new(xm_context_t *ctx, xm_string_type_t type, char *value);

void
xm_string_free(xm_context_t *ctx, xm_string_t *xstr);

xm_string_t *
xm_string_new_from_integer(xm_context_t *ctx, int ivalue);

xm_sequence_t *
xm_range_new(xm_context_t *ctx, int start_range, int end_range);

xm_sequence_t *
xm_sequence_prepend_range(xm_context_t *ctx, xm_sequence_t *seq,
                          xm_sequence_t *range);

xm_sequence_t *
xm_sequence_reverse(xm_context_t *ctx, xm_sequence_t *seq);

void
xm_sequence_free(xm_context_t *ctx, xm_sequence_t *seq);

#endif /* COMMON_H */
