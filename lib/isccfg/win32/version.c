/*
 * Copyright (C) 1998-2001  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* $Id: version.c,v 1.1.12.1 2003/10/10 06:39:49 marka Exp $ */

#include <versions.h>

#include <isccfg/version.h>

LIBISCCFG_EXTERNAL_DATA const char cfg_version[] = VERSION;

LIBISCCFG_EXTERNAL_DATA const unsigned int cfg_libinterface = LIBINTERFACE;
LIBISCCFG_EXTERNAL_DATA const unsigned int cfg_librevision = LIBREVISION;
LIBISCCFG_EXTERNAL_DATA const unsigned int cfg_libage = LIBAGE;

