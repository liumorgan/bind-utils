/*
 * Copyright (C) 2005  Internet Systems Consortium, Inc. ("ISC")
 *
 * This code is derived from software contributed to ISC by
 * Berkeley Software Design, Inc.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC AND BERKELEY SOFTWARE DESIGN, INC.
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* $Id: dnsconf.h,v 1.2 2009/09/01 00:22:28 jinmei Exp $ */

#ifndef IRS_DNSCONF_H
#define IRS_DNSCONF_H 1

/*! \file
 *
 * \brief
 * The IRS dnsconf module parses an "advanced" configuration file related to
 * the DNS library, such as trusted keys for DNSSEC validation, and creates
 * the corresponding configuration objects for the DNS library modules.
 *
 * Notes:
 * This module is very experimental and the configuration syntax or library
 * interfaces may change in future versions.  Currently, only the
 * 'trusted-keys' statement is supported, whose syntax is the same as the
 * same name of statement for named.conf.
 */

#include <irs/types.h>

/*%
 * A compound structure storing DNS key information mainly for DNSSEC
 * validation.  A dns_key_t object will be created using the 'keyname' and
 * 'keydatabuf' members with the dst_key_fromdns() function.
 */
typedef struct irs_dnsconf_dnskey {
	dns_name_t				*keyname;
	isc_buffer_t				*keydatabuf;
	ISC_LINK(struct irs_dnsconf_dnskey)	link;
} irs_dnsconf_dnskey_t;

typedef ISC_LIST(irs_dnsconf_dnskey_t) irs_dnsconf_dnskeylist_t;

ISC_LANG_BEGINDECLS

isc_result_t
irs_dnsconf_load(isc_mem_t *mctx, const char *filename, irs_dnsconf_t **confp);
/*%<
 * Load the "advanced" DNS configuration file 'filename' in the "dns.conf"
 * format, and create a new irs_dnsconf_t object from the configuration.
 *
 * Requires:
 *
 *\li	'mctx' is a valid memory context.
 *
 *\li	'filename' != NULL
 *
 *\li	'confp' != NULL && '*confp' == NULL
 */

void
irs_dnsconf_destroy(irs_dnsconf_t **confp);
/*%<
 * Destroy the dnsconf object.
 *
 * Requires:
 *
 *\li	'*confp' is a valid dnsconf object.
 *
 * Ensures:
 *
 *\li	*confp == NULL
 */

irs_dnsconf_dnskeylist_t *
irs_dnsconf_gettrustedkeys(irs_dnsconf_t *conf);
/*%<
 * Return a list of key information stored in 'conf'.
 *
 * Requires:
 *
 *\li	'conf' is a valid dnsconf object.
 */

ISC_LANG_ENDDECLS

#endif /* IRS_DNSCONF_H */
