/*
 * Copyright (C) 1999, 2000  Internet Software Consortium.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/* $Id: nxt_30.c,v 1.25 2000/04/07 03:54:27 explorer Exp $ */

/* reviewed: Wed Mar 15 18:21:15 PST 2000 by brister */

/* RFC 2065 */

#ifndef RDATA_GENERIC_NXT_30_C
#define RDATA_GENERIC_NXT_30_C

#define RRTYPE_NXT_ATTRIBUTES (DNS_RDATATYPEATTR_DNSSEC | DNS_RDATATYPEATTR_SINGLETON)

static inline isc_result_t
fromtext_nxt(dns_rdataclass_t rdclass, dns_rdatatype_t type,
	     isc_lex_t *lexer, dns_name_t *origin,
	     isc_boolean_t downcase, isc_buffer_t *target)
{
	isc_token_t token;
	dns_name_t name;
	isc_buffer_t buffer;
	char *e;
	unsigned char bm[8*1024]; /* 64k bits */
	dns_rdatatype_t covered;
	dns_rdatatype_t maxcovered = 0;
	isc_boolean_t first = ISC_TRUE;
	long n;

	REQUIRE(type == 30);

	UNUSED(rdclass);
	
	/* next domain */
	RETERR(gettoken(lexer, &token, isc_tokentype_string, ISC_FALSE));
	dns_name_init(&name, NULL);
	buffer_fromregion(&buffer, &token.value.as_region,
			  ISC_BUFFERTYPE_TEXT);
	origin = (origin != NULL) ? origin : dns_rootname;
	RETERR(dns_name_fromtext(&name, &buffer, origin, downcase, target));

	memset(bm, 0, sizeof bm);
	do {
		RETERR(gettoken(lexer, &token, isc_tokentype_string,
				  ISC_TRUE));
		if (token.type != isc_tokentype_string)
			break;
		n = strtol(token.value.as_pointer, &e, 10);
		if (e != token.value.as_pointer && *e == '\0') {
			covered = (dns_rdatatype_t)n;
		} else if (dns_rdatatype_fromtext(&covered, 
				&token.value.as_textregion) == DNS_R_UNKNOWN)
			return (DNS_R_UNKNOWN);
		/*
		 * NXT is only specified for types 1..127.
		 */
		if (covered < 1 || covered > 127)
			return (DNS_R_RANGE);
		if (first || covered > maxcovered)
			maxcovered = covered;
		first = ISC_FALSE;
		bm[covered/8] |= (0x80>>(covered%8));
	} while (1);
	isc_lex_ungettoken(lexer, &token);
	if (first) 
		return (ISC_R_SUCCESS);
	n = (maxcovered + 8) / 8;
	return (mem_tobuffer(target, bm, n));
}

static inline isc_result_t
totext_nxt(dns_rdata_t *rdata, dns_rdata_textctx_t *tctx, 
	   isc_buffer_t *target) 
{
	isc_region_t sr;
	char buf[sizeof "65535"];
	unsigned int i, j;
	dns_name_t name;
	dns_name_t prefix;
	isc_result_t result;
	isc_boolean_t sub;

	REQUIRE(rdata->type == 30);

	dns_name_init(&name, NULL);
	dns_name_init(&prefix, NULL);
	dns_rdata_toregion(rdata, &sr);
	dns_name_fromregion(&name, &sr);
	isc_region_consume(&sr, name_length(&name));
	sub = name_prefix(&name, tctx->origin, &prefix);
	RETERR(dns_name_totext(&prefix, sub, target));

	RETERR(str_totext(" ( ", target));

	for (i = 0 ; i < sr.length ; i++) {
		if (sr.base[i] != 0)
			for (j = 0; j < 8; j++)
				if ((sr.base[i] & (0x80 >> j)) != 0) {
					result = dns_rdatatype_totext(
						  (dns_rdatatype_t)(i * 8 + j),
						  target);
					if (result == ISC_R_SUCCESS) {
						RETERR(str_totext(" ",
								  target));
						continue;
					}
					if (result != DNS_R_UNKNOWN)
						return (result);
					sprintf(buf, "%u", i * 8 + j);
					RETERR(str_totext(" ", target));
					RETERR(str_totext(buf, target));
				}
	}
	return (str_totext(")", target));
}

static inline isc_result_t
fromwire_nxt(dns_rdataclass_t rdclass, dns_rdatatype_t type,
	     isc_buffer_t *source, dns_decompress_t *dctx,
	     isc_boolean_t downcase, isc_buffer_t *target)
{
	isc_region_t sr;
	dns_name_t name;

	REQUIRE(type == 30);

	UNUSED(rdclass);

	if (dns_decompress_edns(dctx) >= 1 || !dns_decompress_strict(dctx))
		dns_decompress_setmethods(dctx, DNS_COMPRESS_ALL);
	else
		dns_decompress_setmethods(dctx, DNS_COMPRESS_NONE);

	dns_name_init(&name, NULL);
	RETERR(dns_name_fromwire(&name, source, dctx, downcase, target));
	
	isc_buffer_active(source, &sr);
	/* XXXRTH  Enforce RFC 2535 length rules if bit 0 is not set. */
	if (sr.length > 8 * 1024)
		return (DNS_R_EXTRADATA);
	RETERR(mem_tobuffer(target, sr.base, sr.length));
	isc_buffer_forward(source, sr.length);
	return (ISC_R_SUCCESS);
}

static inline isc_result_t
towire_nxt(dns_rdata_t *rdata, dns_compress_t *cctx, isc_buffer_t *target)
{
	isc_region_t sr;
	dns_name_t name;

	REQUIRE(rdata->type == 30);

	if (dns_compress_getedns(cctx) >= 1)
		dns_compress_setmethods(cctx, DNS_COMPRESS_ALL);
	else
		dns_compress_setmethods(cctx, DNS_COMPRESS_NONE);

	dns_name_init(&name, NULL);
	dns_rdata_toregion(rdata, &sr);
	dns_name_fromregion(&name, &sr);
	isc_region_consume(&sr, name_length(&name));
	RETERR(dns_name_towire(&name, cctx, target));

	return (mem_tobuffer(target, sr.base, sr.length));
}

static inline int
compare_nxt(dns_rdata_t *rdata1, dns_rdata_t *rdata2) {
	isc_region_t r1;
	isc_region_t r2;
	dns_name_t name1;
	dns_name_t name2;
	int order;

	REQUIRE(rdata1->type == rdata2->type);
	REQUIRE(rdata1->rdclass == rdata2->rdclass);
	REQUIRE(rdata1->type == 30);

	dns_name_init(&name1, NULL);
	dns_name_init(&name2, NULL);
	dns_rdata_toregion(rdata1, &r1);
	dns_rdata_toregion(rdata2, &r2);
	dns_name_fromregion(&name1, &r1);
	dns_name_fromregion(&name2, &r2);
	order = dns_name_rdatacompare(&name1, &name2);
	if (order != 0)
		return (order);

	return (compare_region(&r1, &r2));
}

static inline isc_result_t
fromstruct_nxt(dns_rdataclass_t rdclass, dns_rdatatype_t type, void *source,
	       isc_buffer_t *target)
{

	REQUIRE(type == 30);

	UNUSED(rdclass);

	UNUSED(source);
	UNUSED(target);

	return (ISC_R_NOTIMPLEMENTED);
}

static inline isc_result_t
tostruct_nxt(dns_rdata_t *rdata, void *target, isc_mem_t *mctx) {

	REQUIRE(rdata->type == 30);

	UNUSED(target);
	UNUSED(mctx);

	return (ISC_R_NOTIMPLEMENTED);
}

static inline void
freestruct_nxt(void *source) {
	dns_rdata_nxt_t *nxt = source;

	REQUIRE(source != NULL);
	REQUIRE(nxt->common.rdtype == 30);
	REQUIRE(ISC_FALSE);
}

static inline isc_result_t
additionaldata_nxt(dns_rdata_t *rdata, dns_additionaldatafunc_t add,
		   void *arg)
{
	REQUIRE(rdata->type == 30);

	(void)add;
	(void)arg;

	return (ISC_R_SUCCESS);
}

static inline isc_result_t
digest_nxt(dns_rdata_t *rdata, dns_digestfunc_t digest, void *arg) {
	isc_region_t r;
	dns_name_t name;
	isc_result_t result;

	REQUIRE(rdata->type == 30);

	dns_rdata_toregion(rdata, &r);
	dns_name_init(&name, NULL);
	dns_name_fromregion(&name, &r);
	result = dns_name_digest(&name, digest, arg);
	if (result != ISC_R_SUCCESS)
		return (result);
	isc_region_consume(&r, name_length(&name));

	return ((digest)(arg, &r));
}

#endif	/* RDATA_GENERIC_NXT_30_C */
