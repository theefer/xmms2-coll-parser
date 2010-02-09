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

#ifndef XM_COLL_PARSER_H
#define XM_COLL_PARSER_H

/* #include <unistd.h> */
#include <stddef.h>
#include "xmmsc/xmmsv.h"

xmmsv_t *
xm_coll_compile_pattern (const char *pattern);

int
xm_coll_check_pattern (const char *pattern, xmmsv_t **error);

#endif /* XM_COLL_PARSER_H */
