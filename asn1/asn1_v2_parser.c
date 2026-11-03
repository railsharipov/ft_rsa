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
	ASN_V2_TOKEN_KIND_TSTRING,
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
	ASN_V2_TOKEN_KIND_INT_UNI_LBL,
	ASN_V2_TOKEN_KIND_NON_INT_UNI_LBL,
} t_asn_v2_token_kind;

static const char *__asn1_v2_get_token_kind_name(t_asn_v2_token_kind kind)
{
	switch (kind) {
	case ASN_V2_TOKEN_KIND_NONE: 			return "NONE";
	case ASN_V2_TOKEN_KIND_COMMENT: 		return "COMMENT";
	case ASN_V2_TOKEN_KIND_BSTRING: 		return "BSTRING";
	case ASN_V2_TOKEN_KIND_HSTRING: 		return "HSTRING";
	case ASN_V2_TOKEN_KIND_CSTRING: 		return "CSTRING";
	case ASN_V2_TOKEN_KIND_TSTRING: 		return "TSTRING";
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
	case ASN_V2_TOKEN_KIND_INT_UNI_LBL:		return "integerUnicodeLabel";
	case ASN_V2_TOKEN_KIND_NON_INT_UNI_LBL:	return "non-integerUnicodeLabel";
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

typedef struct s_asn_v2_data {
	void *p;
} t_asn_v2_data;

typedef struct s_asn_v2_parser_ctx {
	t_asn_v2_token token;
	t_list ops;
	t_list funcs;
	bool abort;
} t_asn_v2_parser_ctx;

typedef bool (*t_func_asn_v2_parse)(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);

typedef enum e_asn_v2_parser_op_kind {
	ASN_V2_PARSER_OP_KIND_CALL = 0,
	ASN_V2_PARSER_OP_KIND_AND,
	ASN_V2_PARSER_OP_KIND_OR,
} t_asn_v2_parser_op_kind;

static const char *__asn1_v2_get_op_kind_name(t_asn_v2_parser_op_kind kind)
{
	switch (kind) {
	case ASN_V2_PARSER_OP_KIND_CALL: return ("CALL");
	case ASN_V2_PARSER_OP_KIND_AND: return ("AND");
	case ASN_V2_PARSER_OP_KIND_OR: return ("OR");
	default: return ("unknown");
	}
}

typedef struct s_asn_v2_parser_op {
	t_asn_v2_parser_op_kind kind;
} t_asn_v2_parser_op;

typedef struct s_asn_v2_parser_func {
	t_func_asn_v2_parse call;
	const char *name;
} t_asn_v2_parser_func;

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
	{"ANY",              "ANY"},
	{"DEFINED",          "DEFINED"},
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

const char *__asn1_v2_token_dumpb(t_asn_v2_token token, char *buf, size_t bufsize)
{
	size_t len = MIN(bufsize, token.end - token.start+1);
	ft_strncpy(buf, token.s + token.start, len);
	buf[len-1] = 0;
	return (buf);
}

bool __asn1_v2_is_bound(char c)
{
	if (c == '\0') return true;
	if (ft_iseolws(c)) return true;
	for (size_t i = 0; i < SINGLE_CHAR_LEX_COUNT; i++) {
		if (c == SINGLE_CHAR_LEXES[i].chr) return true;
	}
	return false;
}

bool __asn1_v2_token_next(t_asn_v2_token *token)
{
	assert(NULL != token);
	t_asn_v2_token t = *token;
	if (t.slen == 0 || t.end >= t.slen) return false;
	t.start = t.end;
	t.end = t.slen;

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

	if (ft_isalnum(s[0]) || s[0] == '.' || s[0] == '_' || s[0] == '~') {
		bool has_at_least_one_non_digit = false;
		bool has_hyphen_at_idx_2 = false;
		bool has_hyphen_at_idx_3 = false;
		for (size_t i = 0; i < slen; i++) {
			if (!ft_isdigit(s[i])) has_at_least_one_non_digit = true;
			if (i == 2 && s[i] == '-') has_hyphen_at_idx_2 = true;
			if (i == 3 && s[i] == '-') has_hyphen_at_idx_3 = true;
			if (!(ft_isalnum_or_hyphen(s[i]) || s[i] == '.' || s[i] == '_' || s[i] == '~')) {
				if (__asn1_v2_is_bound(s[i])) {
					if (has_at_least_one_non_digit && !(has_hyphen_at_idx_2 && has_hyphen_at_idx_3)) {
						t.kind = ASN_V2_TOKEN_KIND_NON_INT_UNI_LBL;
						t.end = t.start + i;
						goto label_ok;
					}
				}
				break;
			}
		}
	}

	SSL_LOG(TRACE, "no token: pos: %zu", token->end);
	return false;

label_ok:
	// char cbuf[512];
	// SSL_LOG(TRACE, "next token: pos: % 5zu: %-12s: %s", t.start, __asn1_v2_get_token_kind_name(t.kind), __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	*token = t;
	if (token->kind == ASN_V2_TOKEN_KIND_COMMENT) {
		return (__asn1_v2_token_next(token));
	}
	return true;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static t_asn_v2_parser_op *__asn1_v2_parser_op_create(t_asn_v2_parser_op_kind kind)
{
	t_asn_v2_parser_op *op = NULL;
	SSL_ALLOC(op, sizeof(t_asn_v2_parser_op));
	op->kind = kind;
	return (op);
}

static void __asn1_v2_parser_op_delete(t_asn_v2_parser_op *op)
{
	SSL_FREE(op);
}

static t_asn_v2_parser_func *__asn1_v2_parser_func_create(t_func_asn_v2_parse f, const char *name)
{
	t_asn_v2_parser_func *func = NULL;
	SSL_ALLOC(func, sizeof(t_asn_v2_parser_func));
	func->call = f;
	func->name = name;
	return (func);
}

static void __asn1_v2_parser_func_delete(t_asn_v2_parser_func *func)
{
	SSL_FREE(func);
}

static void __asn1_v2_parser_op_push_call(t_asn_v2_parser_ctx *ctx, t_func_asn_v2_parse func, const char *name)
{
	ft_list_append_content(&ctx->ops, __asn1_v2_parser_op_create(ASN_V2_PARSER_OP_KIND_CALL));
	ft_list_append_content(&ctx->funcs, __asn1_v2_parser_func_create(func, name));
}

static void __asn1_v2_parser_op_push_and(t_asn_v2_parser_ctx *ctx)
{
	ft_list_append_content(&ctx->ops, __asn1_v2_parser_op_create(ASN_V2_PARSER_OP_KIND_AND));
}

static void __asn1_v2_parser_op_push_or(t_asn_v2_parser_ctx *ctx)
{
	ft_list_append_content(&ctx->ops, __asn1_v2_parser_op_create(ASN_V2_PARSER_OP_KIND_OR));
}

static inline const char *__asn1_v2_strip_parse_func_prefix(const char *name)
{
	const char prefix[16] = "__asn1_v2_parse_";
    return (ft_strncmp(name, prefix, sizeof(prefix)) == 0 ? name + sizeof(prefix) : name);
}

static t_list call_history = {0};
static t_list call_history = {0};
static void __asn1_v2_call_history_init(size_t slen)
{
	
}

static bool __asn1_v2_parser_op_run(t_asn_v2_parser_ctx *ctx, const char *name, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// {
	// 	t_ostring ostring = {0};
	// 	t_list_next op_next = {0};
	// 	t_list_next func_next = {0};
	// 	void *content = NULL;
	// 	while (ft_list_next_content(&ctx->ops, &op_next, &content)) {
	// 		t_asn_v2_parser_op *op = content;
	// 		switch (op->kind) {
	// 		case ASN_V2_PARSER_OP_KIND_CALL: {
	// 			ft_list_next_content(&ctx->funcs, &func_next, &content);
	// 			t_asn_v2_parser_func *func = content;
	// 			ft_ostr_appendf(&ostring, "    CALL: %s\n", func->name);
	// 			break;
	// 		}
	// 		case ASN_V2_PARSER_OP_KIND_AND: {
	// 			ft_ostr_appendf(&ostring, "    AND\n");
	// 			break;
	// 		}
	// 		case ASN_V2_PARSER_OP_KIND_OR: {
	// 			ft_ostr_appendf(&ostring, "    OR\n");
	// 			break;
	// 		}}
	// 	}
	// 	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	// 	SSL_LOG(TRACE, "CALL STACK:\n%s", dumps);
	// 	SSL_FREE(dumps);
	// }

	t_asn_v2_parser_op *op = ft_list_pop_content(&ctx->ops);
	if (NULL == op) return false;

	switch (op->kind) {
	case ASN_V2_PARSER_OP_KIND_CALL: {
		t_asn_v2_parser_func *func = ft_list_pop_content(&ctx->funcs);
		if (ctx->abort) return false;
		SSL_LOG(TRACE, "CALL : %s => %s", __asn1_v2_strip_parse_func_prefix(name), __asn1_v2_strip_parse_func_prefix(func->name));
		t_asn_v2_parser_ctx call_ctx = {0};
		if (func->call(&call_ctx, token, data)) {
			return true;
		}
		break;
	}
	case ASN_V2_PARSER_OP_KIND_AND: {
		t_asn_v2_token old_token = *token;
		bool b1 = __asn1_v2_parser_op_run(ctx, name, token, data);
		if (!ctx->abort) ctx->abort = !b1;
		bool b2 = __asn1_v2_parser_op_run(ctx, name, token, data);
		ctx->abort = false;
		if (b1 && b2) {
			return true;
		}
		*token = old_token;
		break;
	}
	case ASN_V2_PARSER_OP_KIND_OR: {
		bool b1 = __asn1_v2_parser_op_run(ctx, name, token, data);
		if (!ctx->abort) ctx->abort = b1;
		bool b2 = __asn1_v2_parser_op_run(ctx, name, token, data);
		ctx->abort = false;
		if (b1 || b2) {
			return true;
		}
		break;
	}}
	// ft_list_prepend_content(&ctx->ops, op);
	return false;
}

#define PUSH_OP_CALL(ctx, fn) \
	((void)__asn1_v2_parser_op_push_call((ctx), (fn), #fn))

#define PUSH_OP_AND(ctx, left, right) \
	((void)__asn1_v2_parser_op_push_and((ctx)), \
		(void)(left), \
		(void)(right))

#define PUSH_OP_AND_3(ctx,i1,i2,i3) \
	PUSH_OP_AND((ctx),(i1),PUSH_OP_AND((ctx),(i2),(i3)))
#define PUSH_OP_AND_4(ctx,i1,i2,i3,i4)\
	PUSH_OP_AND((ctx),(i1),PUSH_OP_AND_3((ctx),(i2),(i3),(i4)))
#define PUSH_OP_AND_5(ctx,i1,i2,i3,i4,i5)\
	PUSH_OP_AND((ctx),(i1),PUSH_OP_AND_4((ctx),(i2),(i3),(i4),(i5)))
#define PUSH_OP_AND_6(ctx,i1,i2,i3,i4,i5,i6)\
	PUSH_OP_AND((ctx),(i1),PUSH_OP_AND_5((ctx),(i2),(i3),(i4),(i5),(i6)))
#define PUSH_OP_AND_7(ctx,i1,i2,i3,i4,i5,i6,i7)\
	PUSH_OP_AND((ctx),(i1),PUSH_OP_AND_6((ctx),(i2),(i3),(i4),(i5),(i6),(i7)))
#define PUSH_OP_AND_8(ctx,i1,i2,i3,i4,i5,i6,i7,i8)\
	PUSH_OP_AND((ctx),(i1),PUSH_OP_AND_7((ctx),(i2),(i3),(i4),(i5),(i6),(i7),(i8)))
#define PUSH_OP_AND_9(ctx,i1,i2,i3,i4,i5,i6,i7,i8,i9)\
	PUSH_OP_AND((ctx),(i1),PUSH_OP_AND_8((ctx),(i2),(i3),(i4),(i5),(i6),(i7),(i8),(i9)))
#define PUSH_OP_AND_10(ctx,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10)\
	PUSH_OP_AND((ctx),(i1),PUSH_OP_AND_9((ctx),(i2),(i3),(i4),(i5),(i6),(i7),(i8),(i9),(i10)))

#define PUSH_OP_OR(ctx, left, right) \
	((void)__asn1_v2_parser_op_push_or((ctx)), \
		(void)(left), \
		(void)(right))

#define PUSH_OP_OR_3(ctx,i1,i2,i3) \
	PUSH_OP_OR((ctx),(i1),PUSH_OP_OR((ctx),(i2),(i3)))
#define PUSH_OP_OR_4(ctx,i1,i2,i3,i4)\
	PUSH_OP_OR((ctx),(i1),PUSH_OP_OR_3((ctx),(i2),(i3),(i4)))
#define PUSH_OP_OR_5(ctx,i1,i2,i3,i4,i5)\
	PUSH_OP_OR((ctx),(i1),PUSH_OP_OR_4((ctx),(i2),(i3),(i4),(i5)))
#define PUSH_OP_OR_6(ctx,i1,i2,i3,i4,i5,i6)\
	PUSH_OP_OR((ctx),(i1),PUSH_OP_OR_5((ctx),(i2),(i3),(i4),(i5),(i6)))
#define PUSH_OP_OR_7(ctx,i1,i2,i3,i4,i5,i6,i7)\
	PUSH_OP_OR((ctx),(i1),PUSH_OP_OR_6((ctx),(i2),(i3),(i4),(i5),(i6),(i7)))
#define PUSH_OP_OR_8(ctx,i1,i2,i3,i4,i5,i6,i7,i8)\
	PUSH_OP_OR((ctx),(i1),PUSH_OP_OR_7((ctx),(i2),(i3),(i4),(i5),(i6),(i7),(i8)))
#define PUSH_OP_OR_9(ctx,i1,i2,i3,i4,i5,i6,i7,i8,i9)\
	PUSH_OP_OR((ctx),(i1),PUSH_OP_OR_8((ctx),(i2),(i3),(i4),(i5),(i6),(i7),(i8),(i9)))
#define PUSH_OP_OR_10(ctx,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10)\
	PUSH_OP_OR((ctx),(i1),PUSH_OP_OR_9((ctx),(i2),(i3),(i4),(i5),(i6),(i7),(i8),(i9),(i10)))

#define RUN_OP(ctx, token, data) \
	__asn1_v2_parser_op_run((ctx), __func__, (token), (data))

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static bool __asn1_v2_parse_char(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data, char c)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_SINGLE_CHAR) {
		if (t.s[t.start] == c) {
			if (c == '\n') {
				SSL_LOG(TRACE, "match: SINGLE_CHAR '\\n'");
			} else {
				SSL_LOG(TRACE, "match: SINGLE_CHAR '%c'", c);
			}
			UNUSED(ctx);
			*token = t;
			return true;
		}
	}
	if (c == '\n') {
		SSL_LOG(TRACE, "no match: SINGLE_CHAR '\\n': %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	} else {
		SSL_LOG(TRACE, "no match: SINGLE_CHAR '%c': %s", c, __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	}
	return false;
}

static bool __asn1_v2_parse_token_L_CURLY_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)  { return (__asn1_v2_parse_char(ctx, token, data, '{')); }
static bool __asn1_v2_parse_token_R_CURLY_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)  { return (__asn1_v2_parse_char(ctx, token, data, '}')); }
static bool __asn1_v2_parse_token_L_ANGLE_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)  { return (__asn1_v2_parse_char(ctx, token, data, '<')); }
static bool __asn1_v2_parse_token_R_ANGLE_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)  { return (__asn1_v2_parse_char(ctx, token, data, '>')); }
static bool __asn1_v2_parse_token_COMMA(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_char(ctx, token, data, ',')); }
static bool __asn1_v2_parse_token_DOT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)           { return (__asn1_v2_parse_char(ctx, token, data, '.')); }
static bool __asn1_v2_parse_token_SLASH(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_char(ctx, token, data, '/')); }
static bool __asn1_v2_parse_token_L_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)        { return (__asn1_v2_parse_char(ctx, token, data, '(')); }
static bool __asn1_v2_parse_token_R_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)        { return (__asn1_v2_parse_char(ctx, token, data, ')')); }
static bool __asn1_v2_parse_token_L_SQUARE_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data) { return (__asn1_v2_parse_char(ctx, token, data, '[')); }
static bool __asn1_v2_parse_token_R_SQUARE_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data) { return (__asn1_v2_parse_char(ctx, token, data, ']')); }
static bool __asn1_v2_parse_token_HYPHEN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)        { return (__asn1_v2_parse_char(ctx, token, data, '-')); }
static bool __asn1_v2_parse_token_COLON(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_char(ctx, token, data, ':')); }
static bool __asn1_v2_parse_token_EQUAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_char(ctx, token, data, '=')); }
static bool __asn1_v2_parse_token_DOUBLE_QUOTE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)  { return (__asn1_v2_parse_char(ctx, token, data, '"')); }
static bool __asn1_v2_parse_token_QUOTE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_char(ctx, token, data, '\'')); }
static bool __asn1_v2_parse_token_SEMICOLON(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)     { return (__asn1_v2_parse_char(ctx, token, data, ';')); }
static bool __asn1_v2_parse_token_AT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)            { return (__asn1_v2_parse_char(ctx, token, data, '@')); }
static bool __asn1_v2_parse_token_PIPE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_char(ctx, token, data, '|')); }
static bool __asn1_v2_parse_token_BANG(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_char(ctx, token, data, '!')); }
static bool __asn1_v2_parse_token_CARET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_char(ctx, token, data, '^')); }

static bool __asn1_v2_parse_keyword(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data, const char *kw)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_KEYWORD) {
		const char *s = t.s + t.start;
		size_t slen = t.end - t.start;
		size_t kwlen = ft_strlen(kw);
		if (kwlen == slen && ft_strneq(s, kw, kwlen)) {
			SSL_LOG(TRACE, "match: KEYWORD '%s'", kw);
			UNUSED(ctx);
			*token = t;
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: KEYWORD '%s': %s", kw, __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_keyword_ABSENT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)           { return (__asn1_v2_parse_keyword(ctx, token, data, "ABSENT")); }
static bool __asn1_v2_parse_keyword_ENCODED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_keyword(ctx, token, data, "ENCODED")); }
static bool __asn1_v2_parse_keyword_INTERSECTION(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)     { return (__asn1_v2_parse_keyword(ctx, token, data, "INTERSECTION")); }
static bool __asn1_v2_parse_keyword_SEQUENCE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_keyword(ctx, token, data, "SEQUENCE")); }
static bool __asn1_v2_parse_keyword_ABSTRACT_SYNTAX(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)  { return (__asn1_v2_parse_keyword(ctx, token, data, "ABSTRACT-SYNTAX")); }
static bool __asn1_v2_parse_keyword_ENCODING_CONTROL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data) { return (__asn1_v2_parse_keyword(ctx, token, data, "ENCODING-CONTROL")); }
static bool __asn1_v2_parse_keyword_ISO646String(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)     { return (__asn1_v2_parse_keyword(ctx, token, data, "ISO646String")); }
static bool __asn1_v2_parse_keyword_SET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)              { return (__asn1_v2_parse_keyword(ctx, token, data, "SET")); }
static bool __asn1_v2_parse_keyword_ALL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)              { return (__asn1_v2_parse_keyword(ctx, token, data, "ALL")); }
static bool __asn1_v2_parse_keyword_END(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)              { return (__asn1_v2_parse_keyword(ctx, token, data, "END")); }
static bool __asn1_v2_parse_keyword_MAX(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)              { return (__asn1_v2_parse_keyword(ctx, token, data, "MAX")); }
static bool __asn1_v2_parse_keyword_SETTINGS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_keyword(ctx, token, data, "SETTINGS")); }
static bool __asn1_v2_parse_keyword_APPLICATION(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)      { return (__asn1_v2_parse_keyword(ctx, token, data, "APPLICATION")); }
static bool __asn1_v2_parse_keyword_ENUMERATED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)       { return (__asn1_v2_parse_keyword(ctx, token, data, "ENUMERATED")); }
static bool __asn1_v2_parse_keyword_MIN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)              { return (__asn1_v2_parse_keyword(ctx, token, data, "MIN")); }
static bool __asn1_v2_parse_keyword_SIZE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)             { return (__asn1_v2_parse_keyword(ctx, token, data, "SIZE")); }
static bool __asn1_v2_parse_keyword_AUTOMATIC(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)        { return (__asn1_v2_parse_keyword(ctx, token, data, "AUTOMATIC")); }
static bool __asn1_v2_parse_keyword_EXCEPT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)           { return (__asn1_v2_parse_keyword(ctx, token, data, "EXCEPT")); }
static bool __asn1_v2_parse_keyword_MINUS_INFINITY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)   { return (__asn1_v2_parse_keyword(ctx, token, data, "MINUS-INFINITY")); }
static bool __asn1_v2_parse_keyword_STRING(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)           { return (__asn1_v2_parse_keyword(ctx, token, data, "STRING")); }
static bool __asn1_v2_parse_keyword_BEGIN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)            { return (__asn1_v2_parse_keyword(ctx, token, data, "BEGIN")); }
static bool __asn1_v2_parse_keyword_EXPLICIT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_keyword(ctx, token, data, "EXPLICIT")); }
static bool __asn1_v2_parse_keyword_NOT_A_NUMBER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)     { return (__asn1_v2_parse_keyword(ctx, token, data, "NOT-A-NUMBER")); }
static bool __asn1_v2_parse_keyword_SYNTAX(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)           { return (__asn1_v2_parse_keyword(ctx, token, data, "SYNTAX")); }
static bool __asn1_v2_parse_keyword_BIT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)              { return (__asn1_v2_parse_keyword(ctx, token, data, "BIT")); }
static bool __asn1_v2_parse_keyword_EXPORTS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_keyword(ctx, token, data, "EXPORTS")); }
static bool __asn1_v2_parse_keyword_NULL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)             { return (__asn1_v2_parse_keyword(ctx, token, data, "NULL")); }
static bool __asn1_v2_parse_keyword_T61String(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)        { return (__asn1_v2_parse_keyword(ctx, token, data, "T61String")); }
static bool __asn1_v2_parse_keyword_BMPString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)        { return (__asn1_v2_parse_keyword(ctx, token, data, "BMPString")); }
static bool __asn1_v2_parse_keyword_EXTENSIBILITY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)    { return (__asn1_v2_parse_keyword(ctx, token, data, "EXTENSIBILITY")); }
static bool __asn1_v2_parse_keyword_NumericString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)    { return (__asn1_v2_parse_keyword(ctx, token, data, "NumericString")); }
static bool __asn1_v2_parse_keyword_TAGS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)             { return (__asn1_v2_parse_keyword(ctx, token, data, "TAGS")); }
static bool __asn1_v2_parse_keyword_BOOLEAN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_keyword(ctx, token, data, "BOOLEAN")); }
static bool __asn1_v2_parse_keyword_EXTERNAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_keyword(ctx, token, data, "EXTERNAL")); }
static bool __asn1_v2_parse_keyword_OBJECT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)           { return (__asn1_v2_parse_keyword(ctx, token, data, "OBJECT")); }
static bool __asn1_v2_parse_keyword_TeletexString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)    { return (__asn1_v2_parse_keyword(ctx, token, data, "TeletexString")); }
static bool __asn1_v2_parse_keyword_BY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)               { return (__asn1_v2_parse_keyword(ctx, token, data, "BY")); }
static bool __asn1_v2_parse_keyword_FALSE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)            { return (__asn1_v2_parse_keyword(ctx, token, data, "FALSE")); }
static bool __asn1_v2_parse_keyword_ObjectDescriptor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data) { return (__asn1_v2_parse_keyword(ctx, token, data, "ObjectDescriptor")); }
static bool __asn1_v2_parse_keyword_TIME(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)             { return (__asn1_v2_parse_keyword(ctx, token, data, "TIME")); }
static bool __asn1_v2_parse_keyword_CHARACTER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)        { return (__asn1_v2_parse_keyword(ctx, token, data, "CHARACTER")); }
static bool __asn1_v2_parse_keyword_FROM(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)             { return (__asn1_v2_parse_keyword(ctx, token, data, "FROM")); }
static bool __asn1_v2_parse_keyword_OCTET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)            { return (__asn1_v2_parse_keyword(ctx, token, data, "OCTET")); }
static bool __asn1_v2_parse_keyword_TIME_OF_DAY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)      { return (__asn1_v2_parse_keyword(ctx, token, data, "TIME-OF-DAY")); }
static bool __asn1_v2_parse_keyword_CHOICE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)           { return (__asn1_v2_parse_keyword(ctx, token, data, "CHOICE")); }
static bool __asn1_v2_parse_keyword_GeneralizedTime(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)  { return (__asn1_v2_parse_keyword(ctx, token, data, "GeneralizedTime")); }
static bool __asn1_v2_parse_keyword_OF(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)               { return (__asn1_v2_parse_keyword(ctx, token, data, "OF")); }
static bool __asn1_v2_parse_keyword_TRUE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)             { return (__asn1_v2_parse_keyword(ctx, token, data, "TRUE")); }
static bool __asn1_v2_parse_keyword_CLASS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)            { return (__asn1_v2_parse_keyword(ctx, token, data, "CLASS")); }
static bool __asn1_v2_parse_keyword_GeneralString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)    { return (__asn1_v2_parse_keyword(ctx, token, data, "GeneralString")); }
static bool __asn1_v2_parse_keyword_OID_IRI(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_keyword(ctx, token, data, "OID-IRI")); }
static bool __asn1_v2_parse_keyword_TYPE_IDENTIFIER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)  { return (__asn1_v2_parse_keyword(ctx, token, data, "TYPE-IDENTIFIER")); }
static bool __asn1_v2_parse_keyword_COMPONENT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)        { return (__asn1_v2_parse_keyword(ctx, token, data, "COMPONENT")); }
static bool __asn1_v2_parse_keyword_GraphicString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)    { return (__asn1_v2_parse_keyword(ctx, token, data, "GraphicString")); }
static bool __asn1_v2_parse_keyword_OPTIONAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_keyword(ctx, token, data, "OPTIONAL")); }
static bool __asn1_v2_parse_keyword_UNION(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)            { return (__asn1_v2_parse_keyword(ctx, token, data, "UNION")); }
static bool __asn1_v2_parse_keyword_COMPONENTS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)       { return (__asn1_v2_parse_keyword(ctx, token, data, "COMPONENTS")); }
static bool __asn1_v2_parse_keyword_IA5String(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)        { return (__asn1_v2_parse_keyword(ctx, token, data, "IA5String")); }
static bool __asn1_v2_parse_keyword_PATTERN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_keyword(ctx, token, data, "PATTERN")); }
static bool __asn1_v2_parse_keyword_UNIQUE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)           { return (__asn1_v2_parse_keyword(ctx, token, data, "UNIQUE")); }
static bool __asn1_v2_parse_keyword_CONSTRAINED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)      { return (__asn1_v2_parse_keyword(ctx, token, data, "CONSTRAINED")); }
static bool __asn1_v2_parse_keyword_IDENTIFIER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)       { return (__asn1_v2_parse_keyword(ctx, token, data, "IDENTIFIER")); }
static bool __asn1_v2_parse_keyword_PDV(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)              { return (__asn1_v2_parse_keyword(ctx, token, data, "PDV")); }
static bool __asn1_v2_parse_keyword_UNIVERSAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)        { return (__asn1_v2_parse_keyword(ctx, token, data, "UNIVERSAL")); }
static bool __asn1_v2_parse_keyword_CONTAINING(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)       { return (__asn1_v2_parse_keyword(ctx, token, data, "CONTAINING")); }
static bool __asn1_v2_parse_keyword_IMPLICIT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_keyword(ctx, token, data, "IMPLICIT")); }
static bool __asn1_v2_parse_keyword_PLUS_INFINITY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)    { return (__asn1_v2_parse_keyword(ctx, token, data, "PLUS-INFINITY")); }
static bool __asn1_v2_parse_keyword_UniversalString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)  { return (__asn1_v2_parse_keyword(ctx, token, data, "UniversalString")); }
static bool __asn1_v2_parse_keyword_DATE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)             { return (__asn1_v2_parse_keyword(ctx, token, data, "DATE")); }
static bool __asn1_v2_parse_keyword_IMPLIED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_keyword(ctx, token, data, "IMPLIED")); }
static bool __asn1_v2_parse_keyword_PRESENT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_keyword(ctx, token, data, "PRESENT")); }
static bool __asn1_v2_parse_keyword_UTCTime(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_keyword(ctx, token, data, "UTCTime")); }
static bool __asn1_v2_parse_keyword_DATE_TIME(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)        { return (__asn1_v2_parse_keyword(ctx, token, data, "DATE-TIME")); }
static bool __asn1_v2_parse_keyword_IMPORTS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_keyword(ctx, token, data, "IMPORTS")); }
static bool __asn1_v2_parse_keyword_PrintableString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)  { return (__asn1_v2_parse_keyword(ctx, token, data, "PrintableString")); }
static bool __asn1_v2_parse_keyword_UTF8String(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)       { return (__asn1_v2_parse_keyword(ctx, token, data, "UTF8String")); }
static bool __asn1_v2_parse_keyword_DEFAULT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_keyword(ctx, token, data, "DEFAULT")); }
static bool __asn1_v2_parse_keyword_INCLUDES(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_keyword(ctx, token, data, "INCLUDES")); }
static bool __asn1_v2_parse_keyword_PRIVATE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_keyword(ctx, token, data, "PRIVATE")); }
static bool __asn1_v2_parse_keyword_VideotexString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)   { return (__asn1_v2_parse_keyword(ctx, token, data, "VideotexString")); }
static bool __asn1_v2_parse_keyword_DEFINITIONS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)      { return (__asn1_v2_parse_keyword(ctx, token, data, "DEFINITIONS")); }
static bool __asn1_v2_parse_keyword_INSTANCE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_keyword(ctx, token, data, "INSTANCE")); }
static bool __asn1_v2_parse_keyword_REAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)             { return (__asn1_v2_parse_keyword(ctx, token, data, "REAL")); }
static bool __asn1_v2_parse_keyword_VisibleString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)    { return (__asn1_v2_parse_keyword(ctx, token, data, "VisibleString")); }
static bool __asn1_v2_parse_keyword_DURATION(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_keyword(ctx, token, data, "DURATION")); }
static bool __asn1_v2_parse_keyword_INSTRUCTIONS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)     { return (__asn1_v2_parse_keyword(ctx, token, data, "INSTRUCTIONS")); }
static bool __asn1_v2_parse_keyword_RELATIVE_OID(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)     { return (__asn1_v2_parse_keyword(ctx, token, data, "RELATIVE-OID")); }
static bool __asn1_v2_parse_keyword_WITH(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)             { return (__asn1_v2_parse_keyword(ctx, token, data, "WITH")); }
static bool __asn1_v2_parse_keyword_EMBEDDED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)         { return (__asn1_v2_parse_keyword(ctx, token, data, "EMBEDDED")); }
static bool __asn1_v2_parse_keyword_INTEGER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_keyword(ctx, token, data, "INTEGER")); }
static bool __asn1_v2_parse_keyword_RELATIVE_OID_IRI(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data) { return (__asn1_v2_parse_keyword(ctx, token, data, "RELATIVE-OID-IRI")); }
static bool __asn1_v2_parse_keyword_ANY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)              { return (__asn1_v2_parse_keyword(ctx, token, data, "ANY")); }
static bool __asn1_v2_parse_keyword_DEFINED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)          { return (__asn1_v2_parse_keyword(ctx, token, data, "DEFINED")); }

static bool __asn1_v2_parse_token_ASSIGNMENT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_ASSIGNMENT) {
		SSL_LOG(TRACE, "match: ASSIGNMENT");
		UNUSED(ctx);
		*token = t;
		return true;
	}
	SSL_LOG(TRACE, "no match: ASSIGNMENT: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_token_RANGE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_RANGE) {
		SSL_LOG(TRACE, "match: RANGE");
		UNUSED(ctx);
		*token = t;
		return true;
	}
	SSL_LOG(TRACE, "no match: RANGE: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_token_ELLIPSIS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_ELLIPSIS) {
		SSL_LOG(TRACE, "match: ELLIPSIS");
		UNUSED(ctx);
		*token = t;
		return true;
	}
	SSL_LOG(TRACE, "no match: ELLIPSIS: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_token_L_VER_BRACKET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_LVER_BRACKET) {
		SSL_LOG(TRACE, "match: LVER_BRACKET");
		UNUSED(ctx);
		*token = t;
		return true;
	}
	SSL_LOG(TRACE, "no match: LVER_BRACKET: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_token_R_VER_BRACKET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_RVER_BRACKET) {
		SSL_LOG(TRACE, "match: RVER_BRACKET");
		UNUSED(ctx);
		*token = t;
		return true;
	}
	SSL_LOG(TRACE, "no match: RVER_BRACKET: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_bstring(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_BSTRING) {
		SSL_LOG(TRACE, "match: BSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		UNUSED(ctx);
		*token = t;
		return true;
	}
	SSL_LOG(TRACE, "no match: BSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_hstring(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_HSTRING) {
		SSL_LOG(TRACE, "match: HSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		UNUSED(ctx);
		*token = t;
		return true;
	}
	SSL_LOG(TRACE, "no match: HSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_cstring(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_CSTRING) {
		SSL_LOG(TRACE, "match: CSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		UNUSED(ctx);
		*token = t;
		return true;
	}
	SSL_LOG(TRACE, "no match: CSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_tstring(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_CSTRING) {
		const char time_specific_chars[] = {
			'+', '-', ':', '.', ',', '/', 'C', 'D', 'H', 'M', 'R', 'P', 'S', 'T', 'W', 'Y', 'Z'
		};
		for (size_t i = t.start; i < t.end; i++) {
			bool is_digit = ft_isdigit(t.s[i]);
			bool is_time_char = false;
			for (size_t j = 0; j < sizeof(time_specific_chars); i++) {
				if ((is_time_char = (t.s[i] == time_specific_chars[j]))) break;
			}
			if (!is_digit && !is_time_char) goto label_error;
		}
		SSL_LOG(TRACE, "match: TSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		UNUSED(ctx);
		*token = t;
		return true;
	}

label_error:
	SSL_LOG(TRACE, "no match: TSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_number(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NUMBER) {
		SSL_LOG(TRACE, "match: NUMBER: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		UNUSED(ctx);
		*token = t;
		return true;
	}
	SSL_LOG(TRACE, "no match: NUMBER: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_realnumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_REAL_NUMBER) {
		SSL_LOG(TRACE, "match: REAL_NUMBER: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		UNUSED(ctx);
		*token = t;
		return true;
	}
	SSL_LOG(TRACE, "no match: REAL_NUMBER: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_upper_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NAME) {
		size_t i = t.start;
		while (i < t.end && ft_isalpha_upper(t.s[i])) i++;
		if (i == t.end) {
			SSL_LOG(TRACE, "match: UPPER_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
			UNUSED(ctx);
			*token = t;
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: UPPER_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_lower_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NAME) {
		size_t i = t.start;
		while (i < t.end && ft_isalpha_lower(t.s[i])) i++;
		if (i == t.end) {
			SSL_LOG(TRACE, "match: LOWER_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
			UNUSED(ctx);
			*token = t;
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: LOWER_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_upper_first_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NAME) {
		if (ft_isalpha_upper(t.s[t.start])) {
			SSL_LOG(TRACE, "match: UPPER_FIRST_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
			UNUSED(ctx);
			*token = t;
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: UPPER_FIRST_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_lower_first_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NAME) {
		if (ft_isalpha_lower(t.s[t.start])) {
			SSL_LOG(TRACE, "match: LOWER_FIRST_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
			UNUSED(ctx);
			*token = t;
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: LOWER_FIRST_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_integer_unicode_label(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NUMBER) {
		t.kind = ASN_V2_TOKEN_KIND_INT_UNI_LBL;
		SSL_LOG(TRACE, "match: integerUnicodeLabel: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		UNUSED(ctx);
		*token = t;
		return true;
	}
	SSL_LOG(TRACE, "no match: integerUnicodeLabel: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_non_integer_unicode_label(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NON_INT_UNI_LBL) {
		SSL_LOG(TRACE, "match: non-integerUnicodeLabel: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		UNUSED(ctx);
		*token = t;
		return true;
	}
	SSL_LOG(TRACE, "no match: non-integerUnicodeLabel: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static bool __asn1_v2_parse_identifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_valuereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_typereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_objectclassreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_objectreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_objectsetreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_modulereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_xmlasn1typename(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);

static bool __asn1_v2_parse_EncodingReferenceDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_TagDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExtensionDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NameForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_DefinitiveNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_DefinitiveNameAndNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_DefinitiveObjIdComponent(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_DefinitiveObjIdComponentList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_DefinitiveOID(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_IRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_DefinitiveOIDAndIRI(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_DefinitiveIdentification(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ModuleIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Reference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ParameterizedReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Symbol(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SymbolList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SymbolsExported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Exports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_AssignedIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_GlobalModuleReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SelectionOption(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SymbolsFromModule(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SymbolsFromModuleList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SymbolsImported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Imports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExternalTypeReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExternalValueReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ItemId(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ItemSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_AbsoluteReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_xmlasn1typename(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NonParametrizedTypeName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ParameterizedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ParameterizedValueSetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_DefinedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ParameterizedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_DefinedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NamedBit(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NamedBitList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_BitStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_BooleanType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_RestrictedCharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_UnrestrictedCharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_CharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_AlternativeTypeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExtensionAdditionAlternativeGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExtensionAdditionAlternative(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExtensionAdditionAlternativeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExtensionAdditionAlternatives(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_AlternativeTypeLists(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ChoiceType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_DateType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_DateTimeType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_DurationType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_EmbeddedPDVType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_EnumerationItem(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Enumeration(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExceptionIdentification(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExceptionSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Enumerations(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_EnumeratedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExternalType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_InstanceOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SignedNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NamedNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NamedNumberList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_IntegerType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_IRIType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NullType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ObjectClassFieldType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ObjectIdentifierType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_OctetStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_RealType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_RelativeIRIType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_RelativeOIDType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExtensionAndException(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_OptionalExtensionMarker(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ComponentType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ComponentTypeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_VersionNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExtensionAdditionGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExtensionAddition(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExtensionAdditionList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExtensionAdditions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ComponentTypeLists(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SequenceType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SequenceOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SetOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ClassNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Class(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_EncodingReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Tag(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_TaggedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_EncodingInstruction(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_EncodingPrefix(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_EncodingPrefixedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_PrefixedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_TimeType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_TimeOfDayType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_BuiltinType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_UsefulType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SelectionType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_TypeFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ValueSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ReferencedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SubtypeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ConstraintSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Constraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ConstrainedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ConstrainedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NamedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_IdentifierList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_BitStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLBitStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_BooleanValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLBooleanValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Tuple(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Quadruple(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_CharsDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_CharSyms(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_CharacterStringList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_RestrictedCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_UnrestrictedCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_CharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ChoiceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLChoiceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_EmbeddedPDVValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_EnumeratedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLEnumeratedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ExternalValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLExternalValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_InstanceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_IntegerValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLIntegerValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_FirstArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SubsequentArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_IRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NullValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLNullValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_OctetStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLOctetStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NameAndNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ObjIdComponents(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ObjIdComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NumericRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SpecialRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_RealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_FirstRelativeArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_RelativeIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLRelativeIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_RelativeOIDComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_RelativeOIDComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_RelativeOIDValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLRelativeOIDValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ComponentValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SequenceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLSequenceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NamedValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SequenceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLSequenceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SetValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLSetValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SetOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLSetOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_PrefixedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLPrefixedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_TimeValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLTimeValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_BuiltinValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ValueFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ReferencedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ObjectClassFieldValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NamedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLNamedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_TypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLTypedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_XMLValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SingleValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Includes(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ContainedSubtype(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_UpperEndValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_LowerEndValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_UpperEndpoint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_LowerEndpoint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ValueRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_PermittedAlphabet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_SizeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_TypeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_PresenceConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ValueConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ComponentConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_NamedConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_TypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_MultipleTypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_InnerTypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_PatternConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_PropertyAndSettingPair(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_PropertySettingsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_PropertySettings(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_DurationRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_TimePointRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_RecurrenceRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Elements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Elements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Exclusions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Intersections(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_IntersectionMark(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Intersections(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_UnionMark(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Unions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ElementSetSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ElementSetSpecs(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ValueSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ValueSetTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ObjectClassAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ObjectAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ObjectSetAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ParameterizedAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_AssignmentList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_ModuleBody(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);
static bool __asn1_v2_parse_Module(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data);

static bool __asn1_v2_parse_identifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_lower_first_name);

	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_valuereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier);

	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_typereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name);

	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_encodingreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name);

	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_objectclassreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_name);

	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_objectreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference);

	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_objectsetreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference);

	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_modulereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference);

	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EncodingReferenceDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// encodingreference INSTRUCTIONS | empty
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INSTRUCTIONS)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_TagDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// EXPLICIT TAGS | IMPLICIT TAGS | AUTOMATIC TAGS | empty
	PUSH_OP_OR_3(ctx,
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXPLICIT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TAGS)
		),
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IMPLICIT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TAGS)
		),
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_AUTOMATIC),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TAGS)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_ExtensionDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// EXTENSIBILITY IMPLIED | empty
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXTENSIBILITY),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IMPLIED)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_NameForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// identifier
	PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// number
	PUSH_OP_CALL(ctx, __asn1_v2_parse_number);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveNameAndNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// identifier "(" DefinitiveNumberForm ")"
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveObjIdComponent(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// NameForm | DefinitiveNumberForm | DefinitiveNameAndNumberForm
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NameForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveNumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveNameAndNumberForm)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveObjIdComponentList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// DefinitiveObjIdComponent | DefinitiveObjIdComponent DefinitiveObjIdComponentList
	PUSH_OP_OR(ctx,
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveObjIdComponentList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveObjIdComponent)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveObjIdComponent)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveOID(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "{" DefinitiveObjIdComponentList "}"
	PUSH_OP_AND_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveObjIdComponentList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveOIDAndIRI(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// DefinitiveOID IRIValue
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveOID),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_IRIValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveIdentification(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// DefinitiveOIDandIRI | DefinitiveOID | empty
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveOIDAndIRI),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveOID)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_ModuleIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// modulereference DefinitiveIdentification
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveIdentification)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Reference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// typereference | valuereference | objectclassreference | objectreference | objectsetreference
	PUSH_OP_OR_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectclassreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetreference)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Reference | Reference "{" "}"
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Reference),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Reference)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Symbol(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Reference | ParameterizedReference
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Reference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedReference)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Symbol | Symbol "," SymbolList
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Symbol),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Symbol)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolsExported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SymbolList | empty
	PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolList);

	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_Exports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// EXPORTS SymbolsExported ";" | EXPORTS ALL ";" | empty
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXPORTS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolsExported),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SEMICOLON)
		),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXPORTS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ALL),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SEMICOLON)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_AssignedIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ObjectIdentifierValue | DefinedValue | empty
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectIdentifierValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_GlobalModuleReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// modulereference AssignedIdentifier
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_AssignedIdentifier)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SelectionOption(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// empty | WITH "SUCCESSORS" | WITH "DESCENDANTS"
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_WITH),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_name),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_SymbolsFromModule(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SymbolList FROM GlobalModuleReference SelectionOption
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_FROM),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_GlobalModuleReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SelectionOption)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolsFromModuleList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SymbolsFromModule | SymbolsFromModuleList SymbolsFromModule
	PUSH_OP_OR(ctx,
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolsFromModule),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolsFromModuleList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolsFromModule)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolsImported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SymbolsFromModuleList | empty
	PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolsFromModuleList);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_Imports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// IMPORTS SymbolsImported ";" | empty
	PUSH_OP_AND_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IMPORTS),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolsImported),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SEMICOLON)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_ExternalTypeReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// modulereference "." typereference
	PUSH_OP_AND_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalValueReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// modulereference "." valuereference
	PUSH_OP_AND_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ItemId(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ItemSpec
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ItemSpec);

	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ItemSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// typereference | ItemId "." ComponentId
	TODO("__asn1_v2_parse_ItemSpec");
}

static bool __asn1_v2_parse_AbsoluteReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "@" ModuleIdentifier "." ItemSpec
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ModuleIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ItemSpec)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_xmlasn1typename(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("xml grammar");
}

static bool __asn1_v2_parse_NonParametrizedTypeName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ExternalTypeReference | typereference | xmlasn1typename
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalTypeReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_xmlasn1typename)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	SSL_LOG(WARN, "skipping parsing of ParameterizedType");
	return false;

	NOT_IMPLEMENTED("ParameterizedType");
}

static bool __asn1_v2_parse_ParameterizedValueSetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	SSL_LOG(WARN, "skipping parsing of ParameterizedValueSetType");
	return false;

	NOT_IMPLEMENTED("ParameterizedValueSetType");
}

static bool __asn1_v2_parse_DefinedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ExternalTypeReference | typereference | ParameterizedType | ParameterizedValueSetType
	PUSH_OP_OR_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalTypeReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedValueSetType)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("ParameterizedValue");
}

static bool __asn1_v2_parse_DefinedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ExternalValueReference | valuereference | ParameterizedValue
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalValueReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedBit(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// identifier "(" number ")" | identifier "(" DefinedValue ")"
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
		),
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedBitList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// NamedBit | NamedBitList "," NamedBit
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedBit),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedBitList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedBit)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BitStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// BIT STRING | BIT STRING "{" NamedBitList "}"
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BIT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_STRING),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedBitList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BIT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_STRING)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BooleanType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BOOLEAN);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RestrictedCharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// See Rec. ITU-T X.680
	// Definition of restricted character string types.

	// BMPString | GeneralString | GraphicString | IA5String | ISO646String | NumericString | PrintableString | TeletexString
	// | T61String | UniversalString | UTF8String | VideotexString | VisibleString
	PUSH_OP_OR(ctx,
		PUSH_OP_OR_10(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BMPString),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_GeneralString),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_GraphicString),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IA5String),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ISO646String),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_NumericString),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PrintableString),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TeletexString),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_T61String),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UniversalString)
		),
		PUSH_OP_OR_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UTF8String),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_VideotexString),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_VisibleString)
		)
	);

	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UnrestrictedCharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CHARACTER),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_STRING)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// RestrictedCharacterStringType | UnrestrictedCharacterStringType
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_RestrictedCharacterStringType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UnrestrictedCharacterStringType)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_AlternativeTypeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// NamedType | AlternativeTypeList "," NamedType
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_AlternativeTypeList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionAlternativeGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "[[" VersionNumber AlternativeTypeList "]]"
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_VER_BRACKET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_VersionNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_AlternativeTypeList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_VER_BRACKET)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionAlternative(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ExtensionAdditionAlternativesGroup | NamedType
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionAlternativeGroup),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionAlternativeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ExtensionAdditionAlternative | ExtensionAdditionAlternativeList "," ExtensionAdditionAlternative
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionAlternative),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionAlternativeList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionAlternative)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionAlternatives(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "," ExtensionAdditionAlternativeList | empty
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionAlternativeList)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_AlternativeTypeLists(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// RootAlternativeTypeList
	// | RootAlternativeTypeList "," ExtensionAndException ExtensionAdditionAlternatives OptionalExtensionMarker
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_AlternativeTypeList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionAlternatives),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_AlternativeTypeList)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ChoiceType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// CHOICE "{" AlternativeTypeLists "}"
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CHOICE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_AlternativeTypeLists),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DateType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DATE);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DateTimeType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DATE_TIME);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DurationType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DURATION);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EmbeddedPDVType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EMBEDDED),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PDV)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EnumerationItem(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// identifier | NamedNumber
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedNumber)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Enumeration(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// EnumerationItem | EnumerationItem "," Enumeration
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_EnumerationItem),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumeration)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EnumerationItem)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExceptionIdentification(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SignedNumber | DefinedValue | Type ":" Value
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SignedNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}
static bool __asn1_v2_parse_ExceptionSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "!" ExceptionIdentification | empty
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_BANG),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionIdentification)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_Enumerations(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// RootEnumeration
	// | RootEnumeration "," "..." ExceptionSpec
	// | RootEnumeration "," "..." ExceptionSpec "," AdditionalEnumeration
	PUSH_OP_OR_3(ctx,
		PUSH_OP_AND_6(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumeration),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionSpec),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumeration)
		),
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumeration),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionSpec)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumeration)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EnumeratedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ENUMERATED "{" Enumerations "}"
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ENUMERATED),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumerations),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXTERNAL);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_InstanceOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INSTANCE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SignedNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// number | "-" number
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_HYPHEN),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_number)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// identifier "(" SignedNumber ")" | identifier "(" DefinedValue ")"
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SignedNumber),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
		),
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedNumberList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// NamedNumber | NamedNumberList "," NamedNumber
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedNumber),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedNumberList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedNumber)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IntegerType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// INTEGER | INTEGER "{" NamedNumberList "}"
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INTEGER),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedNumberList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INTEGER)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IRIType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OID_IRI);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NullType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_NULL);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectClassFieldType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// DefinedObjectClass "." FieldName
	SSL_LOG(WARN, "skipping __asn1_v2_parse_ObjectClassFieldType: NOT_IMPLEMENTED");
	return false;

	NOT_IMPLEMENTED("__asn1_v2_parse_ObjectClassFieldType");

	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectIdentifierType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OBJECT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IDENTIFIER)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_OctetStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OCTET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_STRING)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RealType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_REAL);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeIRIType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_RELATIVE_OID_IRI);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeOIDType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_RELATIVE_OID);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAndException(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "..." | "..." ExceptionSpec
	PUSH_OP_OR(ctx,
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionSpec)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS)
	);
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_OptionalExtensionMarker(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "," "..." | empty
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_ComponentType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// NamedType | NamedType OPTIONAL | NamedType DEFAULT Value | COMPONENTS OF Type
	PUSH_OP_OR_4(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_COMPONENTS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL)
		),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFAULT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_ComponentTypeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ComponentType | ComponentTypeList "," ComponentType
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentType)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_VersionNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// empty | number ":"
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_ExtensionAdditionGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "[[" VersionNumber ComponentTypeList "]]"
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_VER_BRACKET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_VersionNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_VER_BRACKET)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_ExtensionAddition(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ComponentType | ExtensionAdditionGroup
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionGroup)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_ExtensionAdditionList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ExtensionAddition | ExtensionAdditionList "," ExtensionAddition
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAddition),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAddition)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "," ExtensionAdditionList | empty
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionList)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_ExtensionEndMarker(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "," "..."
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_ComponentTypeLists(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// RootComponentTypeList
	// | RootComponentTypeList "," ExtensionAndException ExtensionAdditions OptionalExtensionMarker
	// | RootComponentTypeList "," ExtensionAndException ExtensionAdditions ExtensionEndMarker "," RootComponentTypeList
	// | ExtensionAndException ExtensionAdditions ExtensionEndMarker "," RootComponentTypeList
	// | ExtensionAndException ExtensionAdditions OptionalExtensionMarker
	PUSH_OP_OR_5(ctx,
		PUSH_OP_AND_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditions),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker)
		),
		PUSH_OP_AND_7(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditions),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionEndMarker),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList)
		),
		PUSH_OP_AND_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditions),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionEndMarker),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList)
		),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditions),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SequenceType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SEQUENCE "{" "}"
	// | SEQUENCE "{" ExtensionAndException OptionalExtensionMarker "}"
	// | SEQUENCE "{" ComponentTypeLists "}"
	PUSH_OP_OR_3(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_AND_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeLists),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SequenceOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SEQUENCE OF Type | SEQUENCE OF NamedType
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SET "{" "}"
	// | SET "{" ExtensionAndException OptionalExtensionMarker "}"
	// | SET "{" ComponentTypeLists "}"
	PUSH_OP_OR_3(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_AND_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeLists),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SetOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SET OF Type | SET OF NamedType
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ClassNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// number | DefinedValue
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Class(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// UNIVERSAL | APPLICATION | PRIVATE | empty
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UNIVERSAL),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_APPLICATION),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PRIVATE)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_EncodingReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// encodingreference ":" | empty
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_encodingreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_Tag(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "[" EncodingReference Class ClassNumber "]"
	PUSH_OP_AND_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_SQUARE_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Class),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ClassNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_SQUARE_BRKT)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TaggedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Tag Type | Tag IMPLICIT Type | Tag EXPLICIT Type
	PUSH_OP_OR_3(ctx,
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Tag),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Tag),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IMPLICIT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Tag),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXPLICIT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EncodingInstruction(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_EncodingInstruction");
}

static bool __asn1_v2_parse_EncodingPrefix(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "[" EncodingReference EncodingInstruction "]"
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_SQUARE_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingInstruction),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_SQUARE_BRKT)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EncodingPrefixedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// EncodingPrefix Type
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingPrefix),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PrefixedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// TaggedType | EncodingPrefixedType
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TaggedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingPrefixedType)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TimeType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TIME);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TimeOfDayType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TIME_OF_DAY);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

// This is legacy and included only for backward compatibility.
static bool __asn1_v2_parse_AnyType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ANY | ANY DEFINED BY identifier
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ANY),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFINED),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BY),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ANY)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BuiltinType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// BitStringType | BooleanType | CharacterStringType | ChoiceType | DateType | DateTimeType | DurationType |
	// EmbeddedPDVType | EnumeratedType | ExternalType | InstanceOfType | IntegerType | IRIType | NullType |
	// ObjectClassFieldType | ObjectIdentifierType | OctetStringType | RealType | RelativeIRIType | RelativeOIDType |
	// SequenceType | SequenceOfType | SetType | SetOfType | PrefixedType | TimeType | TimeOfDayType | AnyType
	PUSH_OP_OR_3(ctx,
		PUSH_OP_OR_10(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_BitStringType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_BooleanType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_CharacterStringType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ChoiceType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DateType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DateTimeType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DurationType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_EmbeddedPDVType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_EnumeratedType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalType)
		),
		PUSH_OP_OR_10(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_InstanceOfType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_IntegerType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_IRIType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NullType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClassFieldType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectIdentifierType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_OctetStringType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_RealType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeIRIType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeOIDType)
		),
		PUSH_OP_OR_8(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceOfType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SetType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SetOfType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_PrefixedType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TimeType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TimeOfDayType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_AnyType)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UsefulType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// GeneralizedTime | UTCTime | ObjectDescriptor
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_GeneralizedTime),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UTCTime),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ObjectDescriptor)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SelectionType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// identifier "<" Type
	PUSH_OP_AND_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_ANGLE_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	SSL_LOG(WARN, "skipping parsing of TypeFromObject");
	return false;

	NOT_IMPLEMENTED("__asn1_v2_parse_TypeFromObject");
}

static bool __asn1_v2_parse_ValueSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	SSL_LOG(WARN, "skipping parsing of ValueSetFromObjects");
	return false;

	NOT_IMPLEMENTED("__asn1_v2_parse_ValueSetFromObjects");
}

static bool __asn1_v2_parse_ReferencedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// DefinedType | UsefulType | SelectionType | TypeFromObject | ValueSetFromObjects
	PUSH_OP_OR_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UsefulType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SelectionType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeFromObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSetFromObjects)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SubtypeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ElementSetSpecs
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpecs);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_GeneralConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_GeneralConstraint");
}

static bool __asn1_v2_parse_ConstraintSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SubtypeConstraint | GeneralConstraint
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SubtypeConstraint),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_GeneralConstraint)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Constraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "(" ConstraintSpec ExceptionSpec ")"
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ConstraintSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeWithConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SET Constraint OF Type
	// | SET SizeConstraint OF Type
	// | SEQUENCE Constraint OF Type
	// | SEQUENCE SizeConstraint OF Type
	// | SET Constraint OF NamedType
	// | SET SizeConstraint OF NamedType
	// | SEQUENCE Constraint OF NamedType
	// | SEQUENCE SizeConstraint OF NamedType
	PUSH_OP_OR_8(ctx,
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SizeConstraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SizeConstraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		),
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SizeConstraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		),
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		),
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SizeConstraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ConstrainedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Type Constraint | TypeWithConstraint
	PUSH_OP_OR(ctx,
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeWithConstraint)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// BuiltinType | ReferencedType | ConstrainedType
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_BuiltinType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ConstrainedType)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// identifier Type
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IdentifierList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// identifier | IdentifierList "," identifier
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_IdentifierList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BitStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// bstring | hstring | "{" IdentifierList "}" | "{" "}" | CONTAINING Value
	PUSH_OP_OR_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_bstring),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_hstring),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_IdentifierList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CONTAINING),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLBitStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLBitStringValue");
}

static bool __asn1_v2_parse_BooleanValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// TRUE | FALSE
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TRUE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_FALSE)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLBooleanValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLBooleanValue");
}

static bool __asn1_v2_parse_Tuple(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "{" TableColumn "," TableRow "," Row "," Cell "}"
	PUSH_OP_AND_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Quadruple(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "{" Group "," Plane "," Row "," Cell "}"
	PUSH_OP_AND_9(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharsDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// cstring	| Quadruple	| Tuple	| DefinedValue
	PUSH_OP_OR_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_cstring),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Quadruple),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Tuple),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharSyms(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// CharsDefn | CharSyms "," CharsDefn
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_CharsDefn),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_CharSyms)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_CharsDefn)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharacterStringList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "{" CharSyms "}"

	PUSH_OP_AND_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_CharSyms),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_RestrictedCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// See Rec. ITU-T X.680
	// Definition of restricted character string types.

	// cstring | CharacterStringList | Quadruple | Tuple

	PUSH_OP_OR_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_cstring),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_CharacterStringList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Quadruple),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Tuple)
	);

	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UnrestrictedCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SequenceValue
	PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// RestrictedCharacterStringValue | UnrestrictedCharacterStringValue
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_RestrictedCharacterStringValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UnrestrictedCharacterStringValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLCharacterStringValue");
}

static bool __asn1_v2_parse_ChoiceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// identifier ":" Value
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLChoiceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLChoiceValue");
}

static bool __asn1_v2_parse_EmbeddedPDVValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SequenceValue
	PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EnumeratedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// identifier
	PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLEnumeratedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLEnumeratedValue");
}

static bool __asn1_v2_parse_ExternalValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SequenceValue
	PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLExternalValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLExternalValue");
}

static bool __asn1_v2_parse_InstanceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_InstanceOfValue");
}
static bool __asn1_v2_parse_XMLInstanceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLInstanceOfValue");
}

static bool __asn1_v2_parse_IntegerValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SignedNumber | identifier
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SignedNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLIntegerValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLIntegerValue");
}

static bool __asn1_v2_parse_ArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// integerUnicodeLabel | non-integerUnicodeLabel
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_integer_unicode_label),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_non_integer_unicode_label)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FirstArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "/" ArcIdentifier
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SLASH),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ArcIdentifier)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SubsequentArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "/" ArcIdentifier SubsequentArcIdentifier | empty
	PUSH_OP_AND_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SLASH),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ArcIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SubsequentArcIdentifier)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_IRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// """ FirstArcIdentifier SubsequentArcIdentifier """
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FirstArcIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SubsequentArcIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLIRIValue");
}


static bool __asn1_v2_parse_NullValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// NULL
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_NULL);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLNullValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLNullValue");
}


static bool __asn1_v2_parse_OctetStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// bstring | hstring | CONTAINING Value
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_bstring),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_hstring),
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CONTAINING),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLOctetStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLOctetStringValue");
}

static bool __asn1_v2_parse_NumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// number | DefinedValue
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NameAndNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// identifier "(" NumberForm ")"
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjIdComponents(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// NameForm | NumberForm | NameAndNumberForm | DefinedValue
	PUSH_OP_OR_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NameForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NameAndNumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjIdComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ObjIdComponents | ObjIdComponents ObjIdComponentsList
	PUSH_OP_OR(ctx,
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjIdComponents),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjIdComponentsList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjIdComponents)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "{" ObjIdComponentsList "}" | "{" DefinedValue ObjIdComponentsList "}"
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjIdComponentsList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_AND_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjIdComponentsList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLObjectIdentifierValue");
}

static bool __asn1_v2_parse_NumericRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// realnumber | "-" realnumber | SequenceValue
	PUSH_OP_OR_3(ctx,
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_HYPHEN),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_realnumber)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_realnumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_SpecialRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// PLUS-INFINITY | MINUS-INFINITY | NOT-A-NUMBER
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PLUS_INFINITY),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_MINUS_INFINITY),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_NOT_A_NUMBER)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_RealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// NumericRealValue | SpecialRealValue
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NumericRealValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SpecialRealValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLRealValue");
}

static bool __asn1_v2_parse_FirstRelativeArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ArcIdentifier
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ArcIdentifier);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// """ FirstRelativeArcIdentifier SubsequentArcIdentifier """
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FirstRelativeArcIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SubsequentArcIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLRelativeIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLRelativeIRIValue");
}

static bool __asn1_v2_parse_RelativeOIDComponents(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// NumberForm | NameAndNumberForm | DefinedValue
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NameAndNumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeOIDComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// RelativeOIDComponents | RelativeOIDComponents RelativeOIDComponentsList
	PUSH_OP_OR(ctx,
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeOIDComponents),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeOIDComponentsList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeOIDComponents)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeOIDValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "{" RelativeOIDComponentsList "}"
	PUSH_OP_AND_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeOIDComponentsList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLRelativeOIDValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLRelativeOIDValue");
}

static bool __asn1_v2_parse_ComponentValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// NamedValue | ComponentValueList "," NamedValue
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentValueList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SequenceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "{" ComponentValueList "}" | "{" "}"
	PUSH_OP_OR(ctx,
		PUSH_OP_OR_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentValueList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_OR(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLSequenceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLSequenceValue");
}

static bool __asn1_v2_parse_ValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Value | ValueList "," Value
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Value),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// NamedValue | ComponentValueList "," NamedValue
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedValueList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SequenceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "{" ValueList "}" | "{" NamedValueList "}" | "{" "}"
	PUSH_OP_OR_3(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedValueList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLSequenceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLSequenceOfValue");
}

static bool __asn1_v2_parse_SetValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "{" ComponentValueList "}" | "{" "}"
	PUSH_OP_OR(ctx,
		PUSH_OP_OR_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentValueList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_OR(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLSetValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLSetValue");
}

static bool __asn1_v2_parse_SetOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "{" ValueList "}" | "{" NamedValueList "}" | "{" "}"
	PUSH_OP_OR_3(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedValueList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLSetOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLSetOfValue");
}

static bool __asn1_v2_parse_PrefixedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Value
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Value);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLPrefixedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLPrefixedValue");
}

static bool __asn1_v2_parse_TimeValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// tstring
	PUSH_OP_CALL(ctx, __asn1_v2_parse_tstring);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLTimeValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLTimeValue");
}

// This is legacy and included only for backward compatibility.
static bool __asn1_v2_parse_AnyValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Type Value
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BuiltinValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// BitStringValue | BooleanValue | CharacterStringValue | ChoiceValue | EmbeddedPDVValue | EnumeratedValue |
	// ExternalValue | InstanceOfValue | IntegerValue | IRIValue | NullValue | ObjectIdentifierValue | OctetStringValue |
	// RealValue | RelativeIRIValue | RelativeOIDValue | SequenceValue | SequenceOfValue | SetValue | SetOfValue |
	// PrefixedValue | TimeValue | AnyValue
	PUSH_OP_OR_3(ctx,
		PUSH_OP_OR_10(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_BitStringValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_BooleanValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_CharacterStringValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ChoiceValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_EmbeddedPDVValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_EnumeratedValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_InstanceOfValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_IntegerValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_IRIValue)
		),
		PUSH_OP_OR_10(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NullValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_OctetStringValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectIdentifierValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_RealValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeIRIValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeOIDValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceOfValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SetValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SetOfValue)
		),
		PUSH_OP_OR_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_PrefixedValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TimeValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_AnyValue)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_ValueFromObject");
}

static bool __asn1_v2_parse_ReferencedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// DefinedValue | ValueFromObject
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueFromObject)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectClassFieldValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_ObjectClassFieldValue");
}

static bool __asn1_v2_parse_Value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// BuiltinValue | ReferencedValue | ObjectClassFieldValue
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_BuiltinValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClassFieldValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// identifier Value
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_XMLValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// XMLBuiltinValue | XMLObjectClassFieldValue
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLValue");
}

static bool __asn1_v2_parse_XMLNamedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "<" & identifier ">" XMLValue "</" & identifier ">"
	NOT_IMPLEMENTED("__asn1_v2_parse_XMLNamedValue");
}

static bool __asn1_v2_parse_TypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// typereference "::=" Type
	PUSH_OP_AND_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// valuereference Type "::=" Value
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_XMLTypedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("xml grammar");
}

static bool __asn1_v2_parse_XMLValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// valuereference "::=" XMLTypedValue
	PUSH_OP_AND_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_XMLTypedValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SingleValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Value
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Value);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Includes(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// INCLUDES | empty
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INCLUDES);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}
static bool __asn1_v2_parse_ContainedSubtype(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Includes Type
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Includes),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UpperEndValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// UpperEndValue ::= Value | MAX
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_MAX)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_LowerEndValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// LowerEndValue ::= Value | MIN
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_MIN)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UpperEndpoint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// UpperEndpoint ::= UpperEndValue | "<" UpperEndValue
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UpperEndValue),
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_ANGLE_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_UpperEndValue)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_LowerEndpoint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// LowerEndpoint ::= LowerEndValue | LowerEndValue "<"
	PUSH_OP_OR(ctx,
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_LowerEndValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_ANGLE_BRKT)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_LowerEndValue)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// LowerEndpoint ".." UpperEndpoint
	PUSH_OP_AND_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_LowerEndpoint),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_RANGE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UpperEndpoint)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PermittedAlphabet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// FROM Constraint
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_FROM),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SizeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SIZE Constraint
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SIZE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Type
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Type);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PresenceConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// PRESENT | ABSENT | OPTIONAL | empty
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PRESENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ABSENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_ValueConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Constraint | empty
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_ComponentConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ValueConstraint PresenceConstraint
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueConstraint),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_PresenceConstraint)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// identifier ComponentConstraint
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentConstraint)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// NamedConstraint | NamedConstraint "," TypeConstraints
	// NamedConstraint ::= identifier ComponentConstraint
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedConstraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeConstraints)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedConstraint)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_MultipleTypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// FullSpecification | PartialSpecification
	// FullSpecification ::= "{" TypeConstraints "}"
	// PartialSpecification ::= "{" "..." "," TypeConstraints "}"
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeConstraints),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_AND_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeConstraints),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_InnerTypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// WITH COMPONENT SingleTypeConstraint
	// | WITH COMPONENTS MultipleTypeConstraints
	PUSH_OP_OR(ctx,
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_WITH),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_COMPONENT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
		),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_WITH),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_COMPONENTS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_MultipleTypeConstraints)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PatternConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// PATTERN Value
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PATTERN),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PropertyAndSettingPair(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// PropertyName "=" SettingName
	// PropertyName ::= psname
	// SettingName ::= psname
	PUSH_OP_AND_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_EQUAL),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PropertySettingsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// PropertyAndSettingPair | PropertySettingsList PropertyAndSettingPair
	PUSH_OP_OR(ctx,
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_PropertyAndSettingPair),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_PropertySettingsList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_PropertySettingsList)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PropertySettings(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SETTINGS simplestring
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SETTINGS),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_cstring)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DurationRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ValueRange
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueRange);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TimePointRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ValueRange
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueRange);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RecurrenceRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ValueRange
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueRange);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SubtypeElements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SingleValue
	// | ContainedSubtype
	// | ValueRange
	// | PermittedAlphabet
	// | SizeConstraint
	// | TypeConstraint
	// | InnerTypeConstraints
	// | PatternConstraint
	// | PropertySettings
	// | DurationRange
	// | TimePointRange
	// | RecurrenceRange
	PUSH_OP_OR(ctx,
		PUSH_OP_OR_10(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SingleValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ContainedSubtype),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueRange),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_PermittedAlphabet),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SizeConstraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeConstraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_InnerTypeConstraints),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_PatternConstraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_PropertySettings),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DurationRange)
		),
		PUSH_OP_OR(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TimePointRange),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_RecurrenceRange)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSetElements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("__asn1_v2_parse_ObjectSetElements");
}

static bool __asn1_v2_parse_Elements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// SubtypeElements | ObjectSetElements | "(" ElementSetSpec ")"
	PUSH_OP_OR_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SubtypeElements),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSetElements),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Exclusions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// EXCEPT Elements
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXCEPT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Elements)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IntersectionElements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Elements | Elems Exclusions
	PUSH_OP_OR(ctx,
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Elements),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Exclusions)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Elements)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IntersectionMark(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "^" | INTERSECTION
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_CARET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INTERSECTION)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Intersections(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// IntersectionElements | IElems IntersectionMark IntersectionElements
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_IntersectionElements),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Intersections),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_IntersectionMark),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_IntersectionElements)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UnionMark(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "|" | UNION
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_PIPE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UNION)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Unions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Intersections | UElems UnionMark Intersections
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Intersections),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Unions),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_UnionMark),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Intersections)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ElementSetSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Unions | ALL Exclusions
	PUSH_OP_OR(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Unions),
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ALL),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Exclusions)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ElementSetSpecs(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// RootElementSetSpec
	// | RootElementSetSpec "," "..."
	// | RootElementSetSpec "," "..." "," AdditionalElementSetSpec
	PUSH_OP_OR_3(ctx,
		PUSH_OP_AND_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec)
		),
		PUSH_OP_AND_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// "{" ElementSetSpecs "}"
	PUSH_OP_AND_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpecs),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueSetTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// typereference Type "::=" ValueSet
	PUSH_OP_AND_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSet)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectClassAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("ObjectClassAssignment");
}

static bool __asn1_v2_parse_ObjectAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("ObjectAssignment");
}

static bool __asn1_v2_parse_ObjectSetAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("ObjectSetAssignment");
}

static bool __asn1_v2_parse_ParameterizedAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	NOT_IMPLEMENTED("ParameterizedAssignment");
}

static bool __asn1_v2_parse_Assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// TypeAssignment | ValueAssignment | XMLValueAssignment | ValueSetTypeAssignment |
	// ObjectClassAssignment | ObjectAssignment | ObjectSetAssignment | ParameterizedAssignment
	PUSH_OP_OR_8(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_XMLValueAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSetTypeAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClassAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSetAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedAssignment)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_AssignmentList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Assignment | AssignmentList Assignment
	PUSH_OP_OR(ctx,
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Assignment),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_AssignmentList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Assignment)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_ModuleBody(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// Exports Imports AssignmentList | empty
	PUSH_OP_AND(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Exports),
		PUSH_OP_AND(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Imports),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_AssignmentList)
		)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return true;
}

static bool __asn1_v2_parse_Module(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_data **data)
{
	// ModuleIdentifier DEFINITIONS EncodingReferenceDefault TagDefault ExtensionDefault "::="
	// BEGIN ModuleBody EncodingControlSections END
	PUSH_OP_AND_9(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ModuleIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFINITIONS),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingReferenceDefault),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TagDefault),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionDefault),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BEGIN),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ModuleBody),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_END)
	);
	if (RUN_OP(ctx, token, data)) {
		return true;
	}
	return false;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

// static t_asn_v2_token *__asn1_v2_token_create(void)
// {
// 	t_asn_v2_token *token = NULL;
// 	SSL_ALLOC(token, sizeof(t_asn_v2_token));
// 	*token = (t_asn_v2_token){0};
// 	return (token);
// }
//
// static void __asn1_v2_token_delete(t_asn_v2_token *token)
// {
// 	*token = (t_asn_v2_token){0};
// 	SSL_FREE(token);
// }

int asn1_v2_parse(const char *s, size_t slen)
{
	if (NULL == s) return (SSL_ERR);

	t_asn_v2_parser_ctx ctx = {0};
	t_asn_v2_token token = {.s = s, .slen = slen,};
	t_asn_v2_data *data = NULL;

	if (!__asn1_v2_parse_Module(&ctx, &token, &data)) {
		return (SSL_ERR);
	}
	if (!token.done) {
		SSL_LOG(ERROR, "parsing is not complete");
		return (SSL_ERR);
	}
	return (SSL_OK);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

// static void __asn1_v2_parser_op_push_any_va(t_asn_v2_parser_ctx *ctx, size_t count, va_list va)
// {
// 	assert(count > 0);
//
// 	if (count == 1) {
// 		t_func_asn_v2_parse f = va_arg(va, t_func_asn_v2_parse);
// 		__asn1_v2_parser_op_push_call(ctx, f);
// 	}
// 	else if (count == 2) {
// 		t_func_asn_v2_parse f1 = va_arg(va, t_func_asn_v2_parse);
// 		t_func_asn_v2_parse f2 = va_arg(va, t_func_asn_v2_parse);
// 		__asn1_v2_parser_op_push_or(ctx);
// 		__asn1_v2_parser_op_push_call(ctx, f1);
// 		__asn1_v2_parser_op_push_call(ctx, f2);
// 	}
// 	else {
// 		t_func_asn_v2_parse f1 = va_arg(va, t_func_asn_v2_parse);
// 		__asn1_v2_parser_op_push_or(ctx);
// 		__asn1_v2_parser_op_push_call(ctx, f1);
// 		__asn1_v2_parser_op_push_any_va(ctx, count-1, va);
// 	}
// }
//
// static void __asn1_v2_parser_op_push_any(t_asn_v2_parser_ctx *ctx, ...)
// {
// 	va_list va;
// 	va_start(va);
// 	size_t count = 0;
// 	t_func_asn_v2_parse f = va_arg(va, t_func_asn_v2_parse);
// 	while (f != nullptr) {
// 		count++;
// 		f = va_arg(va, t_func_asn_v2_parse);
// 	}
// 	va_end(va);
// 	va_start(va);
// 	__asn1_v2_parser_op_push_any_va(ctx, count, va);
// 	va_end(va);
// }
//
// static void __asn1_v2_parser_op_push_all_va(t_asn_v2_parser_ctx *ctx, size_t count, va_list va)
// {
// 	assert(count > 0);
//
// 	if (count == 1) {
// 		t_func_asn_v2_parse f = va_arg(va, t_func_asn_v2_parse);
// 		__asn1_v2_parser_op_push_call(ctx, f);
// 	}
// 	else if (count == 2) {
// 		t_func_asn_v2_parse f1 = va_arg(va, t_func_asn_v2_parse);
// 		t_func_asn_v2_parse f2 = va_arg(va, t_func_asn_v2_parse);
// 		__asn1_v2_parser_op_push_and(ctx);
// 		__asn1_v2_parser_op_push_call(ctx, f1);
// 		__asn1_v2_parser_op_push_call(ctx, f2);
// 	}
// 	else {
// 		t_func_asn_v2_parse f1 = va_arg(va, t_func_asn_v2_parse);
// 		__asn1_v2_parser_op_push_and(ctx);
// 		__asn1_v2_parser_op_push_call(ctx, f1);
// 		__asn1_v2_parser_op_push_any_va(ctx, count-1, va);
// 	}
// }
//
// static void __asn1_v2_parser_op_push_all(t_asn_v2_parser_ctx *ctx, ...)
// {
// 	va_list va;
// 	va_start(va);
// 	size_t count = 0;
// 	t_func_asn_v2_parse f = va_arg(va, t_func_asn_v2_parse);
// 	while (f != nullptr) {
// 		count++;
// 		f = va_arg(va, t_func_asn_v2_parse);
// 	}
// 	va_end(va);
// 	va_start(va);
// 	__asn1_v2_parser_op_push_all_va(ctx, count, va);
// 	va_end(va);
// }
//
// #define PUSH_OP_ALL(ctx, ...) \
// 	((void)__asn1_v2_parser_op_push_all((ctx) __VA_OPT__(,) __VA_ARGS__)
//
// #define PUSH_OP_ANY(ctx, ..) \
// 	((void)__asn1_v2_parser_op_push_any((ctx) __VA_OPT__(,) __VA_ARGS__)
