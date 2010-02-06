/*  XMMS2 Collection parser.
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
 *
 */

#include "coll-parser.h"
#include "context.h"

static xmmsv_t *
xm_coll_parse(const char *pattern, xm_parser_mode_t mode)
{
	xm_context_t *ctx;
	xmmsv_t *result = NULL;

	ctx = xm_context_new(mode);

	if (!xm_context_parse(ctx, pattern)) {
		result = xmmsv_new_error(ctx->error);
	} else if (ctx->mode == XM_PARSER_COMPILE && ctx->result) {
		result = xmmsv_new_coll(ctx->result);
	}
	xm_context_free(ctx);

	return result;
}

xmmsv_t *
xm_coll_compile_pattern(const char *pattern)
{
	return xm_coll_parse(pattern, XM_PARSER_COMPILE);
}

int
xm_coll_check_pattern(const char *pattern, xmmsv_t **error)
{
	xmmsv_t *ret;
	
	ret = xm_coll_parse(pattern, XM_PARSER_CHECK_ONLY);
	if (ret) {
		if (error) {
			*error = xmmsv_ref(ret);
		}
		xmmsv_unref(ret);
	}
	return !(ret);
}
