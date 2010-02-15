/*  XMMS2 Collection parser
 *  Copyright (C) 2009-2010 Raphaël Bois
 *
 *  Some parts of the code are based on code from the XMMS2's xmmsclient
 *  library:
 *  Copyright (C) 2003-2010 XMMS2 Team
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

#include "coll-builder.h"

#define XM_COLL_PARSER_DEFAULT_NAMESPACE "Collections"

static void
coll_append_universe (xmmsv_coll_t *coll);

static xmmsv_coll_t *
xm_build_and_or (xm_context_t *ctx, xmmsv_coll_t *act_op, xmmsv_coll_t *op,
                 xmmsv_coll_type_t type);

static char *
xm_extract_namespace (const char *refname, const char **ref);

static int
xm_idlist_sort (const void *i1, const void *i2);


typedef struct {
	char shortstr;
	const char *longstr;
} xm_coll_prop_t;

static const xm_coll_prop_t xm_coll_prop_short[] = {
	{ 'a', "artist" },
	{ 'l', "album"},
	{ 't', "title"},
	{ 'n', "tracknr"},
	{ 'y', "year"},
	{ 'g', "genre"},
	{ 'u', "url"},
	{ '\0', NULL }
};


/* from xmmsclient's collparser.c*/
static void
coll_append_universe (xmmsv_coll_t *coll)
{
	xmmsv_coll_t *univ;

	univ = xmmsv_coll_universe ();
	xmmsv_coll_add_operand (coll, univ);
	xmmsv_coll_unref (univ);
}

static xmmsv_coll_t *
xm_build_and_or (xm_context_t *ctx, xmmsv_coll_t *act_op, xmmsv_coll_t *op,
                 xmmsv_coll_type_t type)
{
	xmmsv_coll_t *coll, *coll2;
	xmmsv_t *operands;
	xmmsv_list_iter_t *iter;

	if (xmmsv_coll_get_type (act_op) == type) {
		coll = xmmsv_coll_ref(act_op);
	} else {
		coll = xmmsv_coll_new (type);
		xmmsv_coll_add_operand (coll, act_op);
	}
	if (xmmsv_coll_get_type (op) == type) {
		operands = xmmsv_coll_operands_get (op);
		xmmsv_get_list_iter (operands, &iter);
		while (xmmsv_list_iter_valid(iter)) {
			xmmsv_list_iter_entry_coll(iter, &coll2);
			xmmsv_coll_ref(coll2);
			xmmsv_list_iter_remove(iter);
			xmmsv_coll_add_operand (coll, coll2);
			xmmsv_coll_unref(coll2);
		}
		xmmsv_list_iter_explicit_destroy (iter);
	} else {
		xmmsv_coll_add_operand (coll, op);
	}

	return coll;
}

static char *
xm_extract_namespace (const char *refname, const char **ref)
{
	size_t len;
	char *namespace = NULL, *slash;

	if (refname) {
		slash = strchr (refname, '/');
		if (!slash) {
			namespace = xm_strdup_safe (XM_COLL_PARSER_DEFAULT_NAMESPACE);
			if (ref) {
				*ref = refname;
			}
		} else {
			len = (size_t)(slash - refname);
			namespace = xm_new (char, len + 1);
			strncpy (namespace, refname, len);
			namespace[len] = '\0';
			if (ref) {
				*ref = slash + 1;
			}
		}
	}
	return namespace;
}

static int
xm_idlist_sort(const void *i1, const void *i2)
{
	return *((int *)i1) - *((int *)i2);
}

/* l1 and l2 must be sorted with no dupplicated entries.
 * (it is the case when an idlist is generated) */
static void
xm_idlist_merge(unsigned int *dest, const uint32_t *l1, size_t s1,
                const uint32_t *l2, size_t s2)
{
	int i, j, k;
	for (i = 0, j = 0, k = 0; i < s1; i++) {
		if (j >= s2 || l1[i] < l2[j]) {
			dest[k] = l1[i];
			k++;
		} else {
			for (; j < s2 && l2[j] <= l1[i]; j++) {
				dest[k] = l2[j];
				k++;
			}
		}
	}
	for (; j < s2; j++) {
		dest[k] = l2[j];
		k++;
	}
	dest[k] = 0;
}

static xmmsv_coll_t *
xm_coll_merge_idlists (xmmsv_coll_t *coll)
{
	xmmsv_coll_t *coll_idlist, *coll_cur;
	xmmsv_t *operands;
	xmmsv_list_iter_t *iter;
	const uint32_t *ids_main, *ids_cur;
	unsigned int *ids_new;
	size_t main_size, new_size, cur_size;

	/* Merge IdLists. */
	coll_idlist = NULL;
	coll_cur = NULL;
	operands = xmmsv_coll_operands_get (coll);
	xmmsv_get_list_iter (operands, &iter);
	while (xmmsv_list_iter_valid (iter)) {
		xmmsv_list_iter_entry_coll (iter, &coll_cur);
		if (xmmsv_coll_get_type (coll_cur) == XMMS_COLLECTION_TYPE_IDLIST) {
			if (coll_idlist) {
				xmmsv_coll_ref (coll_cur);
				xmmsv_list_iter_remove (iter);

				ids_main = xmmsv_coll_get_idlist (coll_idlist);
				ids_cur = xmmsv_coll_get_idlist (coll_cur);
				main_size = xmmsv_coll_idlist_get_size (coll_idlist);
				cur_size = xmmsv_coll_idlist_get_size (coll_cur);
				new_size = main_size + cur_size;
				ids_new = xm_new (unsigned int, new_size + 1);

				xm_idlist_merge (ids_new, ids_main, main_size, ids_cur, cur_size);
				xmmsv_coll_set_idlist (coll_idlist, ids_new);

				free (ids_new);
				xmmsv_coll_unref (coll_cur);
			} else {
				coll_idlist = coll_cur;
				xmmsv_list_iter_next (iter);
			}
		} else {
			xmmsv_list_iter_next (iter);
		}
	}
	xmmsv_list_iter_explicit_destroy (iter);

	/* If IDList is alone, remove the parent. */
	if (xmmsv_list_get_size (operands) == 1) {
		xmmsv_list_get_coll (operands, 0, &coll_cur);
		xmmsv_coll_ref (coll_cur);
		xmmsv_coll_remove_operand (coll, coll_cur);
		xmmsv_coll_unref (coll);
		coll = coll_cur;
	}

	return coll;
}

xmmsv_coll_t *
xm_build_union (xm_context_t *ctx, xmmsv_coll_t *or_op, xmmsv_coll_t *and_op)
{
	xmmsv_coll_t *coll;

	coll = xm_build_and_or (ctx, or_op, and_op, XMMS_COLLECTION_TYPE_UNION);
	coll = xm_coll_merge_idlists (coll);

	return coll;
}

xmmsv_coll_t *
xm_build_intersection (xm_context_t *ctx, xmmsv_coll_t *and_op,
                       xmmsv_coll_t *expr)
{
	xmmsv_coll_t *coll;
	coll = xm_build_and_or (ctx, and_op, expr, XMMS_COLLECTION_TYPE_INTERSECTION);
	return coll;
}

xmmsv_coll_t *
xm_build_complement (xm_context_t *ctx, xmmsv_coll_t *expr)
{
	xmmsv_coll_t *coll;

	coll = xmmsv_coll_new (XMMS_COLLECTION_TYPE_COMPLEMENT);
	xmmsv_coll_add_operand (coll, expr);

	return coll;
}

xmmsv_coll_t *
xm_build_reference (xm_context_t *ctx, const char *refname)
{
	char *namespace;
	const char *ref = NULL;
	xmmsv_coll_t *coll;

	namespace = xm_extract_namespace (refname, &ref);

	coll = xmmsv_coll_new (XMMS_COLLECTION_TYPE_REFERENCE);
	xmmsv_coll_attribute_set (coll, "namespace", namespace);
	xmmsv_coll_attribute_set (coll, "reference", ref);

	free (namespace);

	return coll;
}

xmmsv_coll_t *
xm_build_universe (xm_context_t *ctx)
{
	return xmmsv_coll_universe ();
}

xmmsv_coll_t *
xm_build_idlist (xm_context_t *ctx, xm_sequence_t *seq)
{
	xmmsv_coll_t *coll;
	unsigned int len, i, j, last;
	unsigned int *idlist;
	xm_sequence_t *iter;

	for (len = 0, iter = seq; iter; iter = iter->next) {
		if (iter->start <= iter->end) {
			len += iter->end - iter->start + 1;
		}
	}
	idlist = xm_new (unsigned int, len + 1);
	for (i = 0, iter = seq; iter; iter = iter->next) {
		for (j = iter->start; j <= iter->end; j++) {
			idlist[i] = j;
			i++;
		}
	}
	idlist[i] = 0;
	qsort(idlist, len, sizeof (unsigned int), xm_idlist_sort);

	/* Now removes dupplicated entries. */
	for (last=0,j=0,i=0; i < len; i++) {
		if (idlist[i] != last) {
			last = idlist[i];
			if (i != j) {
				idlist[j] = idlist[i];
			}
			j++;
		}
	}
	idlist[j] = 0;
	idlist = xm_renew(idlist, unsigned int, j+1);

	coll = xmmsv_coll_new (XMMS_COLLECTION_TYPE_IDLIST);
	xmmsv_coll_set_idlist (coll, idlist);
	free (idlist);

	return coll;
}

xmmsv_coll_t *
xm_build_unary (xm_context_t *ctx, xmmsv_coll_type_t unary_op,
                const char *property)
{
	xmmsv_coll_t *coll;

	coll = xmmsv_coll_new (unary_op);
	xmmsv_coll_attribute_set (coll, "field", property);
	coll_append_universe (coll);

	return coll;
}

xmmsv_coll_t *
xm_build_binary_e (xm_context_t *ctx, xmmsv_coll_type_t lge_op,
                   const char *property, xm_string_t *xstr)
{
	xm_string_t *cpy;
	xmmsv_coll_t *coll;

	cpy = xm_string_new (xstr->type, xstr->value);
	switch (lge_op) {
		case XMMS_COLLECTION_TYPE_GREATER:
			xm_string_numeric_add (cpy, -1);
			break;
		case XMMS_COLLECTION_TYPE_SMALLER:
			xm_string_numeric_add (cpy, 1);
			break;
		default:
			break;
	}
	coll = xm_build_binary (ctx, lge_op, property, cpy);
	xm_string_unref (cpy);
	return coll;
}

xmmsv_coll_t *
xm_build_binary (xm_context_t *ctx, xmmsv_coll_type_t binary_op,
                 const char *property, xm_string_t *xstr)
{
	xmmsv_coll_t *coll, *m1, *m2, *m3;
	xm_string_t *mstr = NULL;

	if (property) {
		switch (binary_op) {
			case XMMS_COLLECTION_TYPE_EQUALS:
				if (strchr (xstr->value, '*') || strchr (xstr->value, '?')) {
					binary_op = XMMS_COLLECTION_TYPE_MATCH;
				}
				mstr = xm_string_ref (xstr);
				break;
			case XMMS_COLLECTION_TYPE_MATCH:
				mstr = xm_string_new (XM_STRING_TYPE_PATTERN, xstr->value);
				if (mstr->value[0] != '*') {
					xm_string_prepend (mstr, "*");
				}
				if (mstr->value[mstr->len-1] != '*') {
					xm_string_append (mstr, "*");
				}
				break;
			default:
				mstr = xm_string_ref (xstr);
				break;
		}

		coll = xmmsv_coll_new (binary_op);
		xmmsv_coll_attribute_set (coll, "field", property);
		xmmsv_coll_attribute_set (coll, "value", mstr->value);
		xm_string_unref (mstr);
		coll_append_universe (coll);
	} else {
		m1 = xm_build_binary (ctx, binary_op, "artist", xstr);
		m2 = xm_build_binary (ctx, binary_op, "album", xstr);
		m3 = xm_build_binary (ctx, binary_op, "title", xstr);
		coll = xm_build_union (ctx, m1, m2);
		coll = xm_build_union (ctx, coll, m3);
		xmmsv_coll_unref(m1);
		xmmsv_coll_unref(m2);
		xmmsv_coll_unref(m3);
	}
	return coll;
}

xm_string_t *
xm_property_get_from_short (xm_context_t *ctx, xm_string_t *p)
{
	const xm_coll_prop_t *iter;
	const char *prop_name = p->value;

	for (iter = xm_coll_prop_short; iter->shortstr; iter++) {
		if (p->value[0] == iter->shortstr) {
			prop_name = iter->longstr;
			break;
		}
	}

	return xm_context_string_new (ctx, XM_STRING_TYPE_STRING, prop_name);
}
