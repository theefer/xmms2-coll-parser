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

#include <stdio.h> /* snprintf() */
#include <string.h> /* strdup() */

#include "utils.h"

/* Misc functions. */

char *
xm_strdup_safe (const char *p) {
	char *dup = NULL;
	if (p) {
		dup = strdup (p);
	}
	return dup;
}


/* Disposable. */

void
xm_disposable_init (xm_disposable_t *it, xm_destroy_func destroy_f,
                    xm_disposable_t *prev, xm_disposable_t *next)
{
	it->ref = 1;
	it->destroy_f = destroy_f;
	xm_disposable_list_insert (it, prev, next);
}
void
xm_disposable_list_insert (xm_disposable_t *it, xm_disposable_t *prev,
                           xm_disposable_t *next)
{
	if (it->prev) {
		it->prev->next = it->next;
	}
	if (it->next) {
		it->next->prev = it->prev;
	}
	it->prev = prev;
	it->next = next;
	if (prev) {
		prev->next = it;
	}
	if (next) {
		next->prev = it;
	}
}

xm_disposable_t *
xm_disposable_ref (xm_disposable_t *d)
{
	if (d) {
		d->ref++;
	}
	return d;
}

void
xm_disposable_destroy (xm_disposable_t *d)
{
	if (d) {
		xm_disposable_list_insert (d, NULL, NULL);
		if (d->destroy_f) {
			d->destroy_f (d);
		}
	}
}

void
xm_disposable_unref (xm_disposable_t *d)
{
	if (d) {
		d->ref--;
		if (!d->ref) {
			xm_disposable_destroy (d);
		}
	}
}


/* String. */

static void
xm_string_destroy (xm_string_t *xstr)
{
	if (xstr) {
		free (xstr->value);
		free (xstr);
	}
}

xm_string_t *
xm_string_new (xm_string_type_t type, const char *value)
{
	xm_string_t *xstr;

	xstr = xm_new0 (xm_string_t, 1);
	xm_disposable_init (&xstr->base, (xm_destroy_func) xm_string_destroy, NULL, NULL);
	xstr->type = type;
	xstr->value = xm_strdup_safe (value);
	if (value) {
		xstr->len = strlen (value);
	}

	return xstr;
}

xm_string_t *
xm_string_ref (xm_string_t *xstr)
{
	if (xstr) {
		xm_disposable_ref (&xstr->base);
	}
	return xstr;
}

void
xm_string_unref (xm_string_t *xstr)
{
	if (xstr) {
		xm_disposable_unref (&xstr->base);
	}
}

void
xm_string_append (xm_string_t *xstr, const char *str)
{
	int n;

	if (xstr && str) {
		n = strlen (str);
		if (!xstr->value) {
			xstr->value = xm_strdup_safe (str);
			xstr->len = n;
		} else {
			xstr->value = xm_renew (xstr->value, char, xstr->len + n + 1);
			strncat (xstr->value, str, n);
			xstr->len += n;
		}
	}
}

void
xm_string_prepend (xm_string_t *xstr, const char *str)
{
	int n;
	char *value;

	if (xstr && str) {
		value = xstr->value;
		n = strlen (str);
		xstr->value = xm_new (char, xstr->len + n + 1);
		strncpy (xstr->value, str, n);
		if (xstr->len) {
			strncpy (&(xstr->value[n]), value, xstr->len);
		}
		xstr->len += n;
		xstr->value[xstr->len] = '\0';
		free(value);
	}
}

void
xm_string_numeric_add (xm_string_t *xstr, int val)
{
	char buf[64] = "";

	if (xstr && xstr->type == XM_STRING_TYPE_INTEGER) {
		snprintf (buf, 64, "%d", atoi(xstr->value) + val);
		free (xstr->value);
		xstr->value = xm_strdup_safe (buf);
		xstr->len = strlen (xstr->value);
  }
}

xm_string_type_t
xm_string_get_type (xm_string_t *xstr)
{
  if (xstr) {
    return xstr->type;
  }
  return XM_STRING_TYPE_NONE;
}


/* Sequence. */

xm_sequence_t *
xm_sequence_new ()
{
	xm_sequence_t *newit;

	newit = xm_new0 (xm_sequence_t, 1);
	xm_disposable_init (&newit->base, (xm_destroy_func) free, NULL, NULL);

	newit->start = 1; /* start > end  ==> empty id list. */
	return newit;
}

void
xm_sequence_set_range (xm_sequence_t *seq, unsigned int start, unsigned int end)
{
	if (seq) {
		if (start == 0) {
			start = 1;
		}
		seq->start = start;
		seq->end = end;
	}
}

xm_sequence_t *
xm_sequence_prepend (xm_sequence_t *seq, xm_sequence_t *item)
{
	if (item) {
		item->next = seq;
		if (seq) {
			seq->prev = item;
		}
	} else {
		item = seq;
	}
	return item;
}

xm_sequence_t *
xm_sequence_reverse (xm_sequence_t *seq)
{
	xm_sequence_t *iter = NULL, *next = seq;
	while (next) {
		iter = next;
		next = iter->next;
		iter->next = iter->prev;
		iter->prev = next;
	}
	return iter;
}

xm_sequence_t *
xm_sequence_get_first (xm_sequence_t *seq)
{
	if (seq) {
		while (seq->prev) {
			seq = seq->prev;
		}
	}
	return seq;
}

/* To simplify the implementation and to avoid side-effects,
 * sequences do not support references tracking.
 */
void
xm_sequence_destroy (xm_sequence_t *seq)
{
	xm_sequence_t *iter, *next;
	next = xm_sequence_get_first (seq);
	while (next) {
		iter = next;
		next = iter->next;
		xm_disposable_destroy (&iter->base);
	}
}


/* Boxed */
static void
xm_boxed_destroy (xm_boxed_t *boxed)
{
	if (boxed) {
		if (boxed->data && boxed->destroy_f) {
			boxed->destroy_f (boxed->data);
		}
		free(boxed);
	}
}

xm_boxed_t *
xm_boxed_new (void *data, xm_destroy_func destroy_f)
{
	xm_boxed_t *newboxed;

	newboxed = xm_new0 (xm_boxed_t, 1);
	xm_disposable_init (&newboxed->base, (xm_destroy_func) xm_boxed_destroy,
	                    NULL, NULL);
	newboxed->data = data;
	newboxed->destroy_f = destroy_f;
	return newboxed;
}

xm_boxed_t *
xm_boxed_ref (xm_boxed_t *boxed)
{
	if (boxed) {
		xm_disposable_ref (&boxed->base);
	}
	return boxed;
}

void
xm_boxed_unref (xm_boxed_t *boxed)
{
	if (boxed) {
		xm_disposable_unref (&boxed->base);
	}
}
