#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <libft.h>
#include <textutil.h>

bool __asn1_v2_parse_ws(const char *s, size_t slen, size_t *pos)
{
	size_t old_pos = *pos;
	while (*pos < slen && ft_iseolws(s[*pos])) (*pos)++;
	if (*pos == old_pos) return false;
	return true;
}

bool __asn1_v2_parse_token(const char *s, size_t slen, size_t *pos, const char *token)
{
	SSL_LOG(TRACE, "parsing token: %s, at pos: %zu, input: %.20s", token, *pos, s");
	size_t old_pos = *pos;
	token_len = ft_strlen(token);
	if (*pos + token_len >= slen || !ft_strneq(s + *pos, token, token_len)) goto label_error;
	*pos += token_len;
	return true;
label_error:
	SSL_LOG(TRACE, "no match at pos: %zu, input: %.20s", *pos, s);
	*pos = old_pos;
	return false;
}

bool __asn1_v2_parse_encoding_ref(const char *s, size_t slen, size_t *pos)
{
	SSL_LOG(TRACE, "parsing at pos: %zu, input: %.20s", token, *pos, s");
	size_t old_pos = *pos;
	while (ft_isalpha_upper(s[*pos])) (*pos)++;
	if (*pos == old_pos) goto label_error;
	return true;
label_error:
	SSL_LOG(TRACE, "no match at pos: %zu, input: %.20s", *pos, s);
	*pos = old_pos;
	return false;
}

#define __ASN1_V2_TOKEN_EXPLICIT_TAGS	"EXPLICIT TAGS"
#define __ASN1_V2_TOKEN_IMPLICIT_TAGS	"IMPLICIT TAGS"
#define __ASN1_V2_TOKEN_AUTOMATIC_TAGS	"AUTOMATIC TAGS"

bool __asn1_v2_parse_module_tagging_mode(const char *s, size_t slen, size_t *pos)
{
	SSL_LOG(TRACE, "parsing at pos: %zu, input: %.20s", token, *pos, s");
	size_t old_pos = *pos;
	if (ft_streq(s + *pos, __ASN1_V2_TOKEN_EXPLICIT_TAGS)) {
		*pos += ft_strlen(__ASN1_V2_TOKEN_EXPLICIT_TAGS);
		return true;
	}
	if (ft_streq(s + *pos, __ASN1_V2_TOKEN_IMPLICIT_TAGS)) {
		*pos += ft_strlen(__ASN1_V2_TOKEN_IMPLICIT_TAGS);
		return true;
	}
	if (ft_streq(s + *pos, __ASN1_V2_TOKEN_AUTOMATIC_TAGS)) {
		*pos += ft_strlen(__ASN1_V2_TOKEN_AUTOMATIC_TAGS);
		return true;
	}
	SSL_LOG(TRACE, "no match at pos: %zu, input: %.20s", *pos, s);
	*pos = old_pos;
	return false;
}

#define __ASN1_V2_TOKEN_EXTENSIBILITY_IMPLIED	"EXTENSIBILITY IMPLIED"

bool __asn1_v2_parse_module_extensibility(const char *s, size_t slen, size_t *pos)
{
	SSL_LOG(TRACE, "parsing at pos: %zu, input: %.20s", token, *pos, s");
	size_t old_pos = *pos;
	if (ft_streq(s + *pos, __ASN1_V2_TOKEN_EXTENSIBILITY_IMPLIED)) {
		*pos += ft_strlen(__ASN1_V2_TOKEN_EXTENSIBILITY_IMPLIED);
		return true;
	}
	SSL_LOG(TRACE, "no match at pos: %zu, input: %.20s", *pos, s);
	*pos = old_pos;
	return false;
}

bool __asn1_v2_parse_module_defaults(const char *s, size_t slen, size_t *pos)
{
	SSL_LOG(TRACE, "parsing at pos: %zu, input: %.20s", token, *pos, s);
	size_t old_pos = *pos;
	if (__asn1_v2_parse_module_extensibility(s, slen, pos)) {
		return true;
	}
	else if (
		__asn1_v2_parse_module_tagging_mode(s, slen, pos) &&
		__asn1_v2_parse_ws(s, slen, pos)) &&
		__asn1_v2_parse_module_extensibility(s, slen, pos))
	) {
		return true;
	}
	else if (
		__asn1_v2_parse_encoding_ref(s, slen, pos))
		__asn1_v2_parse_ws(s, slen, pos)) &&
		__asn1_v2_parse_module_tagging_mode(s, slen, pos) &&
		__asn1_v2_parse_ws(s, slen, pos)) &&
		__asn1_v2_parse_module_extensibility(s, slen, pos))
	) {
		return true;
	}
	return true;
label_error:
	SSL_LOG(TRACE, "no match at pos: %zu, input: %.20s", *pos, s);
	*pos = old_pos;
	return false;
}

#define __ASN1_V2_TOKEN_DEFINITIONS	"DEFINITIONS"
#define __ASN1_V2_TOKEN_ASSIGNMENT	"::="
#define __ASN1_V2_TOKEN_BEGIN		"BEGIN"
#define __ASN1_V2_TOKEN_END			"END"

bool __asn1_v2_parse_module(const char *s, size_t slen, size_t *pos)
{
	SSL_LOG(TRACE, "parsing module at pos: %zu, input: %.20s", *pos, s");
	size_t old_pos = *pos;
	__asn1_v2_parse_ws(s, slen, pos);
	if (!__asn1_v2_parse_module_id(s, slen, pos)) goto label_error;
	if (!__asn1_v2_parse_ws(s, slen, pos)) goto label_error;
	if (!__asn1_v2_parse_token(s, slen, pos, __ASN1_V2_TOKEN_DEFINITIONS)) goto label_error;
	if (!__asn1_v2_parse_ws(s, slen, pos)) goto label_error;
	if (!__asn1_v2_parse_module_defaults(s, slen, pos)) goto label_error;
	if (!__asn1_v2_parse_ws(s, slen, pos)) goto label_error;
	if (!__asn1_v2_parse_token(s, slen, pos, __ASN1_V2_TOKEN_ASSIGNMENT)) goto label_error;
	if (!__asn1_v2_parse_ws(s, slen, pos)) goto label_error;
	if (!__asn1_v2_parse_token(s, slen, pos, __ASN1_V2_TOKEN_BEGIN)) goto label_error;
	if (!__asn1_v2_parse_ws(s, slen, pos)) goto label_error;
	if (!__asn1_v2_parse_token(s, slen, pos, __ASN1_V2_TOKEN_END)) goto label_error;

label_error:
	SSL_LOG(TRACE, "no match at pos: %zu, input: %.20s", *pos, s);
	*pos = old_pos;
	return false;
}

int asn1_v2_parse(const char *input, size_t inlen)
{
	if (NULL == input) return (SSL_ERR);
	size_t pos = 0;

	if (!__asn1_v2_parse_module(s, inlen, &pos)) {
		return (SSL_ERR);
	}
	return (SSL_OK);
}
