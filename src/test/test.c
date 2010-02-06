/*  XMMS2 Collection parser -- Test application.
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
 *  
 *  NOTE:
 *    Functions coll_* and print_info are copied from nycli, from the XMMS2
 *    project:
 *    Copyright (C) 2003-2010 XMMS2 Team.
 */

#include <glib.h>
#include <gio/gio.h>
#include "coll-parser.h"

static void
coll_dump (xmmsv_coll_t *coll, guint level);

gint main() {
  GIOChannel *in_channel = g_io_channel_unix_new(0); /* stdin */
  GString *linebuf = g_string_new("");
  const char *errval;
  gsize termpos = 0;
  GError *err = NULL;
  xmmsv_t *value;
  xmmsv_coll_t *coll = NULL;

  while (g_io_channel_read_line_string(in_channel, linebuf, &termpos, &err) == G_IO_STATUS_NORMAL) {
    g_string_overwrite(linebuf, termpos, "");
    g_printf("QUERY: %s", linebuf->str);
    value = xm_coll_compile_pattern(linebuf->str);
    if (xmmsv_is_error(value)) {
      xmmsv_get_error(value, &errval);
      g_printf("ERROR: %s\n", errval);
    } else {
      xmmsv_get_coll(value, &coll);
      if (coll) {
        coll_dump(coll, 0);
      } else {
        g_printf("ERROR: No collection.\n");
      }
    }
    g_printf("\n");
    xmmsv_unref(value);
  };
  if (err) {
    g_critical("%s", err->message);
    g_error_free(err);
    return 1;
  }
  return 0;
}


/* Function copied from nycli. */
static void
coll_dump_list (xmmsv_t *list, unsigned int level)
{
	xmmsv_list_iter_t *it;
	xmmsv_coll_t *operand;
	xmmsv_t *v;

	xmmsv_get_list_iter (list, &it);
	while (xmmsv_list_iter_entry (it, &v)) {
		if (xmmsv_get_coll (v, &operand)) {
			coll_dump (operand, level);
		}
		xmmsv_list_iter_next (it);
	}

}

/* Produce a GString from the idlist of the collection.
   (must be freed manually!)
   (from src/clients/cli/cmd_coll.c) */
static GString *
coll_idlist_to_string (xmmsv_coll_t *coll)
{
	gint i;
	guint *idlist;
	GString *s;

	s = g_string_new ("(");

	idlist = xmmsc_coll_get_idlist (coll);
	for (i = 0; idlist[i] != 0; ++i) {
		if (i > 0) {
			g_string_append (s, ", ");
		}
		g_string_append_printf (s, "%d", idlist[i]);
	}
	g_string_append_c (s, ')');

	return s;
}

/* (from src/clients/cli/common.c) */
static void
print_info (const gchar *fmt, ...)
{
	gchar buf[8096];
	va_list ap;

	va_start (ap, fmt);
	g_vsnprintf (buf, 8096, fmt, ap);
	va_end (ap);

	g_printf ("%s\n", buf);
}

/* Function copied from nycli. */
static void
coll_dump (xmmsv_coll_t *coll, guint level)
{
	gint i;
	gchar *indent;

	gchar *attr1;
	gchar *attr2;
	GString *idlist_str;

	indent = g_malloc ((level * 2) + 1);
	for (i = 0; i < level * 2; ++i) {
		indent[i] = ' ';
	}
	indent[i] = '\0';

	/* type */
	switch (xmmsc_coll_get_type (coll)) {
	case XMMS_COLLECTION_TYPE_REFERENCE:
		xmmsc_coll_attribute_get (coll, "reference", &attr1);
		print_info ("%sReference: '%s'", indent, attr1);
		break;

	case XMMS_COLLECTION_TYPE_UNION:
		print_info ("%sUnion:", indent);
		coll_dump_list (xmmsv_coll_operands_get (coll), level + 1);
		break;

	case XMMS_COLLECTION_TYPE_INTERSECTION:
		print_info ("%sIntersection:", indent);
		coll_dump_list (xmmsv_coll_operands_get (coll), level + 1);
		break;

	case XMMS_COLLECTION_TYPE_COMPLEMENT:
		print_info ("%sComplement:", indent);
		coll_dump_list (xmmsv_coll_operands_get (coll), level + 1);
		break;

	case XMMS_COLLECTION_TYPE_EQUALS:
		xmmsc_coll_attribute_get (coll, "field",  &attr1);
		xmmsc_coll_attribute_get (coll, "value", &attr2);
		print_info ("%sEquals ('%s', '%s') for:", indent, attr1, attr2);
		coll_dump_list (xmmsv_coll_operands_get (coll), level + 1);
		break;

	case XMMS_COLLECTION_TYPE_HAS:
		xmmsc_coll_attribute_get (coll, "field",  &attr1);
		print_info ("%sHas ('%s') for:", indent, attr1);
		coll_dump_list (xmmsv_coll_operands_get (coll), level + 1);
		break;

	case XMMS_COLLECTION_TYPE_MATCH:
		xmmsc_coll_attribute_get (coll, "field",  &attr1);
		xmmsc_coll_attribute_get (coll, "value", &attr2);
		print_info ("%sMatch ('%s', '%s') for:", indent, attr1, attr2);
		coll_dump_list (xmmsv_coll_operands_get (coll), level + 1);
		break;

	case XMMS_COLLECTION_TYPE_SMALLER:
		xmmsc_coll_attribute_get (coll, "field",  &attr1);
		xmmsc_coll_attribute_get (coll, "value", &attr2);
		print_info ("%sSmaller ('%s', '%s') for:", indent, attr1, attr2);
		coll_dump_list (xmmsv_coll_operands_get (coll), level + 1);
		break;

	case XMMS_COLLECTION_TYPE_GREATER:
		xmmsc_coll_attribute_get (coll, "field",  &attr1);
		xmmsc_coll_attribute_get (coll, "value", &attr2);
		print_info ("%sGreater ('%s', '%s') for:", indent, attr1, attr2);
		coll_dump_list (xmmsv_coll_operands_get (coll), level + 1);
		break;

	case XMMS_COLLECTION_TYPE_IDLIST:
		idlist_str = coll_idlist_to_string (coll);
		print_info ("%sIdlist: %s", indent, idlist_str->str);
		g_string_free (idlist_str, TRUE);
		break;

	case XMMS_COLLECTION_TYPE_QUEUE:
		idlist_str = coll_idlist_to_string (coll);
		print_info ("%sQueue: %s", indent, idlist_str->str);
		g_string_free (idlist_str, TRUE);
		break;

	case XMMS_COLLECTION_TYPE_PARTYSHUFFLE:
		idlist_str = coll_idlist_to_string (coll);
		print_info ("%sParty Shuffle: %s from :", indent, idlist_str->str);
		g_string_free (idlist_str, TRUE);
		coll_dump_list (xmmsv_coll_operands_get (coll), level + 1);
		break;

	default:
		print_info ("%sUnknown Operator!", indent);
		break;
	}
}
