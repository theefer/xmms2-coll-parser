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

#ifndef COMMON_H
#define COMMON_H

#include "error.h"
#include "utils.h"

#include <xmmsc/xmmsv.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _xm_context xm_context_t;

typedef enum {
	XM_PARSER_CHECK_ONLY,
	XM_PARSER_COMPILE
} xm_parser_mode_t;

struct _xm_context
{
	void *scanner;
	void *scanbuf;
	xmmsv_coll_t *result;
	char *error;
	xm_parser_mode_t mode;
	xm_string_t *scan_str;

	xm_disposable_t garbage_root;
};

xm_context_t *
xm_context_new(xm_parser_mode_t mode);

void
xm_context_prepare(xm_context_t *ctx);

void
xm_context_free(xm_context_t *ctx);

/* in parser.y */
int
xm_context_parse(xm_context_t *ctx, const char *pat);
/* */

/* in scanner.l */
void
xm_context_init_scanner(xm_context_t *ctx, const char *input);

void
xm_context_destroy_scanner(xm_context_t *ctx);
/* */

void
xm_context_store_result(xm_context_t *ctx, xmmsv_coll_t *result);

xmmsv_coll_t *
xm_context_get_result(xm_context_t *ctx);

void
xm_context_string_buffer_init(xm_context_t *ctx);

xm_string_t *
xm_context_string_buffer_ref(xm_context_t *ctx);

void
xm_context_string_buffer_append_patchar(xm_context_t *ctx, const char *pat);

void
xm_context_string_buffer_append_digits(xm_context_t *ctx, const char *digits);

void
xm_context_string_buffer_append_str(xm_context_t *ctx, const char *str);

xm_string_t *
xm_context_string_new(xm_context_t *ctx, xm_string_type_t t, const char *value);

xm_sequence_t *
xm_context_sequence_new_range(xm_context_t *ctx, int start, int end);

xm_boxed_t *
xm_context_boxed_new(xm_context_t *ctx, void *data, xm_destroy_func destroy_f);



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
xm_build_unary(xm_context_t *ctx, xmmsv_coll_type_t unary_op,
               const char *property);

xmmsv_coll_t *
xm_build_binary_e(xm_context_t *ctx, xmmsv_coll_type_t lge_op,
									const char *property, xm_string_t *xstr);

xmmsv_coll_t *
xm_build_binary(xm_context_t *ctx, xmmsv_coll_type_t binary_op,
                const char *property, xm_string_t *xstr);

xm_string_t *
xm_property_get_from_short(xm_context_t *ctx, xm_string_t *p);


#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */
