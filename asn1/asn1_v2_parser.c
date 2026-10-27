#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <libft.h>
#include <textutil.h>

typedef enum e_asn_v2_token_kind {
	ASN_V2_TOKEN_KIND_NONE = 0,
	ASN_V2_TOKEN_KIND_COMMENT,
	ASN_V2_TOKEN_KIND_BSTRING,
	ASN_V2_TOKEN_KIND_HSTRING,
	ASN_V2_TOKEN_KIND_CSTRING,
	ASN_V2_TOKEN_KIND_NUMBER,
	ASN_V2_TOKEN_KIND_REAL_NUMBER,
	ASN_V2_TOKEN_KIND_ASSIGNMENT,
	ASN_V2_TOKEN_KIND_RANGE,
	ASN_V2_TOKEN_KIND_ELLIPSIS,
	ASN_V2_TOKEN_KIND_LVER_BRACKET,
	ASN_V2_TOKEN_KIND_RVER_BRACKET,
	ASN_V2_TOKEN_KIND_KEYWORD,
	ASN_V2_TOKEN_KIND_SINGLE_CHAR,
	ASN_V2_TOKEN_KIND_NAME,
} t_asn_v2_token_kind;

static const char *__asn1_v2_get_token_kind_name(t_asn_v2_token_kind kind)
{
	switch (kind) {
	case ASN_V2_TOKEN_KIND_NONE: 			return "NONE";
	case ASN_V2_TOKEN_KIND_COMMENT: 		return "COMMENT";
	case ASN_V2_TOKEN_KIND_BSTRING: 		return "BSTRING";
	case ASN_V2_TOKEN_KIND_HSTRING: 		return "HSTRING";
	case ASN_V2_TOKEN_KIND_CSTRING: 		return "CSTRING";
	case ASN_V2_TOKEN_KIND_NUMBER: 			return "NUMBER";
	case ASN_V2_TOKEN_KIND_REAL_NUMBER:		return "REAL_NUMBER";
	case ASN_V2_TOKEN_KIND_ASSIGNMENT: 		return "ASSIGNMENT";
	case ASN_V2_TOKEN_KIND_RANGE: 			return "RANGE";
	case ASN_V2_TOKEN_KIND_ELLIPSIS: 		return "ELLIPSIS";
	case ASN_V2_TOKEN_KIND_LVER_BRACKET:	return "LVER_BRACKET";
	case ASN_V2_TOKEN_KIND_RVER_BRACKET:	return "RVER_BRACKET";
	case ASN_V2_TOKEN_KIND_KEYWORD: 		return "KEYWORD";
	case ASN_V2_TOKEN_KIND_SINGLE_CHAR:		return "SINGLE_CHAR";
	case ASN_V2_TOKEN_KIND_NAME: 			return "NAME";
	default:								return "unknown";
	}
}

typedef struct s_asn_v2_token {
	t_asn_v2_token_kind kind;
	const char *s;
	size_t slen;
	size_t start;
	size_t end;
	bool done;
} t_asn_v2_token;

// typedef bool (*t_func_parse)(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
//
// typedef enum s_asn_v2_parser_op {
// 	PARSER_OP_AND,
// 	PARSER_OP_OR,
// } t_asn_v2_parser_op;
//
// typedef struct s_asn_v2_parser_node {
// 	t_node_v2 base;
// 	t_asn_v2_parser_op op;
// 	t_list funcs;
// 	union {
// 		char *cstring;
// 		char chr;
// 	} data;
// } t_asn_v2_parser_node;

typedef struct s_asn_v2_parser_ctx {
	t_asn_v2_token *tokens;
	size_t token_idx;
	t_list parsers;
} t_asn_v2_parser_ctx;

const char *RESERVED_KEYWORDS[] = {
	"ABSENT", "ENCODED", "INTERSECTION", "SEQUENCE", "ABSTRACT-SYNTAX", "ENCODING-CONTROL", "ISO646String",
	"SET", "ALL", "END", "MAX", "SETTINGS", "APPLICATION", "ENUMERATED", "MIN", "SIZE",
	"AUTOMATIC", "EXCEPT", "MINUS-INFINITY", "STRING", "BEGIN", "EXPLICIT", "NOT-A-NUMBER", "SYNTAX",
	"BIT", "EXPORTS", "NULL", "T61String", "BMPString", "EXTENSIBILITY", "NumericString", "TAGS",
	"BOOLEAN", "EXTERNAL", "OBJECT", "TeletexString", "BY", "FALSE", "ObjectDescriptor", "TIME",
	"CHARACTER", "FROM", "OCTET", "TIME-OF-DAY", "CHOICE", "GeneralizedTime", "OF", "TRUE",
	"CLASS", "GeneralString", "OID-IRI", "TYPE-IDENTIFIER", "COMPONENT", "GraphicString", "OPTIONAL", "UNION",
	"COMPONENTS", "IA5String", "PATTERN", "UNIQUE", "CONSTRAINED", "IDENTIFIER", "PDV", "UNIVERSAL",
	"CONTAINING", "IMPLICIT", "PLUS-INFINITY", "UniversalString", "DATE", "IMPLIED", "PRESENT", "UTCTime",
	"DATE-TIME", "IMPORTS", "PrintableString", "UTF8String", "DEFAULT", "INCLUDES", "PRIVATE", "VideotexString",
	"DEFINITIONS", "INSTANCE", "REAL", "VisibleString", "DURATION", "INSTRUCTIONS", "RELATIVE-OID", "WITH",
	"EMBEDDED", "INTEGER", "RELATIVE-OID-IRI",
};
const size_t RESERVED_KEYWORD_COUNT = sizeof(RESERVED_KEYWORDS)/sizeof(*RESERVED_KEYWORDS);

#define ASN_V2_LEX_ELLIPSIS		"..."
#define ASN_V2_LEX_ASSIGNMENT	"::="
#define ASN_V2_LEX_RANGE		".."
#define ASN_V2_LEX_LVER_BRACKET	"[["
#define ASN_V2_LEX_RVER_BRACKET	"]]"

const char SINGLE_CHAR_LEXES[] = {
	'{', '}', '<', '>', ',', '.', '/', '(', ')', '[', ']', '-', ':', '=', '"', '\'', ';', '@', '|', '!', '^'
};
const size_t SINGLE_CHAR_LEX_COUNT = sizeof(SINGLE_CHAR_LEXES)/sizeof(*SINGLE_CHAR_LEXES);

static bool __asn1_v2_token_expect_bstring(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_hstring(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_cstring(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_number(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_real_number(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_keyword(t_asn_v2_token *token, const char *kw);
static bool __asn1_v2_token_expect_upper_name(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_lower_name(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_upper_first_name(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_lower_first_name(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_assignment(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_range(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_ellipsis(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_lver_bracket(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_rver_bracket(t_asn_v2_token *token);
static bool __asn1_v2_token_expect_char(t_asn_v2_token *token, char c);
static bool __asn1_v2_parse_encoding_ref_default(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_tag_default(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_extension_default(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_id(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_name_form(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_definitive_number_form(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_definitive_name_and_number_form(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_definitive_oid_component(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_definitive_oid_component_list(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_definitive_oid(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_iri_value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_definitive_oid_and_iri(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_definitive_id(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_module_id(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_value_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_type_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_object_class_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_object_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_object_set_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_parametrized_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_symbol(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_symbol_list(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_symbols_exported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_exports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_oid_value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_assigned_id(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_module_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_global_module_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_selection_option(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_symbols_from_module(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_symbols_from_module_list(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_symbols_imported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_imports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_external_type_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_external_value_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_xml_asn1_type_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_non_parametrized_type_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_parameterized_type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_parameterized_value_set_type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_defined_type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_parameterized_value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_defined_value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_type_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_value_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_xml_typed_value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_xml_value_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_element_set_specs(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_value_set(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_value_set_type_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_object_class_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_object_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_object_set_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_parameterized_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_assignment_list(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_token_parse_module_body(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);
static bool __asn1_v2_parse_module(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token);

bool __asn1_v2_is_bound(char c)
{
	if (c == '\0') return true;
	if (ft_iseolws(c)) return true;
	for (size_t i = 0; i < SINGLE_CHAR_LEX_COUNT; i++) {
		if (c == SINGLE_CHAR_LEXES[i]) return true;
	}
	return false;
}

bool __asn1_v2_token_next(t_asn_v2_token *token)
{
	assert(NULL != token);
	if (token->end >= token->slen) return false;

	t_asn_v2_token t = { .s = token->s, .slen = token->slen, .start = token->end, };

	while (t.start < t.slen && ft_iseolws(t.s[t.start])) t.start++;
	if (t.start == t.slen) {
		token->kind = ASN_V2_TOKEN_KIND_NONE;
		token->done = true;
		return false;
	}

	const char *s = t.s + t.start;
	size_t slen = t.slen - t.start;

	if (s[0] == '-' && s[1] == '-') {
		for (size_t i = 2; i+1 < slen; i++) {
			if (ft_iseol(s[i])) {
				t.kind = ASN_V2_TOKEN_KIND_COMMENT;
				t.end = t.start + i;
				goto label_ok;
			}
			if (s[i] == '-' && i < slen && s[i+1] == '-') {
				t.kind = ASN_V2_TOKEN_KIND_COMMENT;
				t.end = t.start + i+2;
				goto label_ok;
			}
		}
	}
	if (s[0] == '/' && s[1] == '*') {
		for (size_t i = 2; i < slen; i++) {
			if (s[i] == '*' && i+1 < slen && s[i+1] == '/') {
				t.kind = ASN_V2_TOKEN_KIND_COMMENT;
				t.end = t.start + i+2;
				goto label_ok;
			}
		}
	}

	if (s[0] == '"') {
		for (size_t i = 1; i < slen; i++) {
			if (s[i] == '"' && i+1 < slen && s[i+1] != '"') {
				t.kind = ASN_V2_TOKEN_KIND_CSTRING;
				t.end = t.start + i;
				goto label_ok;
			}
		}
	}
	if (s[0] == '\'') {
		for (size_t i = 1; i < slen; i++) {
			if (s[i] == '\'' && i+1 < slen) {
				if (s[i+1] == 'B') {
					t.kind = ASN_V2_TOKEN_KIND_BSTRING;
					t.end = t.start + i;
					goto label_ok;
				}
				else if (s[i+1] == 'H') {
					t.kind = ASN_V2_TOKEN_KIND_HSTRING;
					t.end = t.start + i;
					goto label_ok;
				}
			}
		}
	}

	if (ft_isdigit(s[0])) {
		t.kind = ASN_V2_TOKEN_KIND_NUMBER;
		for (size_t i = 1; i < slen; i++) {
			if (!ft_isdigit(s[i])) {
				if (s[i] == '.') {
					t.kind = ASN_V2_TOKEN_KIND_REAL_NUMBER;
					i++;
					while (i < slen && ft_isdigit(s[i])) i++;
				}
				if (i < slen && (s[i] == 'e' || s[i] == 'E')) {
					t.kind = ASN_V2_TOKEN_KIND_REAL_NUMBER;
					if (i+2 < slen && s[i+1] == '-' && ft_isdigit(s[i+2])) {
						i += 2;
						while (i < slen && ft_isdigit(s[i])) i++;
					}
					else if (i+1 < slen && ft_isdigit(s[i+1])) {
						i++;
						while (i < slen && ft_isdigit(s[i])) i++;
					}
				}
				if (__asn1_v2_is_bound(s[i])) {
					t.end = t.start + i;
					goto label_ok;
				}
				break;
			}
		}
	}

	const char *lex = ASN_V2_LEX_ELLIPSIS;
	if (ft_strneq(s, lex, ft_strlen(lex))) {
		t.kind = ASN_V2_TOKEN_KIND_ELLIPSIS;
		t.end = t.start + ft_strlen(lex);
		goto label_ok;
	}
	lex = ASN_V2_LEX_ASSIGNMENT;
	if (ft_strneq(s, lex, ft_strlen(lex))) {
		t.kind = ASN_V2_TOKEN_KIND_ASSIGNMENT;
		t.end = t.start + ft_strlen(lex);
		goto label_ok;
	}
	lex = ASN_V2_LEX_RANGE;
	if (ft_strneq(s, lex, ft_strlen(lex))) {
		t.kind = ASN_V2_TOKEN_KIND_RANGE;
		t.end = t.start + ft_strlen(lex);
		goto label_ok;
	}
	lex = ASN_V2_LEX_LVER_BRACKET;
	if (ft_strneq(s, lex, ft_strlen(lex))) {
		t.kind = ASN_V2_TOKEN_KIND_LVER_BRACKET;
		t.end = t.start + ft_strlen(lex);
		goto label_ok;
	}
	lex = ASN_V2_LEX_RVER_BRACKET;
	if (ft_strneq(s, lex, ft_strlen(lex))) {
		t.kind = ASN_V2_TOKEN_KIND_RVER_BRACKET;
		t.end = t.start + ft_strlen(lex);
		goto label_ok;
	}

	for (size_t i = 0; i < RESERVED_KEYWORD_COUNT; i++) {
		size_t kwlen = ft_strlen(RESERVED_KEYWORDS[i]);
		if (ft_strneq(s, RESERVED_KEYWORDS[i], kwlen)) {
			t.kind = ASN_V2_TOKEN_KIND_KEYWORD;
			t.end = t.start + kwlen;
			goto label_ok;
		}
	}

	for (size_t i = 0; i < SINGLE_CHAR_LEX_COUNT; i++) {
		if (*s == SINGLE_CHAR_LEXES[i]) {
			t.kind = ASN_V2_TOKEN_KIND_SINGLE_CHAR;
			t.end = t.start + 1;
			goto label_ok;
		}
	}

	if (ft_isalpha(s[0])) {
		bool is_prev_hyphen = false;
		for (size_t i = 1; i < slen; i++) {
			if (!ft_isalnum_or_hyphen(s[i])) {
				if (is_prev_hyphen) break;
				if (__asn1_v2_is_bound(s[i])) {
					t.kind = ASN_V2_TOKEN_KIND_NAME;
					t.end = t.start + i;
					goto label_ok;
				}
				break;
			}
			is_prev_hyphen = (s[i] == '-');
			if (s[i] == '-' && is_prev_hyphen) break;
		}
	}

	SSL_LOG(TRACE, "no match at pos: %zu", token->end);
	return false;

label_ok:
	char cbuf[512];
	size_t len = MIN(sizeof(cbuf), t.end-t.start+1);
	ft_strncpy(cbuf, t.s + t.start, len);
	cbuf[len-1] = 0;
	SSL_LOG(TRACE, "match at pos: % 5zu: %-12s: %s", t.start, __asn1_v2_get_token_kind_name(t.kind), cbuf);
	*token = t;
	if (token->kind == ASN_V2_TOKEN_KIND_COMMENT) {
		return (__asn1_v2_token_next(token));
	}
	return true;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static bool __asn1_v2_token_expect_bstring(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_BSTRING) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: BSTRING");
	return false;
}

static bool __asn1_v2_token_expect_hstring(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_HSTRING) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: HSTRING");
	return false;
}

static bool __asn1_v2_token_expect_cstring(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_CSTRING) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: CSTRING");
	return false;
}

static bool __asn1_v2_token_expect_number(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_NUMBER) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: NUMBER");
	return false;
}

static bool __asn1_v2_token_expect_real_number(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_REAL_NUMBER) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: REAL_NUMBER");
	return false;
}

static bool __asn1_v2_token_expect_keyword(t_asn_v2_token *token, const char *kw)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_KEYWORD) {
		const char *s = token->s + token->start;
		size_t slen = token->end - token->start;
		size_t kwlen = ft_strlen(kw);
		if (kwlen == slen && ft_strneq(s, kw, kwlen)) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: KEYWORD: '%s'", kw);
	return false;
}

static bool __asn1_v2_token_expect_upper_name(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_NAME) {
		size_t i = token->start;
		while (i < token->end && ft_isalpha_upper(token->s[i])) i++;
		if (i == token->end) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: UPPER_NAME");
	return false;
}

static bool __asn1_v2_token_expect_lower_name(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_NAME) {
		size_t i = token->start;
		while (i < token->end && ft_isalpha_lower(token->s[i])) i++;
		if (i == token->end) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: LOWER_NAME");
	return false;
}

static bool __asn1_v2_token_expect_upper_first_name(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_NAME) {
		if (ft_isalpha_upper(token->s[token->start])) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: UPPER_FIRST_NAME");
	return false;
}

static bool __asn1_v2_token_expect_lower_first_name(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_NAME) {
		if (ft_isalpha_lower(token->s[token->start])) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: LOWER_FIRST_NAME");
	return false;
}

static bool __asn1_v2_token_expect_assignment(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_ASSIGNMENT) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: ASSIGNMENT");
	return false;
}

static bool __asn1_v2_token_expect_range(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_RANGE) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: RANGE");
	return false;
}

static bool __asn1_v2_token_expect_ellipsis(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_ELLIPSIS) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: ELLIPSIS");
	return false;
}

static bool __asn1_v2_token_expect_lver_bracket(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_LVER_BRACKET) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: LVER_BRACKET");
	return false;
}

static bool __asn1_v2_token_expect_rver_bracket(t_asn_v2_token *token)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_RVER_BRACKET) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: RVER_BRACKET");
	return false;
}

static bool __asn1_v2_token_expect_char(t_asn_v2_token *token, char c)
{
	if (__asn1_v2_token_next(token) && token->kind == ASN_V2_TOKEN_KIND_SINGLE_CHAR) {
		if (token->s[token->start] == c) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: SINGLE_CHAR: '%c'", c);
	return false;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static bool __asn1_v2_parse_encoding_ref_default(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	t_asn_v2_token old_token = *token;
	if (__asn1_v2_token_expect_upper_first_name(token)) {
		t_asn_v2_token name_token = *token;
		if (__asn1_v2_token_expect_keyword(token, "DEFINITIONS")) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: EncodingReferenceDefault");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_tag_default(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_token_expect_keyword(token, "EXPLICIT") ||
		__asn1_v2_token_expect_keyword(token, "IMPLICIT") ||
		__asn1_v2_token_expect_keyword(token, "AUTOMATIC")
	) {
		t_asn_v2_token tag_token = *token;
		if (__asn1_v2_token_expect_keyword(token, "TAGS")) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: TagDefault");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_extension_default(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	t_asn_v2_token old_token = *token;
	if (__asn1_v2_token_expect_keyword(token, "EXTENSIBILITY")) {
		if (__asn1_v2_token_expect_keyword(token, "IMPLIED")) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: ExtensionDefault");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_id(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	if (__asn1_v2_token_expect_lower_name(token)) {
		// SSL_LOG(TRACE, "match: identifier");
		return true;
	}
	// SSL_LOG(TRACE, "no match: identifier");
	return false;
}

static bool __asn1_v2_parse_name_form(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	if (__asn1_v2_parse_id(ctx, token)) {
		// SSL_LOG(TRACE, "match: NameForm");
		return true;
	}
	// SSL_LOG(TRACE, "no match: NameForm");
	return false;
}

static bool __asn1_v2_parse_definitive_number_form(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	if (__asn1_v2_token_expect_number(token)) {
		// SSL_LOG(TRACE, "match: DefinitiveNumberForm");
		return true;
	}
	// SSL_LOG(TRACE, "no match: DefinitiveNumberForm");
	return false;
}

static bool __asn1_v2_parse_definitive_name_and_number_form(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_parse_id(ctx, token) &&
		__asn1_v2_token_expect_char(token, '(') &&
		__asn1_v2_token_expect_number(token) &&
		__asn1_v2_token_expect_char(token, ')')
	) {
		// SSL_LOG(TRACE, "match: DefinitiveNameAndNumberForm");
		return true;
	}
	// SSL_LOG(TRACE, "no match: DefinitiveNameAndNumberForm");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_definitive_oid_component(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// NameForm | DefinitiveNumberForm | DefinitiveNameAndNumberForm
	t_asn_v2_token old_token = *token;
	if (__asn1_v2_parse_name_form(ctx, token)) {
		// SSL_LOG(TRACE, "match: DefinitiveObjIdComponent");
		return true;
	}
	else if (__asn1_v2_parse_definitive_number_form(ctx, token)) {
		// SSL_LOG(TRACE, "match: DefinitiveObjIdComponent");
		return true;
	}
	else if (__asn1_v2_parse_definitive_name_and_number_form(ctx, token)) {
		// SSL_LOG(TRACE, "match: DefinitiveObjIdComponent");
		return true;
	}
	// SSL_LOG(TRACE, "no match: DefinitiveObjIdComponent");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_definitive_oid_component_list(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// DefinitiveObjIdComponent | DefinitiveObjIdComponent DefinitiveObjIdComponentList
	bool match = false;
	while (__asn1_v2_parse_definitive_oid_component(ctx, token)) {
		match = true;
		/* */
	}
	// SSL_LOG(TRACE, (match ? "match": "no match") ": DefinitiveObjIdComponentList");
	return (match);
}

static bool __asn1_v2_parse_definitive_oid(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// "{" DefinitiveObjIdComponentList "}"
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_token_expect_char(token, '{') &&
		__asn1_v2_parse_definitive_oid_component_list(ctx, token) &&
		__asn1_v2_token_expect_char(token, '}')
	) {
		// SSL_LOG(TRACE, "match: DefinitiveOID");
		return true;
	}
	else {
		// SSL_LOG(TRACE, "no match: DefinitiveOID");
		*token = old_token;
		return false;
	}
}

static bool __asn1_v2_parse_iri_value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("value grammar");
}

static bool __asn1_v2_parse_definitive_oid_and_iri(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// DefinitiveOID IRIValue
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_parse_definitive_oid(ctx, token) &&
		__asn1_v2_parse_iri_value(ctx, token)
	) {
		// SSL_LOG(TRACE, "match: DefinitiveOIDandIRI");
		return true;
	}
	// SSL_LOG(TRACE, "no match: DefinitiveOIDandIRI");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_definitive_id(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// DefinitiveOIDandIRI | DefinitiveOID | empty
	if (__asn1_v2_parse_definitive_oid_and_iri(ctx, token)) {
		/* */
	}
	else if (__asn1_v2_parse_definitive_oid(ctx, token)) {
		/* */
	}
	// SSL_LOG(TRACE, "match: DefinitiveIdentification");
	return true;
}

static bool __asn1_v2_parse_module_id(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// modulereference DefinitiveIdentification
	t_asn_v2_token old_token = *token;
	if (__asn1_v2_token_expect_upper_first_name(token)) {
		t_asn_v2_token name_token = *token;
		if (__asn1_v2_parse_definitive_id(ctx, token)) {
			// SSL_LOG(TRACE, "match: ModuleIdentifier");
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: ModuleIdentifier");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_value_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	if (__asn1_v2_parse_id(ctx, token)) {
		// SSL_LOG(TRACE, "match: typereference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: typereference");
	return false;
}

static bool __asn1_v2_parse_type_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	if (__asn1_v2_token_expect_upper_first_name(token)) {
		// SSL_LOG(TRACE, "match: typereference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: typereference");
	return false;
}

static bool __asn1_v2_parse_object_class_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	if (__asn1_v2_token_expect_upper_name(token)) {
		// SSL_LOG(TRACE, "match: objectclassreference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: objectclassreference");
	return false;
}

static bool __asn1_v2_parse_object_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	if (__asn1_v2_parse_value_ref(ctx, token)) {
		// SSL_LOG(TRACE, "match: objectreference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: objectreference");
	return false;
}

static bool __asn1_v2_parse_object_set_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	if (__asn1_v2_parse_type_ref(ctx, token)) {
		// SSL_LOG(TRACE, "match: objectsetreference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: objectsetreference");
	return false;
}

static bool __asn1_v2_parse_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// typereference | valuereference | objectclassreference | objectreference | objectsetreference
	if (__asn1_v2_parse_type_ref(ctx, token)) {
		// SSL_LOG(TRACE, "match: Reference");
		return true;
	}
	else if (__asn1_v2_parse_value_ref(ctx, token)) {
		// SSL_LOG(TRACE, "match: Reference");
		return true;
	}
	else if (__asn1_v2_parse_object_class_ref(ctx, token)) {
		// SSL_LOG(TRACE, "match: Reference");
		return true;
	}
	else if (__asn1_v2_parse_object_ref(ctx, token)) {
		// SSL_LOG(TRACE, "match: Reference");
		return true;
	}
	else if (__asn1_v2_parse_object_set_ref(ctx, token)) {
		// SSL_LOG(TRACE, "match: Reference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: Reference");
	return false;
}

static bool __asn1_v2_parse_parametrized_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// Reference | Reference "{" "}"
	t_asn_v2_token old_token = *token;
	if (__asn1_v2_parse_ref(ctx, token)) {
		// SSL_LOG(TRACE, "match: ParameterizedReference");
		return true;
	}
	else if (
		__asn1_v2_parse_ref(ctx, token) &&
		__asn1_v2_token_expect_char(token, '{') &&
		__asn1_v2_token_expect_char(token, '}')
	) {
		// SSL_LOG(TRACE, "match: ParameterizedReference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: ParameterizedReference");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_symbol(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// Reference | ParameterizedReference
	if (__asn1_v2_parse_ref(ctx, token)) {
		// SSL_LOG(TRACE, "match: Symbol");
		return true;
	}
	else if (__asn1_v2_parse_parametrized_ref(ctx, token)) {
		// SSL_LOG(TRACE, "match: Symbol");
		return true;
	}
	// SSL_LOG(TRACE, "no match: Symbol");
	return false;
}

static bool __asn1_v2_parse_symbol_list(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// Symbol | SymbolList "," Symbol
	bool match = false;
	if (__asn1_v2_parse_symbol(ctx, token)) {
		match = true;
		t_asn_v2_token old_token = *token;
		while (
			__asn1_v2_token_expect_char(token, ',') &&
			__asn1_v2_parse_symbol(ctx, token)
		) {
			old_token = *token;
		}
		*token = old_token;
	}
	return (match);
}

static bool __asn1_v2_parse_symbols_exported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// SymbolList | empty
	if (__asn1_v2_parse_symbol_list(ctx, token)) {
		/* */
	}
	// SSL_LOG(TRACE, "match: SymbolsExported");
	return true;
}

static bool __asn1_v2_parse_exports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// EXPORTS SymbolsExported ";" | EXPORTS ALL ";" | empty
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_token_expect_keyword(token, "EXPORTS") &&
		__asn1_v2_parse_symbols_exported(ctx, token) &&
		__asn1_v2_token_expect_char(token, ';')
	) {
		/* */
	}
	else if (
		__asn1_v2_token_expect_keyword(token, "EXPORTS") &&
		__asn1_v2_token_expect_keyword(token, "ALL") &&
		__asn1_v2_token_expect_char(token, ';')
	) {
		/* */
	}
	// SSL_LOG(TRACE, "match: Exports");
	*token = old_token;
	return true;
}

static bool __asn1_v2_parse_oid_value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("value grammar");
}

static bool __asn1_v2_parse_assigned_id(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// ObjectIdentifierValue | DefinedValue | empty
	if (__asn1_v2_parse_oid_value(ctx, token)) {
		/* */
	}
	else if (__asn1_v2_parse_defined_value(ctx, token)) {
		/* */
	}
	// SSL_LOG(TRACE, "match: GlobalModuleReference");
	return true;
}

static bool __asn1_v2_parse_module_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	if (__asn1_v2_parse_type_ref(ctx, token)) {
		// SSL_LOG(TRACE, "match: modulereference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: modulereference");
	return false;
}

static bool __asn1_v2_parse_global_module_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// modulereference AssignedIdentifier
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_parse_module_ref(ctx, token) &&
		__asn1_v2_parse_assigned_id(ctx, token)
	) {
		// SSL_LOG(TRACE, "match: GlobalModuleReference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: GlobalModuleReference");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_selection_option(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// empty | WITH "SUCCESSORS" | WITH "DESCENDANTS"
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_token_expect_keyword(token, "WITH") &&
		__asn1_v2_token_expect_char(token, '"') &&
		__asn1_v2_token_expect_keyword(token, "SUCCESSORS") &&
		__asn1_v2_token_expect_char(token, '"')
	) {
		/* */
	}
	else if (
		__asn1_v2_token_expect_keyword(token, "WITH") &&
		__asn1_v2_token_expect_char(token, '"') &&
		__asn1_v2_token_expect_keyword(token, "DESCENDANTS") &&
		__asn1_v2_token_expect_char(token, '"')
	) {
		/* */
	}
	else {
		*token = old_token;
	}
	// SSL_LOG(TRACE, "match: SelectionOption");
	return true;
}

static bool __asn1_v2_parse_symbols_from_module(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// SymbolList FROM GlobalModuleReference SelectionOption
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_parse_symbol_list(ctx, token) &&
		__asn1_v2_token_expect_keyword(token, "FROM") &&
		__asn1_v2_parse_global_module_ref(ctx, token) &&
		__asn1_v2_parse_selection_option(ctx, token)
	) {
		// SSL_LOG(TRACE, "match: SymbolsFromModule");
		return true;
	}
	// SSL_LOG(TRACE, "no match: SymbolsFromModule");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_symbols_from_module_list(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// SymbolsFromModule | SymbolsFromModuleList SymbolsFromModule
	bool match = false;
	while (__asn1_v2_parse_symbols_from_module(ctx, token)) {
		match = true;
		/* */
	}
	return (match);
}

static bool __asn1_v2_parse_symbols_imported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// SymbolsFromModuleList | empty
	t_asn_v2_token old_token = *token;
	if (__asn1_v2_parse_symbols_from_module_list(ctx, token)) {
		/* */
	}
	// SSL_LOG(TRACE, "match: SymbolsImported");
	*token = old_token;
	return true;
}

static bool __asn1_v2_parse_imports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// IMPORTS SymbolsImported ";" | empty
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_token_expect_keyword(token, "IMPORTS") &&
		__asn1_v2_parse_symbols_imported(ctx, token) &&
		__asn1_v2_token_expect_char(token, ';')
	) {
		/* */
	}
	// SSL_LOG(TRACE, "match: Imports");
	*token = old_token;
	return true;
}

static bool __asn1_v2_parse_external_type_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// modulereference "." typereference
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_parse_module_ref(ctx, token) &&
		__asn1_v2_token_expect_char(token, '.') &&
		__asn1_v2_parse_type_ref(ctx, token)
	) {
		// SSL_LOG(TRACE, "match: ExternalTypeReference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: ExternalTypeReference");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_external_value_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// modulereference "." valuereference
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_parse_module_ref(ctx, token) &&
		__asn1_v2_token_expect_char(token, '.') &&
		__asn1_v2_parse_value_ref(ctx, token)
	) {
		// SSL_LOG(TRACE, "match: ExternalValueReference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: ExternalValueReference");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_item_id(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// ItemSpec
	if (__asn1_v2_parse_item_spec(ctx, token)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_item_spec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// typereference | ItemId "." ComponentId
	TODO("__asn1_v2_parse_item_spec");
}

static bool __asn1_v2_parse_absolute_ref(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// "@" ModuleIdentifier "." ItemSpec
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_token_expect_char(token, '@') &&
		__asn1_v2_parse_module_id(ctx, token) &&
		__asn1_v2_token_expect_char(token, '.') &&
		__asn1_v2_parse_item_spec(ctx, token)
	) {
		// SSL_LOG(TRACE, "match: AbsoluteReference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: AbsoluteReference");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_xml_asn1_type_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("xml grammar");
}

static bool __asn1_v2_parse_non_parametrized_type_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// ExternalTypeReference | typereference | xmlasn1typename
	if (__asn1_v2_parse_external_type_ref(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_type_ref(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_xml_asn1_type_name(ctx, token)) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: NonParameterizedTypeName");
	return false;
}

static bool __asn1_v2_parse_parameterized_type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("ParameterizedType");
}

static bool __asn1_v2_parse_parameterized_value_set_type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("ParameterizedValueSetType");
}

static bool __asn1_v2_parse_defined_type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// ExternalTypeReference | typereference | ParameterizedType | ParameterizedValueSetType
	if (__asn1_v2_parse_external_type_ref(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_type_ref(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_parameterized_type(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_parameterized_value_set_type(ctx, token)) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: DefinedType");
	return false;
}

static bool __asn1_v2_parse_parameterized_value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("ParameterizedValue");
}

static bool __asn1_v2_parse_defined_value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// ExternalValueReference | valuereference | ParameterizedValue
	if (__asn1_v2_parse_external_value_ref(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_value_ref(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_parameterized_value(ctx, token)) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: DefinedValue");
	return false;
}

static bool __asn1_v2_parse_type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("type grammar");
}

static bool __asn1_v2_parse_value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("value grammar");
}

static bool __asn1_v2_parse_type_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// typereference "::=" Type
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_parse_type_ref(ctx, token) &&
		__asn1_v2_token_expect_assignment(token) &&
		__asn1_v2_parse_type(ctx, token)
	) {
		// SSL_LOG(TRACE, "match: TypeAssignment");
		return true;
	}
	// SSL_LOG(TRACE, "no match: TypeAssignment");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_value_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// valuereference Type "::=" Value
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_parse_value_ref(ctx, token) &&
		__asn1_v2_parse_type(ctx, token) &&
		__asn1_v2_token_expect_assignment(token) &&
		__asn1_v2_parse_value(ctx, token)
	) {
		// SSL_LOG(TRACE, "match: ValueAssignment");
		return true;
	}
	// SSL_LOG(TRACE, "no match: ValueAssignment");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_xml_typed_value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("xml grammar");
}

static bool __asn1_v2_parse_xml_value_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// valuereference "::=" XMLTypedValue
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_parse_value_ref(ctx, token) &&
		__asn1_v2_token_expect_assignment(token) &&
		__asn1_v2_parse_xml_typed_value(ctx, token)
	) {
		// SSL_LOG(TRACE, "match: XMLValueAssignment");
		return true;
	}
	// SSL_LOG(TRACE, "no match: XMLValueAssignment");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_element_set_specs(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("ElementSetSpecs");
}

static bool __asn1_v2_parse_value_set(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// "{" ElementSetSpecs "}"
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_token_expect_char(token, '{') &&
		__asn1_v2_parse_element_set_specs(ctx, token) &&
		__asn1_v2_token_expect_char(token, '}')
	) {
		// SSL_LOG(TRACE, "match: ValueSet");
		return true;
	}
	// SSL_LOG(TRACE, "no match: ValueSet");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_value_set_type_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// typereference Type "::=" ValueSet
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_parse_type_ref(ctx, token) &&
		__asn1_v2_parse_type(ctx, token) &&
		__asn1_v2_token_expect_assignment(token) &&
		__asn1_v2_parse_value_set(ctx, token)
	) {
		// SSL_LOG(TRACE, "match: ValueSetTypeAssignment");
		return true;
	}
	// SSL_LOG(TRACE, "no match: ValueSetTypeAssignment");
	*token = old_token;
	return false;
}

static bool __asn1_v2_parse_object_class_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("ObjectClassAssignment");
}

static bool __asn1_v2_parse_object_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("ObjectAssignment");
}

static bool __asn1_v2_parse_object_set_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("ObjectSetAssignment");
}

static bool __asn1_v2_parse_parameterized_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	NOT_IMPLEMENTED("ParameterizedAssignment");
}

static bool __asn1_v2_parse_assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// TypeAssignment | ValueAssignment | XMLValueAssignment | ValueSetTypeAssignment |
	// ObjectClassAssignment | ObjectAssignment | ObjectSetAssignment | ParameterizedAssignment
	if (__asn1_v2_parse_type_assignment(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_value_assignment(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_xml_value_assignment(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_value_set_type_assignment(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_object_class_assignment(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_object_assignment(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_object_set_assignment(ctx, token)) {
		return true;
	}
	else if (__asn1_v2_parse_parameterized_assignment(ctx, token)) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: Assignment");
	return false;
}

static bool __asn1_v2_parse_assignment_list(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// Assignment | AssignmentList Assignment
	bool match = false;
	while (__asn1_v2_parse_assignment(ctx, token)) {
		match = true;
		/* */
	}
	return (match);
}

static bool __asn1_v2_token_parse_module_body(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	// Exports Imports AssignmentList | empty
	t_asn_v2_token old_token = *token;
	if (
		__asn1_v2_parse_exports(ctx, token) &&
		__asn1_v2_parse_imports(ctx, token) &&
		__asn1_v2_parse_assignment_list(ctx, token)
	) {
		/* */
	}
	// SSL_LOG(TRACE, "match: ModuleBody");
	*token = old_token;
	return true;
}

static bool __asn1_v2_parse_module(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token)
{
	t_asn_v2_token old_token = *token;

	if (!__asn1_v2_parse_module_id(ctx, token)) goto label_error;
	if (!__asn1_v2_token_expect_keyword(token, "DEFINITIONS")) goto label_error;

	if (__asn1_v2_parse_encoding_ref_default(ctx, token)) {};
	if (__asn1_v2_parse_tag_default(ctx, token)) {};
	if (__asn1_v2_parse_extension_default(ctx, token)) {};

	if (!__asn1_v2_token_expect_assignment(token)) goto label_error;
	if (!__asn1_v2_token_expect_keyword(token, "BEGIN")) goto label_error;
	if (!__asn1_v2_token_parse_module_body(ctx, token)) goto label_error;
	if (!__asn1_v2_token_expect_keyword(token, "END")) goto label_error;

	// SSL_LOG(TRACE, "match: ExtensionDefault");
	return true;

label_error:
	// SSL_LOG(TRACE, "no match: ModuleDefinition");
	*token = old_token;
	return false;
}

int asn1_v2_parse(const char *s, size_t slen)
{
	if (NULL == s) return (SSL_ERR);
	t_asn_v2_parser_ctx ctx = {0};
	t_asn_v2_token token = { .s = s, .slen = slen };

	if (!__asn1_v2_parse_module(&ctx, &token)) {
		return (SSL_ERR);
	}
	if (!token.done) {
		SSL_LOG(ERROR, "parsing is not complete");
		return (SSL_ERR);
	}
	return (SSL_OK);
}
