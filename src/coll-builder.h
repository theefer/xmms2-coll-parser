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

#ifndef XM_COLL_BUILDER_H
#define XM_COLL_BUILDER_H

#include "context.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#endif /* XM_COLL_BUILDER_H */
