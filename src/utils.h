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

#ifndef XM_UTILS_H
#define XM_UTILS_H

#include <stddef.h>
#include <stdlib.h>

#ifndef NULL
# define NULL ((void)0)
#endif

#define xm_new0(type, c) ((type *) calloc((c), sizeof (type)))
#define xm_new(type, c) ((type *) malloc((c) * sizeof (type)))
#define xm_renew(buf, type, c) ((type *) realloc(buf, (c) * sizeof (type)))

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*xm_destroy_func)(void *d);

/* Misc functions. */
char *
xm_strdup_safe (const char *p);


/* Disposable struct. */
typedef struct xm_disposable xm_disposable_t;

struct xm_disposable {
	int ref;
	xm_destroy_func destroy_f;
	xm_disposable_t *prev;
	xm_disposable_t *next;
};

void
xm_disposable_init (xm_disposable_t *it, xm_destroy_func destroy_f,
                    xm_disposable_t *prev, xm_disposable_t *next);

void
xm_disposable_list_insert (xm_disposable_t *it, xm_disposable_t *prev,
                           xm_disposable_t *next);

xm_disposable_t *
xm_disposable_ref (xm_disposable_t *d);

void
xm_disposable_destroy (xm_disposable_t *d);

void
xm_disposable_unref (xm_disposable_t *d);


/* String struct. */
typedef struct xm_string xm_string_t;
typedef enum {
	XM_STRING_TYPE_NONE,
	XM_STRING_TYPE_INTEGER,
	XM_STRING_TYPE_STRING,
	XM_STRING_TYPE_PATTERN
} xm_string_type_t;

struct xm_string {
	xm_disposable_t base;
	xm_string_type_t type;
	char *value;
	int len;
};

xm_string_t *
xm_string_new (xm_string_type_t type, const char *value);

xm_string_t *
xm_string_ref (xm_string_t *xstr);

void
xm_string_unref (xm_string_t *xstr);

void
xm_string_append (xm_string_t *xstr, const char *str);

void
xm_string_prepend (xm_string_t *xstr, const char *str);

void
xm_string_numeric_add (xm_string_t *xstr, int val);

xm_string_type_t
xm_string_get_type (xm_string_t *xstr);

/* Sequence struct. */

typedef struct xm_sequence xm_sequence_t;

struct xm_sequence {
	xm_disposable_t base;
	unsigned int start;
	unsigned int end;
	xm_sequence_t *prev;
	xm_sequence_t *next;
};

xm_sequence_t *
xm_sequence_new ();

void
xm_sequence_set_range (xm_sequence_t *seq, unsigned int start, unsigned int end);

xm_sequence_t *
xm_sequence_prepend (xm_sequence_t *seq, xm_sequence_t *item);

xm_sequence_t *
xm_sequence_reverse (xm_sequence_t *seq);

xm_sequence_t *
xm_sequence_get_first (xm_sequence_t *seq);

void
xm_sequence_destroy (xm_sequence_t *seq);


/* Boxed struct. */
typedef struct xm_boxed xm_boxed_t;

struct xm_boxed {
	xm_disposable_t base;
	void *data;
	xm_destroy_func destroy_f;
};

xm_boxed_t *
xm_boxed_new (void *data, xm_destroy_func destroy_func);

xm_boxed_t *
xm_boxed_ref (xm_boxed_t *boxed);

void
xm_boxed_unref (xm_boxed_t *boxed);

#ifdef __cplusplus
}
#endif

#endif /* XM_UTILS_H */
