/*  XMMS2 Collection parser
 *  Copyright (C) 2009-2010 Raphaël Bois
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

#include <string.h>

#include "context.h"

static void
xm_context_destroy_garbage (xm_context_t *ctx)
{
	/* Thoses data are only accessed from within the parser.
	 * So it is safe to destroy remaining data outside it. */
	while (ctx->garbage_root.next) {
		xm_disposable_destroy (ctx->garbage_root.next);
	}
	ctx->scan_str = NULL; /* Was in the garbage list. */
}

static void
xm_context_push_garbage (xm_context_t *ctx, xm_disposable_t *d)
{
	xm_disposable_list_insert (d, &(ctx->garbage_root), ctx->garbage_root.next);
}

xm_context_t *
xm_context_new (xm_parser_mode_t mode)
{
	xm_context_t *ctx;

	ctx = xm_new0 (xm_context_t, 1);
	ctx->mode = mode;

	return ctx;
}



void
xm_context_prepare (xm_context_t *ctx)
{
	xm_context_destroy_garbage (ctx);
	if (ctx->result) {
		xmmsv_coll_unref (ctx->result);
		ctx->result = NULL;
	}
	if (ctx->error) {
		free (ctx->error);
		ctx->error = NULL;
	}
}

void
xm_context_free (xm_context_t *ctx)
{
	xm_context_prepare (ctx);
	xm_context_destroy_scanner (ctx);
	free (ctx);
}

/* xm_context_parse() implemented in parser.y */
/* xm_context_init_scanner() implemented in scanner.l */
/* xm_context_destroy_scanner() implemented in scanner.l */

void
xm_context_store_result (xm_context_t *ctx, xmmsv_coll_t *result)
{
	if (ctx->result) {
		xmmsv_coll_unref (ctx->result);
	}
	ctx->result = xmmsv_coll_ref (result);
}

xmmsv_coll_t *
xm_context_get_result (xm_context_t *ctx)
{
	return xmmsv_coll_ref (ctx->result);
}

void
xm_context_string_buffer_init (xm_context_t *ctx)
{
	xm_string_unref (ctx->scan_str);
	ctx->scan_str = xm_string_new (XM_STRING_TYPE_INTEGER, "");
	xm_context_push_garbage (ctx, &ctx->scan_str->base);
}

xm_string_t *
xm_context_string_buffer_ref (xm_context_t *ctx)
{
	if (ctx->scan_str) {
		xm_string_ref (ctx->scan_str);
	}
	return ctx->scan_str;
}

void
xm_context_string_buffer_append_patchar (xm_context_t *ctx, const char *pat)
{
	xm_string_append (ctx->scan_str, pat);
	ctx->scan_str->type = XM_STRING_TYPE_PATTERN;
}

void
xm_context_string_buffer_append_digits (xm_context_t *ctx, const char *digits)
{
	xm_string_append (ctx->scan_str, digits);
}

void
xm_context_string_buffer_append_str (xm_context_t *ctx, const char *str)
{
	xm_string_append (ctx->scan_str, str);
	if (ctx->scan_str->type != XM_STRING_TYPE_PATTERN) {
		if (strchr (str, '*') || strchr (str, '?')) {
			/* Forces inquotes * or ? to be special chars. */
			ctx->scan_str->type = XM_STRING_TYPE_PATTERN;
		} else {
			ctx->scan_str->type = XM_STRING_TYPE_STRING;
		}
	}
}

xm_string_t *
xm_context_string_new (xm_context_t *ctx, xm_string_type_t t, const char *value)
{
	xm_string_t *xstr;

	xstr = xm_string_new (t, value);
	xm_context_push_garbage (ctx, &xstr->base);

	return xstr;
}

xm_sequence_t *
xm_context_sequence_new_range (xm_context_t *ctx, int start, int end)
{
	xm_sequence_t *seq;

	seq = xm_sequence_new ();
	xm_context_push_garbage (ctx, &seq->base);
	xm_sequence_set_range (seq, start, end);

	return seq;
}

xm_boxed_t *
xm_context_boxed_new (xm_context_t *ctx, void *data, xm_destroy_func destroy_f)
{
	xm_boxed_t *boxed;
	
	boxed = xm_boxed_new (data, destroy_f);
	xm_context_push_garbage (ctx, &boxed->base);

	return boxed;
}
