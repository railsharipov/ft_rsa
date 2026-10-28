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

typedef struct s_asn_v2_parser_ctx {
	t_asn_v2_token token;
	t_list ops;
} t_asn_v2_parser_ctx;

typedef bool (*t_func_asn_v2_parse)(t_asn_v2_parser_ctx *ctx);

typedef struct s_asn_v2_parser_op {
	t_func_asn_v2_parse *f;
	t_asn_v2_token token;
} t_asn_v2_parser_op;

const struct {
	const char *name;
	const char *lex;
} RESERVED_KEYWORDS[] = {
	{"ABSENT",           "ABSENT"},
	{"ENCODED",          "ENCODED"},
	{"INTERSECTION",     "INTERSECTION"},
	{"SEQUENCE",         "SEQUENCE"},
	{"ABSTRACT_SYNTAX",  "ABSTRACT-SYNTAX"},
	{"ENCODING_CONTROL", "ENCODING-CONTROL"},
	{"ISO646String",     "ISO646String"},
	{"SET",              "SET"},
	{"ALL",              "ALL"},
	{"END",              "END"},
	{"MAX",              "MAX"},
	{"SETTINGS",         "SETTINGS"},
	{"APPLICATION",      "APPLICATION"},
	{"ENUMERATED",       "ENUMERATED"},
	{"MIN",              "MIN"},
	{"SIZE",             "SIZE"},
	{"AUTOMATIC",        "AUTOMATIC"},
	{"EXCEPT",           "EXCEPT"},
	{"MINUS_INFINITY",   "MINUS-INFINITY"},
	{"STRING",           "STRING"},
	{"BEGIN",            "BEGIN"},
	{"EXPLICIT",         "EXPLICIT"},
	{"NOT_A_NUMBER",     "NOT-A-NUMBER"},
	{"SYNTAX",           "SYNTAX"},
	{"BIT",              "BIT"},
	{"EXPORTS",          "EXPORTS"},
	{"NULL",             "NULL"},
	{"T61String",        "T61String"},
	{"BMPString",        "BMPString"},
	{"EXTENSIBILITY",    "EXTENSIBILITY"},
	{"NumericString",    "NumericString"},
	{"TAGS",             "TAGS"},
	{"BOOLEAN",          "BOOLEAN"},
	{"EXTERNAL",         "EXTERNAL"},
	{"OBJECT",           "OBJECT"},
	{"TeletexString",    "TeletexString"},
	{"BY",               "BY"},
	{"FALSE",            "FALSE"},
	{"ObjectDescriptor", "ObjectDescriptor"},
	{"TIME",             "TIME"},
	{"CHARACTER",        "CHARACTER"},
	{"FROM",             "FROM"},
	{"OCTET",            "OCTET"},
	{"TIME_OF_DAY",      "TIME-OF-DAY"},
	{"CHOICE",           "CHOICE"},
	{"GeneralizedTime",  "GeneralizedTime"},
	{"OF",               "OF"},
	{"TRUE",             "TRUE"},
	{"CLASS",            "CLASS"},
	{"GeneralString",    "GeneralString"},
	{"OID_IRI",          "OID-IRI"},
	{"TYPE_IDENTIFIER",  "TYPE-IDENTIFIER"},
	{"COMPONENT",        "COMPONENT"},
	{"GraphicString",    "GraphicString"},
	{"OPTIONAL",         "OPTIONAL"},
	{"UNION",            "UNION"},
	{"COMPONENTS",       "COMPONENTS"},
	{"IA5String",        "IA5String"},
	{"PATTERN",          "PATTERN"},
	{"UNIQUE",           "UNIQUE"},
	{"CONSTRAINED",      "CONSTRAINED"},
	{"IDENTIFIER",       "IDENTIFIER"},
	{"PDV",              "PDV"},
	{"UNIVERSAL",        "UNIVERSAL"},
	{"CONTAINING",       "CONTAINING"},
	{"IMPLICIT",         "IMPLICIT"},
	{"PLUS_INFINITY",    "PLUS-INFINITY"},
	{"UniversalString",  "UniversalString"},
	{"DATE",             "DATE"},
	{"IMPLIED",          "IMPLIED"},
	{"PRESENT",          "PRESENT"},
	{"UTCTime",          "UTCTime"},
	{"DATE_TIME",        "DATE-TIME"},
	{"IMPORTS",          "IMPORTS"},
	{"PrintableString",  "PrintableString"},
	{"UTF8String",       "UTF8String"},
	{"DEFAULT",          "DEFAULT"},
	{"INCLUDES",         "INCLUDES"},
	{"PRIVATE",          "PRIVATE"},
	{"VideotexString",   "VideotexString"},
	{"DEFINITIONS",      "DEFINITIONS"},
	{"INSTANCE",         "INSTANCE"},
	{"REAL",             "REAL"},
	{"VisibleString",    "VisibleString"},
	{"DURATION",         "DURATION"},
	{"INSTRUCTIONS",     "INSTRUCTIONS"},
	{"RELATIVE_OID",     "RELATIVE-OID"},
	{"WITH",             "WITH"},
	{"EMBEDDED",         "EMBEDDED"},
	{"INTEGER",          "INTEGER"},
	{"RELATIVE_OID_IRI", "RELATIVE-OID-IRI"},
};
const size_t RESERVED_KEYWORD_COUNT = sizeof(RESERVED_KEYWORDS)/sizeof(*RESERVED_KEYWORDS);

#define ASN_V2_LEX_ELLIPSIS		"..."
#define ASN_V2_LEX_ASSIGNMENT	"::="
#define ASN_V2_LEX_RANGE		".."
#define ASN_V2_LEX_LVER_BRACKET	"[["
#define ASN_V2_LEX_RVER_BRACKET	"]]"

const struct {
	const char *name;
	const char chr;
} SINGLE_CHAR_LEXES[] = {
	{"L_CURLY_BRKT",  '{'},
	{"R_CURLY_BRKT",  '}'},
	{"L_ANGLE_BRKT",  '<'},
	{"R_ANGLE_BRKT",  '>'},
	{"COMMA",         ','},
	{"DOT",           '.'},
	{"SLASH",         '/'},
	{"L_BRKT",        '('},
	{"R_BRKT",        ')'},
	{"L_SQUARE_BRKT", '['},
	{"R_SQUARE_BRKT", ']'},
	{"HYPHEN",        '-'},
	{"COLON",         ':'},
	{"EQUAL",         '='},
	{"DOUBLE_QUOTE",  '"'},
	{"QUOTE",         '\''},
	{"SEMICOLON",     ';'},
	{"AT",            '@'},
	{"PIPE",          '|'},
	{"BANG",          '!'},
	{"CARET",         '^'},
};
const size_t SINGLE_CHAR_LEX_COUNT = sizeof(SINGLE_CHAR_LEXES)/sizeof(*SINGLE_CHAR_LEXES);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

bool __asn1_v2_is_bound(char c)
{
	if (c == '\0') return true;
	if (ft_iseolws(c)) return true;
	for (size_t i = 0; i < SINGLE_CHAR_LEX_COUNT; i++) {
		if (c == SINGLE_CHAR_LEXES[i].chr) return true;
	}
	return false;
}

bool __asn1_v2_token_next(t_asn_v2_parser_ctx *ctx)
{
	assert(NULL != ctx);
	t_asn_v2_token t = ctx->token;
	if (t.end >= t.slen) return false;
	t.start = t.end;

	while (t.start < t.slen && ft_iseolws(t.s[t.start])) t.start++;
	if (t.start == t.slen) {
		ctx->token.kind = ASN_V2_TOKEN_KIND_NONE;
		ctx->token.done = true;
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
		size_t kwlen = ft_strlen(RESERVED_KEYWORDS[i].lex);
		if (ft_strneq(s, RESERVED_KEYWORDS[i].lex, kwlen)) {
			t.kind = ASN_V2_TOKEN_KIND_KEYWORD;
			t.end = t.start + kwlen;
			goto label_ok;
		}
	}

	for (size_t i = 0; i < SINGLE_CHAR_LEX_COUNT; i++) {
		if (*s == SINGLE_CHAR_LEXES[i].chr) {
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

	SSL_LOG(TRACE, "no match at pos: %zu", ctx->token.end);
	return false;

label_ok:
	char cbuf[512];
	size_t len = MIN(sizeof(cbuf), t.end-t.start+1);
	ft_strncpy(cbuf, t.s + t.start, len);
	cbuf[len-1] = 0;
	SSL_LOG(TRACE, "match at pos: % 5zu: %-12s: %s", t.start, __asn1_v2_get_token_kind_name(t.kind), cbuf);
	ctx->token = t;
	if (ctx->token.kind == ASN_V2_TOKEN_KIND_COMMENT) {
		return (__asn1_v2_token_next(ctx));
	}
	return true;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static bool __asn1_v2_parse_char(t_asn_v2_parser_ctx *ctx, char c)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_SINGLE_CHAR) {
		if (ctx->token.s[ctx->token.start] == c) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: SINGLE_CHAR: '%c'", c);
	return false;
}

static bool __asn1_v2_parse_token_L_CURLY_BRKT(t_asn_v2_parser_ctx *ctx)  { return (__asn1_v2_parse_char(ctx, '{')); }
static bool __asn1_v2_parse_token_R_CURLY_BRKT(t_asn_v2_parser_ctx *ctx)  { return (__asn1_v2_parse_char(ctx, '}')); }
static bool __asn1_v2_parse_token_L_ANGLE_BRKT(t_asn_v2_parser_ctx *ctx)  { return (__asn1_v2_parse_char(ctx, '<')); }
static bool __asn1_v2_parse_token_R_ANGLE_BRKT(t_asn_v2_parser_ctx *ctx)  { return (__asn1_v2_parse_char(ctx, '>')); }
static bool __asn1_v2_parse_token_COMMA(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_char(ctx, ',')); }
static bool __asn1_v2_parse_token_DOT(t_asn_v2_parser_ctx *ctx)           { return (__asn1_v2_parse_char(ctx, '.')); }
static bool __asn1_v2_parse_token_SLASH(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_char(ctx, '/')); }
static bool __asn1_v2_parse_token_L_BRKT(t_asn_v2_parser_ctx *ctx)        { return (__asn1_v2_parse_char(ctx, '(')); }
static bool __asn1_v2_parse_token_R_BRKT(t_asn_v2_parser_ctx *ctx)        { return (__asn1_v2_parse_char(ctx, ')')); }
static bool __asn1_v2_parse_token_L_SQUARE_BRKT(t_asn_v2_parser_ctx *ctx) { return (__asn1_v2_parse_char(ctx, '[')); }
static bool __asn1_v2_parse_token_R_SQUARE_BRKT(t_asn_v2_parser_ctx *ctx) { return (__asn1_v2_parse_char(ctx, ']')); }
static bool __asn1_v2_parse_token_HYPHEN(t_asn_v2_parser_ctx *ctx)        { return (__asn1_v2_parse_char(ctx, '-')); }
static bool __asn1_v2_parse_token_COLON(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_char(ctx, ':')); }
static bool __asn1_v2_parse_token_EQUAL(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_char(ctx, '=')); }
static bool __asn1_v2_parse_token_DOUBLE_QUOTE(t_asn_v2_parser_ctx *ctx)  { return (__asn1_v2_parse_char(ctx, '"')); }
static bool __asn1_v2_parse_token_QUOTE(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_char(ctx, '\'')); }
static bool __asn1_v2_parse_token_SEMICOLON(t_asn_v2_parser_ctx *ctx)     { return (__asn1_v2_parse_char(ctx, ';')); }
static bool __asn1_v2_parse_token_AT(t_asn_v2_parser_ctx *ctx)            { return (__asn1_v2_parse_char(ctx, '@')); }
static bool __asn1_v2_parse_token_PIPE(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_char(ctx, '|')); }
static bool __asn1_v2_parse_token_BANG(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_char(ctx, '!')); }
static bool __asn1_v2_parse_token_CARET(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_char(ctx, '^')); }

static bool __asn1_v2_parse_keyword(t_asn_v2_parser_ctx *ctx, const char *kw)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_KEYWORD) {
		const char *s = ctx->token.s + ctx->token.start;
		size_t slen = ctx->token.end - ctx->token.start;
		size_t kwlen = ft_strlen(kw);
		if (kwlen == slen && ft_strneq(s, kw, kwlen)) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: KEYWORD: '%s'", kw);
	return false;
}

static bool __asn1_v2_parse_keyword_ABSENT(t_asn_v2_parser_ctx *ctx)           { return (__asn1_v2_parse_keyword(ctx, "ABSENT")); }
static bool __asn1_v2_parse_keyword_ENCODED(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_keyword(ctx, "ENCODED")); }
static bool __asn1_v2_parse_keyword_INTERSECTION(t_asn_v2_parser_ctx *ctx)     { return (__asn1_v2_parse_keyword(ctx, "INTERSECTION")); }
static bool __asn1_v2_parse_keyword_SEQUENCE(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_keyword(ctx, "SEQUENCE")); }
static bool __asn1_v2_parse_keyword_ABSTRACT_SYNTAX(t_asn_v2_parser_ctx *ctx)  { return (__asn1_v2_parse_keyword(ctx, "ABSTRACT-SYNTAX")); }
static bool __asn1_v2_parse_keyword_ENCODING_CONTROL(t_asn_v2_parser_ctx *ctx) { return (__asn1_v2_parse_keyword(ctx, "ENCODING-CONTROL")); }
static bool __asn1_v2_parse_keyword_ISO646String(t_asn_v2_parser_ctx *ctx)     { return (__asn1_v2_parse_keyword(ctx, "ISO646String")); }
static bool __asn1_v2_parse_keyword_SET(t_asn_v2_parser_ctx *ctx)              { return (__asn1_v2_parse_keyword(ctx, "SET")); }
static bool __asn1_v2_parse_keyword_ALL(t_asn_v2_parser_ctx *ctx)              { return (__asn1_v2_parse_keyword(ctx, "ALL")); }
static bool __asn1_v2_parse_keyword_END(t_asn_v2_parser_ctx *ctx)              { return (__asn1_v2_parse_keyword(ctx, "END")); }
static bool __asn1_v2_parse_keyword_MAX(t_asn_v2_parser_ctx *ctx)              { return (__asn1_v2_parse_keyword(ctx, "MAX")); }
static bool __asn1_v2_parse_keyword_SETTINGS(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_keyword(ctx, "SETTINGS")); }
static bool __asn1_v2_parse_keyword_APPLICATION(t_asn_v2_parser_ctx *ctx)      { return (__asn1_v2_parse_keyword(ctx, "APPLICATION")); }
static bool __asn1_v2_parse_keyword_ENUMERATED(t_asn_v2_parser_ctx *ctx)       { return (__asn1_v2_parse_keyword(ctx, "ENUMERATED")); }
static bool __asn1_v2_parse_keyword_MIN(t_asn_v2_parser_ctx *ctx)              { return (__asn1_v2_parse_keyword(ctx, "MIN")); }
static bool __asn1_v2_parse_keyword_SIZE(t_asn_v2_parser_ctx *ctx)             { return (__asn1_v2_parse_keyword(ctx, "SIZE")); }
static bool __asn1_v2_parse_keyword_AUTOMATIC(t_asn_v2_parser_ctx *ctx)        { return (__asn1_v2_parse_keyword(ctx, "AUTOMATIC")); }
static bool __asn1_v2_parse_keyword_EXCEPT(t_asn_v2_parser_ctx *ctx)           { return (__asn1_v2_parse_keyword(ctx, "EXCEPT")); }
static bool __asn1_v2_parse_keyword_MINUS_INFINITY(t_asn_v2_parser_ctx *ctx)   { return (__asn1_v2_parse_keyword(ctx, "MINUS-INFINITY")); }
static bool __asn1_v2_parse_keyword_STRING(t_asn_v2_parser_ctx *ctx)           { return (__asn1_v2_parse_keyword(ctx, "STRING")); }
static bool __asn1_v2_parse_keyword_BEGIN(t_asn_v2_parser_ctx *ctx)            { return (__asn1_v2_parse_keyword(ctx, "BEGIN")); }
static bool __asn1_v2_parse_keyword_EXPLICIT(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_keyword(ctx, "EXPLICIT")); }
static bool __asn1_v2_parse_keyword_NOT_A_NUMBER(t_asn_v2_parser_ctx *ctx)     { return (__asn1_v2_parse_keyword(ctx, "NOT-A-NUMBER")); }
static bool __asn1_v2_parse_keyword_SYNTAX(t_asn_v2_parser_ctx *ctx)           { return (__asn1_v2_parse_keyword(ctx, "SYNTAX")); }
static bool __asn1_v2_parse_keyword_BIT(t_asn_v2_parser_ctx *ctx)              { return (__asn1_v2_parse_keyword(ctx, "BIT")); }
static bool __asn1_v2_parse_keyword_EXPORTS(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_keyword(ctx, "EXPORTS")); }
static bool __asn1_v2_parse_keyword_NULL(t_asn_v2_parser_ctx *ctx)             { return (__asn1_v2_parse_keyword(ctx, "NULL")); }
static bool __asn1_v2_parse_keyword_T61String(t_asn_v2_parser_ctx *ctx)        { return (__asn1_v2_parse_keyword(ctx, "T61String")); }
static bool __asn1_v2_parse_keyword_BMPString(t_asn_v2_parser_ctx *ctx)        { return (__asn1_v2_parse_keyword(ctx, "BMPString")); }
static bool __asn1_v2_parse_keyword_EXTENSIBILITY(t_asn_v2_parser_ctx *ctx)    { return (__asn1_v2_parse_keyword(ctx, "EXTENSIBILITY")); }
static bool __asn1_v2_parse_keyword_NumericString(t_asn_v2_parser_ctx *ctx)    { return (__asn1_v2_parse_keyword(ctx, "NumericString")); }
static bool __asn1_v2_parse_keyword_TAGS(t_asn_v2_parser_ctx *ctx)             { return (__asn1_v2_parse_keyword(ctx, "TAGS")); }
static bool __asn1_v2_parse_keyword_BOOLEAN(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_keyword(ctx, "BOOLEAN")); }
static bool __asn1_v2_parse_keyword_EXTERNAL(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_keyword(ctx, "EXTERNAL")); }
static bool __asn1_v2_parse_keyword_OBJECT(t_asn_v2_parser_ctx *ctx)           { return (__asn1_v2_parse_keyword(ctx, "OBJECT")); }
static bool __asn1_v2_parse_keyword_TeletexString(t_asn_v2_parser_ctx *ctx)    { return (__asn1_v2_parse_keyword(ctx, "TeletexString")); }
static bool __asn1_v2_parse_keyword_BY(t_asn_v2_parser_ctx *ctx)               { return (__asn1_v2_parse_keyword(ctx, "BY")); }
static bool __asn1_v2_parse_keyword_FALSE(t_asn_v2_parser_ctx *ctx)            { return (__asn1_v2_parse_keyword(ctx, "FALSE")); }
static bool __asn1_v2_parse_keyword_ObjectDescriptor(t_asn_v2_parser_ctx *ctx)        { return (__asn1_v2_parse_keyword(ctx, "ObjectDescriptor")); }
static bool __asn1_v2_parse_keyword_TIME(t_asn_v2_parser_ctx *ctx)             { return (__asn1_v2_parse_keyword(ctx, "TIME")); }
static bool __asn1_v2_parse_keyword_CHARACTER(t_asn_v2_parser_ctx *ctx)        { return (__asn1_v2_parse_keyword(ctx, "CHARACTER")); }
static bool __asn1_v2_parse_keyword_FROM(t_asn_v2_parser_ctx *ctx)             { return (__asn1_v2_parse_keyword(ctx, "FROM")); }
static bool __asn1_v2_parse_keyword_OCTET(t_asn_v2_parser_ctx *ctx)            { return (__asn1_v2_parse_keyword(ctx, "OCTET")); }
static bool __asn1_v2_parse_keyword_TIME_OF_DAY(t_asn_v2_parser_ctx *ctx)      { return (__asn1_v2_parse_keyword(ctx, "TIME-OF-DAY")); }
static bool __asn1_v2_parse_keyword_CHOICE(t_asn_v2_parser_ctx *ctx)           { return (__asn1_v2_parse_keyword(ctx, "CHOICE")); }
static bool __asn1_v2_parse_keyword_GeneralizedTime(t_asn_v2_parser_ctx *ctx)  { return (__asn1_v2_parse_keyword(ctx, "GeneralizedTime")); }
static bool __asn1_v2_parse_keyword_OF(t_asn_v2_parser_ctx *ctx)               { return (__asn1_v2_parse_keyword(ctx, "OF")); }
static bool __asn1_v2_parse_keyword_TRUE(t_asn_v2_parser_ctx *ctx)             { return (__asn1_v2_parse_keyword(ctx, "TRUE")); }
static bool __asn1_v2_parse_keyword_CLASS(t_asn_v2_parser_ctx *ctx)            { return (__asn1_v2_parse_keyword(ctx, "CLASS")); }
static bool __asn1_v2_parse_keyword_GeneralString(t_asn_v2_parser_ctx *ctx)    { return (__asn1_v2_parse_keyword(ctx, "GeneralString")); }
static bool __asn1_v2_parse_keyword_OID_IRI(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_keyword(ctx, "OID-IRI")); }
static bool __asn1_v2_parse_keyword_TYPE_IDENTIFIER(t_asn_v2_parser_ctx *ctx)  { return (__asn1_v2_parse_keyword(ctx, "TYPE-IDENTIFIER")); }
static bool __asn1_v2_parse_keyword_COMPONENT(t_asn_v2_parser_ctx *ctx)        { return (__asn1_v2_parse_keyword(ctx, "COMPONENT")); }
static bool __asn1_v2_parse_keyword_GraphicString(t_asn_v2_parser_ctx *ctx)    { return (__asn1_v2_parse_keyword(ctx, "GraphicString")); }
static bool __asn1_v2_parse_keyword_OPTIONAL(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_keyword(ctx, "OPTIONAL")); }
static bool __asn1_v2_parse_keyword_UNION(t_asn_v2_parser_ctx *ctx)            { return (__asn1_v2_parse_keyword(ctx, "UNION")); }
static bool __asn1_v2_parse_keyword_COMPONENTS(t_asn_v2_parser_ctx *ctx)       { return (__asn1_v2_parse_keyword(ctx, "COMPONENTS")); }
static bool __asn1_v2_parse_keyword_IA5String(t_asn_v2_parser_ctx *ctx)        { return (__asn1_v2_parse_keyword(ctx, "IA5String")); }
static bool __asn1_v2_parse_keyword_PATTERN(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_keyword(ctx, "PATTERN")); }
static bool __asn1_v2_parse_keyword_UNIQUE(t_asn_v2_parser_ctx *ctx)           { return (__asn1_v2_parse_keyword(ctx, "UNIQUE")); }
static bool __asn1_v2_parse_keyword_CONSTRAINED(t_asn_v2_parser_ctx *ctx)      { return (__asn1_v2_parse_keyword(ctx, "CONSTRAINED")); }
static bool __asn1_v2_parse_keyword_IDENTIFIER(t_asn_v2_parser_ctx *ctx)       { return (__asn1_v2_parse_keyword(ctx, "IDENTIFIER")); }
static bool __asn1_v2_parse_keyword_PDV(t_asn_v2_parser_ctx *ctx)              { return (__asn1_v2_parse_keyword(ctx, "PDV")); }
static bool __asn1_v2_parse_keyword_UNIVERSAL(t_asn_v2_parser_ctx *ctx)        { return (__asn1_v2_parse_keyword(ctx, "UNIVERSAL")); }
static bool __asn1_v2_parse_keyword_CONTAINING(t_asn_v2_parser_ctx *ctx)       { return (__asn1_v2_parse_keyword(ctx, "CONTAINING")); }
static bool __asn1_v2_parse_keyword_IMPLICIT(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_keyword(ctx, "IMPLICIT")); }
static bool __asn1_v2_parse_keyword_PLUS_INFINITY(t_asn_v2_parser_ctx *ctx)    { return (__asn1_v2_parse_keyword(ctx, "PLUS-INFINITY")); }
static bool __asn1_v2_parse_keyword_UniversalString(t_asn_v2_parser_ctx *ctx)  { return (__asn1_v2_parse_keyword(ctx, "UniversalString")); }
static bool __asn1_v2_parse_keyword_DATE(t_asn_v2_parser_ctx *ctx)             { return (__asn1_v2_parse_keyword(ctx, "DATE")); }
static bool __asn1_v2_parse_keyword_IMPLIED(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_keyword(ctx, "IMPLIED")); }
static bool __asn1_v2_parse_keyword_PRESENT(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_keyword(ctx, "PRESENT")); }
static bool __asn1_v2_parse_keyword_UTCTime(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_keyword(ctx, "UTCTime")); }
static bool __asn1_v2_parse_keyword_DATE_TIME(t_asn_v2_parser_ctx *ctx)        { return (__asn1_v2_parse_keyword(ctx, "DATE-TIME")); }
static bool __asn1_v2_parse_keyword_IMPORTS(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_keyword(ctx, "IMPORTS")); }
static bool __asn1_v2_parse_keyword_PrintableString(t_asn_v2_parser_ctx *ctx)  { return (__asn1_v2_parse_keyword(ctx, "PrintableString")); }
static bool __asn1_v2_parse_keyword_UTF8String(t_asn_v2_parser_ctx *ctx)       { return (__asn1_v2_parse_keyword(ctx, "UTF8String")); }
static bool __asn1_v2_parse_keyword_DEFAULT(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_keyword(ctx, "DEFAULT")); }
static bool __asn1_v2_parse_keyword_INCLUDES(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_keyword(ctx, "INCLUDES")); }
static bool __asn1_v2_parse_keyword_PRIVATE(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_keyword(ctx, "PRIVATE")); }
static bool __asn1_v2_parse_keyword_VideotexString(t_asn_v2_parser_ctx *ctx)   { return (__asn1_v2_parse_keyword(ctx, "VideotexString")); }
static bool __asn1_v2_parse_keyword_DEFINITIONS(t_asn_v2_parser_ctx *ctx)      { return (__asn1_v2_parse_keyword(ctx, "DEFINITIONS")); }
static bool __asn1_v2_parse_keyword_INSTANCE(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_keyword(ctx, "INSTANCE")); }
static bool __asn1_v2_parse_keyword_REAL(t_asn_v2_parser_ctx *ctx)             { return (__asn1_v2_parse_keyword(ctx, "REAL")); }
static bool __asn1_v2_parse_keyword_VisibleString(t_asn_v2_parser_ctx *ctx)    { return (__asn1_v2_parse_keyword(ctx, "VisibleString")); }
static bool __asn1_v2_parse_keyword_DURATION(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_keyword(ctx, "DURATION")); }
static bool __asn1_v2_parse_keyword_INSTRUCTIONS(t_asn_v2_parser_ctx *ctx)     { return (__asn1_v2_parse_keyword(ctx, "INSTRUCTIONS")); }
static bool __asn1_v2_parse_keyword_RELATIVE_OID(t_asn_v2_parser_ctx *ctx)     { return (__asn1_v2_parse_keyword(ctx, "RELATIVE-OID")); }
static bool __asn1_v2_parse_keyword_WITH(t_asn_v2_parser_ctx *ctx)             { return (__asn1_v2_parse_keyword(ctx, "WITH")); }
static bool __asn1_v2_parse_keyword_EMBEDDED(t_asn_v2_parser_ctx *ctx)         { return (__asn1_v2_parse_keyword(ctx, "EMBEDDED")); }
static bool __asn1_v2_parse_keyword_INTEGER(t_asn_v2_parser_ctx *ctx)          { return (__asn1_v2_parse_keyword(ctx, "INTEGER")); }
static bool __asn1_v2_parse_keyword_RELATIVE_OID_IRI(t_asn_v2_parser_ctx *ctx) { return (__asn1_v2_parse_keyword(ctx, "RELATIVE-OID-IRI")); }

static bool __asn1_v2_parse_token_ASSIGNMENT(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_ASSIGNMENT) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: ASSIGNMENT");
	return false;
}

static bool __asn1_v2_parse_token_RANGE(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_RANGE) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: RANGE");
	return false;
}

static bool __asn1_v2_parse_token_ELLIPSIS(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_ELLIPSIS) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: ELLIPSIS");
	return false;
}

static bool __asn1_v2_parse_token_LVER_BRACKET(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_LVER_BRACKET) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: LVER_BRACKET");
	return false;
}

static bool __asn1_v2_parse_token_RVER_BRACKET(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_RVER_BRACKET) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: RVER_BRACKET");
	return false;
}

static bool __asn1_v2_parse_bstring(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_BSTRING) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: BSTRING");
	return false;
}

static bool __asn1_v2_parse_hstring(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_HSTRING) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: HSTRING");
	return false;
}

static bool __asn1_v2_parse_cstring(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_CSTRING) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: CSTRING");
	return false;
}

static bool __asn1_v2_parse_number(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_NUMBER) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: NUMBER");
	return false;
}

static bool __asn1_v2_parse_real_number(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_REAL_NUMBER) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: REAL_NUMBER");
	return false;
}

static bool __asn1_v2_parse_upper_name(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_NAME) {
		size_t i = ctx->token.start;
		while (i < ctx->token.end && ft_isalpha_upper(ctx->token.s[i])) i++;
		if (i == ctx->token.end) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: UPPER_NAME");
	return false;
}

static bool __asn1_v2_parse_lower_name(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_NAME) {
		size_t i = ctx->token.start;
		while (i < ctx->token.end && ft_isalpha_lower(ctx->token.s[i])) i++;
		if (i == ctx->token.end) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: LOWER_NAME");
	return false;
}

static bool __asn1_v2_parse_upper_first_name(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_NAME) {
		if (ft_isalpha_upper(ctx->token.s[ctx->token.start])) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: UPPER_FIRST_NAME");
	return false;
}

static bool __asn1_v2_parse_lower_first_name(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_token_next(ctx) && ctx->token.kind == ASN_V2_TOKEN_KIND_NAME) {
		if (ft_isalpha_lower(ctx->token.s[ctx->token.start])) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: LOWER_FIRST_NAME");
	return false;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static bool __asn1_v2_parse_identifier(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_valuereference(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_typereference(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_objectclassreference(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_objectreference(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_objectsetreference(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_modulereference(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_xmlasn1typename(t_asn_v2_parser_ctx *ctx);

static bool __asn1_v2_parse_EncodingReferenceDefault(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_TagDefault(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ExtensionDefault(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_NameForm(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_DefinitiveNumberForm(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_DefinitiveNameAndNumberForm(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_DefinitiveObjIdComponent(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_DefinitiveObjIdComponentList(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_DefinitiveOid(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_IriValue(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_DefinitiveOidAndIri(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_DefinitiveIdentification(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ModuleIdentifier(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_Reference(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ParameterizedReference(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_Symbol(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_SymbolList(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_SymbolsExported(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_Exports(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ObjectIdentifierValue(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_AssignedIdentifier(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ModuleRef(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_GlobalModuleReference(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_SelectionOption(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_SymbolsFromModule(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_SymbolsFromModuleList(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_SymbolsImported(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_Imports(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ExternalTypeReference(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ExternalValueReference(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_NonParametrizedTypeName(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ParameterizedType(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ParameterizedValueSetType(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_DefinedType(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ParameterizedValue(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_DefinedValue(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_Type(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_Value(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_TypeAssignment(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ValueAssignment(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_XMLTypedValue(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_XMLValueAssignment(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ElementSetSpecs(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ValueSet(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ValueSetTypeAssignment(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ObjectClassAssignment(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ObjectAssignment(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ObjectSetAssignment(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ParameterizedAssignment(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_Assignment(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_AssignmentList(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ModuleBody(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_Module(t_asn_v2_parser_ctx *ctx);
static bool __asn1_v2_parse_ItemSpec(t_asn_v2_parser_ctx *ctx);

static bool __asn1_v2_parse_identifier(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_parse_lower_name(ctx)) {
		// SSL_LOG(TRACE, "match: identifier");
		return true;
	}
	// SSL_LOG(TRACE, "no match: identifier");
	return false;
}

static bool __asn1_v2_parse_valuereference(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_parse_identifier(ctx)) {
		// SSL_LOG(TRACE, "match: valuereference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: valuereference");
	return false;
}

static bool __asn1_v2_parse_typereference(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_parse_upper_first_name(ctx)) {
		// SSL_LOG(TRACE, "match: typereference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: typereference");
	return false;
}

static bool __asn1_v2_parse_objectclassreference(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_parse_upper_name(ctx)) {
		// SSL_LOG(TRACE, "match: objectclassreference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: objectclassreference");
	return false;
}

static bool __asn1_v2_parse_objectreference(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_parse_valuereference(ctx)) {
		// SSL_LOG(TRACE, "match: objectreference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: objectreference");
	return false;
}

static bool __asn1_v2_parse_objectsetreference(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_parse_typereference(ctx)) {
		// SSL_LOG(TRACE, "match: objectsetreference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: objectsetreference");
	return false;
}

static bool __asn1_v2_parse_modulereference(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_parse_typereference(ctx)) {
		// SSL_LOG(TRACE, "match: valuereference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: valuereference");
	return false;
}

static bool __asn1_v2_parse_EncodingReferenceDefault(t_asn_v2_parser_ctx *ctx)
{
	t_asn_v2_token old_token = ctx->token;
	if (__asn1_v2_parse_upper_first_name(ctx)) {
		t_asn_v2_token name_token = ctx->token;
		if (__asn1_v2_parse_keyword_DEFINITIONS(ctx)) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: EncodingReferenceDefault");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_TagDefault(t_asn_v2_parser_ctx *ctx)
{
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_keyword_EXPLICIT(ctx) ||
		__asn1_v2_parse_keyword_IMPLICIT(ctx) ||
		__asn1_v2_parse_keyword_AUTOMATIC(ctx)
	) {
		t_asn_v2_token tag_token = ctx->token;
		if (__asn1_v2_parse_keyword_TAGS(ctx)) {
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: TagDefault");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_ExtensionDefault(t_asn_v2_parser_ctx *ctx)
{
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_keyword_EXTENSIBILITY(ctx) &&
		__asn1_v2_parse_keyword_IMPLIED(ctx)
	) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: ExtensionDefault");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_NameForm(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_parse_identifier(ctx)) {
		// SSL_LOG(TRACE, "match: NameForm");
		return true;
	}
	// SSL_LOG(TRACE, "no match: NameForm");
	return false;
}

static bool __asn1_v2_parse_DefinitiveNumberForm(t_asn_v2_parser_ctx *ctx)
{
	if (__asn1_v2_parse_number(ctx)) {
		// SSL_LOG(TRACE, "match: DefinitiveNumberForm");
		return true;
	}
	// SSL_LOG(TRACE, "no match: DefinitiveNumberForm");
	return false;
}

static bool __asn1_v2_parse_DefinitiveNameAndNumberForm(t_asn_v2_parser_ctx *ctx)
{
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_identifier(ctx) &&
		__asn1_v2_parse_token_L_BRKT(ctx) &&
		__asn1_v2_parse_number(ctx) &&
		__asn1_v2_parse_token_R_BRKT(ctx)
	) {
		// SSL_LOG(TRACE, "match: DefinitiveNameAndNumberForm");
		return true;
	}
	// SSL_LOG(TRACE, "no match: DefinitiveNameAndNumberForm");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_DefinitiveObjIdComponent(t_asn_v2_parser_ctx *ctx)
{
	// NameForm | DefinitiveNumberForm | DefinitiveNameAndNumberForm
	t_asn_v2_token old_token = ctx->token;
	if (__asn1_v2_parse_NameForm(ctx)) {
		// SSL_LOG(TRACE, "match: DefinitiveObjIdComponent");
		return true;
	}
	else if (__asn1_v2_parse_DefinitiveNumberForm(ctx)) {
		// SSL_LOG(TRACE, "match: DefinitiveObjIdComponent");
		return true;
	}
	else if (__asn1_v2_parse_DefinitiveNameAndNumberForm(ctx)) {
		// SSL_LOG(TRACE, "match: DefinitiveObjIdComponent");
		return true;
	}
	// SSL_LOG(TRACE, "no match: DefinitiveObjIdComponent");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_DefinitiveObjIdComponentList(t_asn_v2_parser_ctx *ctx)
{
	// DefinitiveObjIdComponent | DefinitiveObjIdComponent DefinitiveObjIdComponentList
	bool match = false;
	while (__asn1_v2_parse_DefinitiveObjIdComponent(ctx)) {
		match = true;
		/* */
	}
	// SSL_LOG(TRACE, (match ? "match": "no match") ": DefinitiveObjIdComponentList");
	return (match);
}

static bool __asn1_v2_parse_DefinitiveOID(t_asn_v2_parser_ctx *ctx)
{
	// "{" DefinitiveObjIdComponentList "}"
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_token_L_CURLY_BRKT(ctx) &&
		__asn1_v2_parse_DefinitiveObjIdComponentList(ctx) &&
		__asn1_v2_parse_token_R_CURLY_BRKT(ctx)
	) {
		// SSL_LOG(TRACE, "match: DefinitiveOID");
		return true;
	}
	else {
		// SSL_LOG(TRACE, "no match: DefinitiveOID");
		ctx->token = old_token;
		return false;
	}
}

static bool __asn1_v2_parse_IRIValue(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("value grammar");
}

static bool __asn1_v2_parse_DefinitiveOIDAndIRI(t_asn_v2_parser_ctx *ctx)
{
	// DefinitiveOID IRIValue
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_DefinitiveOID(ctx) &&
		__asn1_v2_parse_IRIValue(ctx)
	) {
		// SSL_LOG(TRACE, "match: DefinitiveOIDandIRI");
		return true;
	}
	// SSL_LOG(TRACE, "no match: DefinitiveOIDandIRI");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_DefinitiveIdentification(t_asn_v2_parser_ctx *ctx)
{
	// DefinitiveOIDandIRI | DefinitiveOID | empty
	if (__asn1_v2_parse_DefinitiveOIDAndIRI(ctx)) {
		/* */
	}
	else if (__asn1_v2_parse_DefinitiveOID(ctx)) {
		/* */
	}
	// SSL_LOG(TRACE, "match: DefinitiveIdentification");
	return true;
}

static bool __asn1_v2_parse_ModuleIdentifier(t_asn_v2_parser_ctx *ctx)
{
	// modulereference DefinitiveIdentification
	t_asn_v2_token old_token = ctx->token;
	if (__asn1_v2_parse_upper_first_name(ctx)) {
		t_asn_v2_token name_token = ctx->token;
		if (__asn1_v2_parse_DefinitiveIdentification(ctx)) {
			// SSL_LOG(TRACE, "match: ModuleIdentifier");
			return true;
		}
	}
	// SSL_LOG(TRACE, "no match: ModuleIdentifier");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_Reference(t_asn_v2_parser_ctx *ctx)
{
	// typereference | valuereference | objectclassreference | objectreference | objectsetreference
	if (__asn1_v2_parse_typereference(ctx)) {
		// SSL_LOG(TRACE, "match: Reference");
		return true;
	}
	else if (__asn1_v2_parse_valuereference(ctx)) {
		// SSL_LOG(TRACE, "match: Reference");
		return true;
	}
	else if (__asn1_v2_parse_objectclassreference(ctx)) {
		// SSL_LOG(TRACE, "match: Reference");
		return true;
	}
	else if (__asn1_v2_parse_objectreference(ctx)) {
		// SSL_LOG(TRACE, "match: Reference");
		return true;
	}
	else if (__asn1_v2_parse_objectsetreference(ctx)) {
		// SSL_LOG(TRACE, "match: Reference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: Reference");
	return false;
}

static bool __asn1_v2_parse_ParameterizedReference(t_asn_v2_parser_ctx *ctx)
{
	// Reference | Reference "{" "}"
	t_asn_v2_token old_token = ctx->token;
	if (__asn1_v2_parse_Reference(ctx)) {
		// SSL_LOG(TRACE, "match: ParameterizedReference");
		return true;
	}
	else if (
		__asn1_v2_parse_Reference(ctx) &&
		__asn1_v2_parse_token_L_CURLY_BRKT(ctx) &&
		__asn1_v2_parse_token_R_CURLY_BRKT(ctx)
	) {
		// SSL_LOG(TRACE, "match: ParameterizedReference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: ParameterizedReference");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_Symbol(t_asn_v2_parser_ctx *ctx)
{
	// Reference | ParameterizedReference
	if (__asn1_v2_parse_Reference(ctx)) {
		// SSL_LOG(TRACE, "match: Symbol");
		return true;
	}
	else if (__asn1_v2_parse_ParameterizedReference(ctx)) {
		// SSL_LOG(TRACE, "match: Symbol");
		return true;
	}
	// SSL_LOG(TRACE, "no match: Symbol");
	return false;
}

static bool __asn1_v2_parse_SymbolList(t_asn_v2_parser_ctx *ctx)
{
	// Symbol | SymbolList "," Symbol
	bool match = false;
	if (__asn1_v2_parse_Symbol(ctx)) {
		match = true;
		t_asn_v2_token old_token = ctx->token;
		while (
			__asn1_v2_parse_token_COMMA(ctx) &&
			__asn1_v2_parse_Symbol(ctx)
		) {
			old_token = ctx->token;
		}
		ctx->token = old_token;
	}
	return (match);
}

static bool __asn1_v2_parse_SymbolsExported(t_asn_v2_parser_ctx *ctx)
{
	// SymbolList | empty
	if (__asn1_v2_parse_SymbolList(ctx)) {
		/* */
	}
	// SSL_LOG(TRACE, "match: SymbolsExported");
	return true;
}

static bool __asn1_v2_parse_Exports(t_asn_v2_parser_ctx *ctx)
{
	// EXPORTS SymbolsExported ";" | EXPORTS ALL ";" | empty
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_keyword_EXPORTS(ctx) &&
		__asn1_v2_parse_SymbolsExported(ctx) &&
		__asn1_v2_parse_token_SEMICOLON(ctx)
	) {
		/* */
	}
	else if (
		__asn1_v2_parse_keyword_EXPORTS(ctx) &&
		__asn1_v2_parse_keyword_ALL(ctx) &&
		__asn1_v2_parse_token_SEMICOLON(ctx)
	) {
		/* */
	}
	// SSL_LOG(TRACE, "match: Exports");
	ctx->token = old_token;
	return true;
}

static bool __asn1_v2_parse_ObjectIdentifierValue(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("value grammar");
}

static bool __asn1_v2_parse_AssignedIdentifier(t_asn_v2_parser_ctx *ctx)
{
	// ObjectIdentifierValue | DefinedValue | empty
	if (__asn1_v2_parse_ObjectIdentifierValue(ctx)) {
		/* */
	}
	else if (__asn1_v2_parse_DefinedValue(ctx)) {
		/* */
	}
	// SSL_LOG(TRACE, "match: GlobalModuleReference");
	return true;
}

static bool __asn1_v2_parse_GlobalModuleReference(t_asn_v2_parser_ctx *ctx)
{
	// modulereference AssignedIdentifier
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_modulereference(ctx) &&
		__asn1_v2_parse_AssignedIdentifier(ctx)
	) {
		// SSL_LOG(TRACE, "match: GlobalModuleReference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: GlobalModuleReference");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_SelectionOption(t_asn_v2_parser_ctx *ctx)
{
	// empty | WITH "SUCCESSORS" | WITH "DESCENDANTS"
	t_asn_v2_token old_token = ctx->token;
	if (__asn1_v2_parse_keyword_WITH(ctx) &&
		__asn1_v2_parse_token_DOUBLE_QUOTE(ctx) &&
		__asn1_v2_parse_upper_name(ctx) &&
		__asn1_v2_parse_token_DOUBLE_QUOTE(ctx)
	) {
		/* */
	}
	else {
		ctx->token = old_token;
	}
	// SSL_LOG(TRACE, "match: SelectionOption");
	return true;
}

static bool __asn1_v2_parse_SymbolsFromModule(t_asn_v2_parser_ctx *ctx)
{
	// SymbolList FROM GlobalModuleReference SelectionOption
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_SymbolList(ctx) &&
		__asn1_v2_parse_keyword_FROM(ctx) &&
		__asn1_v2_parse_GlobalModuleReference(ctx) &&
		__asn1_v2_parse_SelectionOption(ctx)
	) {
		// SSL_LOG(TRACE, "match: SymbolsFromModule");
		return true;
	}
	// SSL_LOG(TRACE, "no match: SymbolsFromModule");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_SymbolsFromModuleList(t_asn_v2_parser_ctx *ctx)
{
	// SymbolsFromModule | SymbolsFromModuleList SymbolsFromModule
	bool match = false;
	while (__asn1_v2_parse_SymbolsFromModule(ctx)) {
		match = true;
		/* */
	}
	return (match);
}

static bool __asn1_v2_parse_SymbolsImported(t_asn_v2_parser_ctx *ctx)
{
	// SymbolsFromModuleList | empty
	t_asn_v2_token old_token = ctx->token;
	if (__asn1_v2_parse_SymbolsFromModuleList(ctx)) {
		/* */
	}
	// SSL_LOG(TRACE, "match: SymbolsImported");
	ctx->token = old_token;
	return true;
}

static bool __asn1_v2_parse_Imports(t_asn_v2_parser_ctx *ctx)
{
	// IMPORTS SymbolsImported ";" | empty
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_keyword_IMPORTS(ctx) &&
		__asn1_v2_parse_SymbolsImported(ctx) &&
		__asn1_v2_parse_token_SEMICOLON(ctx)
	) {
		/* */
	}
	// SSL_LOG(TRACE, "match: Imports");
	ctx->token = old_token;
	return true;
}

static bool __asn1_v2_parse_ExternalTypeReference(t_asn_v2_parser_ctx *ctx)
{
	// modulereference "." typereference
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_modulereference(ctx) &&
		__asn1_v2_parse_token_DOT(ctx) &&
		__asn1_v2_parse_typereference(ctx)
	) {
		// SSL_LOG(TRACE, "match: ExternalTypeReference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: ExternalTypeReference");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_ExternalValueReference(t_asn_v2_parser_ctx *ctx)
{
	// modulereference "." valuereference
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_modulereference(ctx) &&
		__asn1_v2_parse_token_DOT(ctx) &&
		__asn1_v2_parse_valuereference(ctx)
	) {
		// SSL_LOG(TRACE, "match: ExternalValueReference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: ExternalValueReference");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_ItemId(t_asn_v2_parser_ctx *ctx)
{
	// ItemSpec
	if (__asn1_v2_parse_ItemSpec(ctx)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ItemSpec(t_asn_v2_parser_ctx *ctx)
{
	// typereference | ItemId "." ComponentId
	TODO("__asn1_v2_parse_ItemSpec");
}

static bool __asn1_v2_parse_AbsoluteReference(t_asn_v2_parser_ctx *ctx)
{
	// "@" ModuleIdentifier "." ItemSpec
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_token_AT(ctx) &&
		__asn1_v2_parse_ModuleIdentifier(ctx) &&
		__asn1_v2_parse_token_DOT(ctx) &&
		__asn1_v2_parse_ItemSpec(ctx)
	) {
		// SSL_LOG(TRACE, "match: AbsoluteReference");
		return true;
	}
	// SSL_LOG(TRACE, "no match: AbsoluteReference");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_xmlasn1typename(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("xml grammar");
}

static bool __asn1_v2_parse_NonParametrizedTypeName(t_asn_v2_parser_ctx *ctx)
{
	// ExternalTypeReference | typereference | xmlasn1typename
	if (__asn1_v2_parse_ExternalTypeReference(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_typereference(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_xmlasn1typename(ctx)) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: NonParameterizedTypeName");
	return false;
}

static bool __asn1_v2_parse_ParameterizedType(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("ParameterizedType");
}

static bool __asn1_v2_parse_ParameterizedValueSetType(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("ParameterizedValueSetType");
}

static bool __asn1_v2_parse_DefinedType(t_asn_v2_parser_ctx *ctx)
{
	// ExternalTypeReference | typereference | ParameterizedType | ParameterizedValueSetType
	if (__asn1_v2_parse_ExternalTypeReference(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_typereference(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_ParameterizedType(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_ParameterizedValueSetType(ctx)) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: DefinedType");
	return false;
}

static bool __asn1_v2_parse_ParameterizedValue(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("ParameterizedValue");
}

static bool __asn1_v2_parse_DefinedValue(t_asn_v2_parser_ctx *ctx)
{
	// ExternalValueReference | valuereference | ParameterizedValue
	if (__asn1_v2_parse_ExternalValueReference(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_valuereference(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_ParameterizedValue(ctx)) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: DefinedValue");
	return false;
}

static bool __asn1_v2_parse_Type(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("type grammar");
}

static bool __asn1_v2_parse_Value(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("value grammar");
}

static bool __asn1_v2_parse_TypeAssignment(t_asn_v2_parser_ctx *ctx)
{
	// typereference "::=" Type
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_typereference(ctx) &&
		__asn1_v2_parse_token_ASSIGNMENT(ctx) &&
		__asn1_v2_parse_Type(ctx)
	) {
		// SSL_LOG(TRACE, "match: TypeAssignment");
		return true;
	}
	// SSL_LOG(TRACE, "no match: TypeAssignment");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_ValueAssignment(t_asn_v2_parser_ctx *ctx)
{
	// valuereference Type "::=" Value
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_valuereference(ctx) &&
		__asn1_v2_parse_Type(ctx) &&
		__asn1_v2_parse_token_ASSIGNMENT(ctx) &&
		__asn1_v2_parse_Value(ctx)
	) {
		// SSL_LOG(TRACE, "match: ValueAssignment");
		return true;
	}
	// SSL_LOG(TRACE, "no match: ValueAssignment");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_XMLTypedValue(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("xml grammar");
}

static bool __asn1_v2_parse_XMLValueAssignment(t_asn_v2_parser_ctx *ctx)
{
	// valuereference "::=" XMLTypedValue
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_valuereference(ctx) &&
		__asn1_v2_parse_token_ASSIGNMENT(ctx) &&
		__asn1_v2_parse_XMLTypedValue(ctx)
	) {
		// SSL_LOG(TRACE, "match: XMLValueAssignment");
		return true;
	}
	// SSL_LOG(TRACE, "no match: XMLValueAssignment");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_ElementSetSpecs(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("ElementSetSpecs");
}

static bool __asn1_v2_parse_ValueSet(t_asn_v2_parser_ctx *ctx)
{
	// "{" ElementSetSpecs "}"
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_token_L_CURLY_BRKT(ctx) &&
		__asn1_v2_parse_ElementSetSpecs(ctx) &&
		__asn1_v2_parse_token_R_CURLY_BRKT(ctx)
	) {
		// SSL_LOG(TRACE, "match: ValueSet");
		return true;
	}
	// SSL_LOG(TRACE, "no match: ValueSet");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_ValueSetTypeAssignment(t_asn_v2_parser_ctx *ctx)
{
	// typereference Type "::=" ValueSet
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_typereference(ctx) &&
		__asn1_v2_parse_Type(ctx) &&
		__asn1_v2_parse_token_ASSIGNMENT(ctx) &&
		__asn1_v2_parse_ValueSet(ctx)
	) {
		// SSL_LOG(TRACE, "match: ValueSetTypeAssignment");
		return true;
	}
	// SSL_LOG(TRACE, "no match: ValueSetTypeAssignment");
	ctx->token = old_token;
	return false;
}

static bool __asn1_v2_parse_ObjectClassAssignment(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("ObjectClassAssignment");
}

static bool __asn1_v2_parse_ObjectAssignment(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("ObjectAssignment");
}

static bool __asn1_v2_parse_ObjectSetAssignment(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("ObjectSetAssignment");
}

static bool __asn1_v2_parse_ParameterizedAssignment(t_asn_v2_parser_ctx *ctx)
{
	NOT_IMPLEMENTED("ParameterizedAssignment");
}

static bool __asn1_v2_parse_Assignment(t_asn_v2_parser_ctx *ctx)
{
	// TypeAssignment | ValueAssignment | XMLValueAssignment | ValueSetTypeAssignment |
	// ObjectClassAssignment | ObjectAssignment | ObjectSetAssignment | ParameterizedAssignment
	if (__asn1_v2_parse_TypeAssignment(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_ValueAssignment(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_XMLValueAssignment(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_ValueSetTypeAssignment(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_ObjectClassAssignment(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_ObjectAssignment(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_ObjectSetAssignment(ctx)) {
		return true;
	}
	else if (__asn1_v2_parse_ParameterizedAssignment(ctx)) {
		return true;
	}
	// SSL_LOG(TRACE, "no match: Assignment");
	return false;
}

static bool __asn1_v2_parse_AssignmentList(t_asn_v2_parser_ctx *ctx)
{
	// Assignment | AssignmentList Assignment
	bool match = false;
	while (__asn1_v2_parse_Assignment(ctx)) {
		match = true;
		/* */
	}
	return (match);
}

static bool __asn1_v2_parse_ModuleBody(t_asn_v2_parser_ctx *ctx)
{
	// Exports Imports AssignmentList | empty
	t_asn_v2_token old_token = ctx->token;
	if (
		__asn1_v2_parse_Exports(ctx) &&
		__asn1_v2_parse_Imports(ctx) &&
		__asn1_v2_parse_AssignmentList(ctx)
	) {
		/* */
	}
	// SSL_LOG(TRACE, "match: ModuleBody");
	ctx->token = old_token;
	return true;
}

static bool __asn1_v2_parse_Module(t_asn_v2_parser_ctx *ctx)
{
	t_asn_v2_token old_token = ctx->token;

	if (!__asn1_v2_parse_ModuleIdentifier(ctx)) goto label_error;
	if (!__asn1_v2_parse_keyword_DEFINITIONS(ctx)) goto label_error;

	if (__asn1_v2_parse_EncodingReferenceDefault(ctx)) {};
	if (__asn1_v2_parse_TagDefault(ctx)) {};
	if (__asn1_v2_parse_ExtensionDefault(ctx)) {};

	if (!__asn1_v2_parse_token_ASSIGNMENT(ctx)) goto label_error;
	if (!__asn1_v2_parse_keyword_BEGIN(ctx)) goto label_error;
	if (!__asn1_v2_parse_ModuleBody(ctx)) goto label_error;
	if (!__asn1_v2_parse_keyword_END(ctx)) goto label_error;

	// SSL_LOG(TRACE, "match: ExtensionDefault");
	return true;

label_error:
	// SSL_LOG(TRACE, "no match: ModuleDefinition");
	ctx->token = old_token;
	return false;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static t_asn_v2_token *__asn1_v2_token_create(void)
{
	t_asn_v2_token *token = NULL;
	SSL_ALLOC(token, sizeof(t_asn_v2_token));
	*token = (t_asn_v2_token){0};
	return (token);
}

static void __asn1_v2_token_delete(t_asn_v2_token *token)
{
	*token = (t_asn_v2_token){0};
	SSL_FREE(token);
}

int asn1_v2_parse(const char *s, size_t slen)
{
	if (NULL == s) return (SSL_ERR);

	t_asn_v2_parser_ctx ctx = { .token.s = s, .token.slen = slen };

	if (!__asn1_v2_parse_Module(&ctx)) {
		return (SSL_ERR);
	}
	if (!ctx.token.done) {
		SSL_LOG(ERROR, "parsing is not complete");
		return (SSL_ERR);
	}
	return (SSL_OK);
}
