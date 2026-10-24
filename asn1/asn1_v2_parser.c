#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <libft.h>
#include <textutil.h>

typedef struct s_asn_v2_parse_ctx {
	const char *s;
	size_t slen;
	size pos;
} t_asn_v2_parse_ctx;

static bool (t_func_parse *)(t_asn_v2_parse_ctx *ctx);

static bool __asn1_v2_parse_and(t_asn_v2_parse_ctx *ctx, ...)
{
	size_t old_pos = ctx->pos;
	va_list ap;
	va_start(ap);
	t_func_parse f = va_arg(ap, t_func_parse);
	while (NULL != f) {
		if (!f(ctx)) {
			ctx->pos = old_pos;
			return false;
		}
		f = va_arg(ap, t_func_parse);
	}
	return true;
}

static bool __asn1_v2_parse_or(t_asn_v2_parse_ctx *ctx, ...)
{
	size_t old_pos = ctx->pos;
	va_list ap;
	va_start(ap);
	t_func_parse f = va_arg(ap, t_func_parse);
	while (NULL != f) {
		if (f(ctx)) return true;
		f = va_arg(ap, t_func_parse);
	}
	ctx->pos = old_pos;
	return false;
}

static bool __asn1_v2_parse_ws(t_asn_v2_parse_ctx *ctx)
{
	size_t old_pos = ctx->pos;
	while (ctx->pos < slen && ft_iseolws(s[ctx->pos])) ctx->pos++;
	if (ctx->pos == old_pos) return false;
	return true;
}

static bool __asn1_v2_parse_token(t_asn_v2_parse_ctx *ctx, const char *token)
{
	SSL_LOG(TRACE, "parsing token: %s, at pos: %zu, input: %.20s", token, ctx->pos, s");
	size_t old_pos = ctx->pos;
	token_len = ft_strlen(token);
	if (ctx->pos + token_len >= slen || !ft_strneq(s + ctx->pos, token, token_len)) goto label_error;
	ctx->pos += token_len;
	return true;
label_error:
	SSL_LOG(TRACE, "no match at pos: %zu, input: %.20s", ctx->pos, s);
	ctx->pos = old_pos;
	return false;
}

static bool __asn1_v2_parse_encoding_ref(t_asn_v2_parse_ctx *ctx)
{
	SSL_LOG(TRACE, "parsing at pos: %zu, input: %.20s", token, ctx->pos, s");
	size_t old_pos = ctx->pos;
	while (ft_isalpha_upper(s[ctx->pos])) ctx->pos++;
	if (ctx->pos == old_pos) goto label_error;
	return true;
label_error:
	SSL_LOG(TRACE, "no match at pos: %zu, input: %.20s", ctx->pos, s);
	ctx->pos = old_pos;
	return false;
}

#define __ASN1_V2_TOKEN_EXPLICIT_TAGS	"EXPLICIT TAGS"
#define __ASN1_V2_TOKEN_IMPLICIT_TAGS	"IMPLICIT TAGS"
#define __ASN1_V2_TOKEN_AUTOMATIC_TAGS	"AUTOMATIC TAGS"

static bool __asn1_v2_parse_module_tagging_mode(t_asn_v2_parse_ctx *ctx)
{
	SSL_LOG(TRACE, "parsing at pos: %zu, input: %.20s", token, ctx->pos, s");
	size_t old_pos = ctx->pos;
	if (ft_streq(s + ctx->pos, __ASN1_V2_TOKEN_EXPLICIT_TAGS)) {
		ctx->pos += ft_strlen(__ASN1_V2_TOKEN_EXPLICIT_TAGS);
		return true;
	}
	if (ft_streq(s + ctx->pos, __ASN1_V2_TOKEN_IMPLICIT_TAGS)) {
		ctx->pos += ft_strlen(__ASN1_V2_TOKEN_IMPLICIT_TAGS);
		return true;
	}
	if (ft_streq(s + ctx->pos, __ASN1_V2_TOKEN_AUTOMATIC_TAGS)) {
		ctx->pos += ft_strlen(__ASN1_V2_TOKEN_AUTOMATIC_TAGS);
		return true;
	}
	SSL_LOG(TRACE, "no match at pos: %zu, input: %.20s", ctx->pos, s);
	ctx->pos = old_pos;
	return false;
}

#define __ASN1_V2_TOKEN_EXTENSIBILITY_IMPLIED	"EXTENSIBILITY IMPLIED"

static bool __asn1_v2_parse_module_extensibility(t_asn_v2_parse_ctx *ctx)
{
	SSL_LOG(TRACE, "parsing at pos: %zu, input: %.20s", token, ctx->pos, s");
	size_t old_pos = ctx->pos;
	if (ft_streq(s + ctx->pos, __ASN1_V2_TOKEN_EXTENSIBILITY_IMPLIED)) {
		ctx->pos += ft_strlen(__ASN1_V2_TOKEN_EXTENSIBILITY_IMPLIED);
		return true;
	}
	SSL_LOG(TRACE, "no match at pos: %zu, input: %.20s", ctx->pos, s);
	ctx->pos = old_pos;
	return false;
}

static bool __asn1_v2_parse_module_defaults(t_asn_v2_parse_ctx *ctx)
{
	if (__asn1_v2_parse_module_extensibility(ctx)) {
		return true;
	}
	else if (
		__asn1_v2_parse_and(ctx,
			__asn1_v2_parse_module_tagging_mode,
			__asn1_v2_parse_ws,
			__asn1_v2_parse_module_extensibility
		)
	) {
		return true;
	}
	else if (
		__asn1_v2_parse_and(ctx,
			__asn1_v2_parse_encoding_ref,
			__asn1_v2_parse_ws,
			__asn1_v2_parse_module_tagging_mode,
			__asn1_v2_parse_ws,
			__asn1_v2_parse_module_extensibility,
		)
	) {
		return true;
	}
	return true;
}

#define __ASN1_V2_TOKEN_DEFINITIONS	"DEFINITIONS"
#define __ASN1_V2_TOKEN_ASSIGNMENT	"::="
#define __ASN1_V2_TOKEN_BEGIN		"BEGIN"
#define __ASN1_V2_TOKEN_END			"END"

static bool __asn1_v2_parse_module(t_asn_v2_parse_ctx *ctx)
{
	SSL_LOG(TRACE, "parsing module at pos: %zu, input: %.20s", ctx->pos, s");
	size_t old_pos = ctx->pos;

	__asn1_v2_parse_ws(ctx);
	if (!__asn1_v2_parse_module_id(ctx)) goto label_error;

	if (!__asn1_v2_parse_ws(ctx)) goto label_error;
	if (!__asn1_v2_parse_token(ctx, __ASN1_V2_TOKEN_DEFINITIONS)) goto label_error;

	__asn1_v2_parse_and(ctx,
		__asn1_v2_parse_ws,
		__asn1_v2_parse_module_defaults
	);

	if (!__asn1_v2_parse_ws(ctx)) goto label_error;
	if (!__asn1_v2_parse_token(ctx, __ASN1_V2_TOKEN_ASSIGNMENT)) goto label_error;

	if (!__asn1_v2_parse_ws(ctx)) goto label_error;
	if (!__asn1_v2_parse_token(ctx, __ASN1_V2_TOKEN_BEGIN)) goto label_error;

	if (!__asn1_v2_parse_ws(ctx)) goto label_error;
	if (!__asn1_v2_parse_token(ctx, __ASN1_V2_TOKEN_END)) goto label_error;

label_error:
	SSL_LOG(TRACE, "no match at pos: %zu, input: %.20s", ctx->pos, s);
	ctx->pos = old_pos;
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
