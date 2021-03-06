/* $OpenBSD: x509_internal.h,v 1.1 2020/09/11 18:34:29 beck Exp $ */
/*
 * Copyright (c) 2020 Bob Beck <beck@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef HEADER_X509_INTERNAL_H
#define HEADER_X509_INTERNAL_H

/* Internal use only, not public API */
#include <netinet/in.h>

/*
 * Limit the number of names and constraints we will check in a chain
 * to avoid a hostile input DOS
 */
#define X509_VERIFY_MAX_CHAIN_NAMES		512
#define X509_VERIFY_MAX_CHAIN_CONSTRAINTS	512

/*
 * Hold the parsed and validated result of names from a certificate.
 * these typically come from a GENERALNAME, but we store the parsed
 * and validated results, not the ASN1 bytes.
 */
struct x509_constraints_name {
	int type;			/* GEN_* types from GENERAL_NAME */
	char *name;			/* Name to check */
	char *local;			/* holds the local part of GEN_EMAIL */
	uint8_t *der;			/* DER encoded value or NULL*/
	size_t der_len;
	int af;				/* INET and INET6 are supported */
	uint8_t address[32];		/* Must hold ipv6 + mask */
};

struct x509_constraints_names {
	struct x509_constraints_name **names;
	size_t names_len;
	size_t names_count;
};

struct x509_verify_chain {
	STACK_OF(X509) *certs;		/* Kept in chain order, includes leaf */
	struct x509_constraints_names *names;	/* All names from all certs */
};

__BEGIN_HIDDEN_DECLS

void x509_constraints_name_clear(struct x509_constraints_name *name);
int x509_constraints_names_add(struct x509_constraints_names *names,
    struct x509_constraints_name *name);
struct x509_constraints_names *x509_constraints_names_dup(
    struct x509_constraints_names *names);
void x509_constraints_names_clear(struct x509_constraints_names *names);
struct x509_constraints_names *x509_constraints_names_new(void);
void x509_constraints_names_free(struct x509_constraints_names *names);
int x509_constraints_valid_host(uint8_t *name, size_t len);
int x509_constraints_valid_sandns(uint8_t *name, size_t len);
int x509_constraints_domain(char *domain, size_t dlen, char *constraint,
    size_t len);
int x509_constraints_parse_mailbox(uint8_t *candidate, size_t len,
    struct x509_constraints_name *name);
int x509_constraints_valid_domain_constraint(uint8_t *constraint,
    size_t len);
int x509_constraints_uri_host(uint8_t *uri, size_t len, char **hostp);
int x509_constraints_uri(uint8_t *uri, size_t ulen, uint8_t *constraint,
    size_t len, int *error);
int x509_constraints_extract_names(struct x509_constraints_names *names,
    X509 *cert, int include_cn, int *error);
int x509_constraints_extract_constraints(X509 *cert,
    struct x509_constraints_names *permitted,
    struct x509_constraints_names *excluded, int *error);
int x509_constraints_check(struct x509_constraints_names *names,
    struct x509_constraints_names *permitted,
    struct x509_constraints_names *excluded, int *error);
int x509_constraints_chain(STACK_OF(X509) *chain, int *error,
    int *depth);

__END_HIDDEN_DECLS

#endif
