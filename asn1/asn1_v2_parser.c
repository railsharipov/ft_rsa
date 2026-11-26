#include <common.h>
#include <logger.h>
#include <asn1_v2.h>
#include <libft.h>
#include <base64.h>
#include <qht.h>

static const char *__asn1_v2_get_cst_kind_name(t_asn_v2_cst_kind kind)
{
	switch (kind) {
	case ASN_V2_DATA_KIND_NULL: return ("NULL");
	case ASN_V2_DATA_KIND_EMPTY: return ("EMPTY");
	case ASN_V2_DATA_KIND_LEAF: return ("LEAF");
	case ASN_V2_DATA_KIND_LIST: return ("LIST");
	case ASN_V2_DATA_KIND_WRAP: return ("WRAP");
	default: return ("unknown");
	}
}

static const char *__asn1_v2_get_token_kind_name(t_asn_v2_token_kind kind)
{
	switch (kind) {
	case ASN_V2_TOKEN_KIND_EMPTY: 			return "EMPTY";
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
	case ASN_V2_TOKEN_KIND_EOF:	            return "EOF";
	default:								return "unknown";
	}
}

static struct {
	bool alternative;
	bool rerouted;
} __global_flags;

static uintptr_t __running_chksum;

typedef enum e_asn_v2_parser_op_kind {
	ASN_V2_PARSER_OP_KIND_NULL = 0,
	ASN_V2_PARSER_OP_KIND_CALL,
	ASN_V2_PARSER_OP_KIND_OPTIONAL,
	ASN_V2_PARSER_OP_KIND_OR_EMPTY,
	ASN_V2_PARSER_OP_KIND_SEQUENCE,
	ASN_V2_PARSER_OP_KIND_CHOICE,
	ASN_V2_PARSER_OP_KIND_REPEAT,
	ASN_V2_PARSER_OP_KIND_UNWRAP,
	ASN_V2_PARSER_OP_KIND_WRAP,
	ASN_V2_PARSER_OP_KIND_COMPACT,
} t_asn_v2_parser_op_kind;

static const char *__asn1_v2_get_op_kind_name(t_asn_v2_parser_op_kind kind)
{
	switch (kind) {
	case ASN_V2_PARSER_OP_KIND_NULL: return ("NULL");
	case ASN_V2_PARSER_OP_KIND_CALL: return ("CALL");
	case ASN_V2_PARSER_OP_KIND_OPTIONAL: return ("OPTIONAL");
	case ASN_V2_PARSER_OP_KIND_OR_EMPTY: return ("OR_EMPTY");
	case ASN_V2_PARSER_OP_KIND_SEQUENCE: return ("SEQUENCE");
	case ASN_V2_PARSER_OP_KIND_CHOICE: return ("CHOICE");
	case ASN_V2_PARSER_OP_KIND_REPEAT: return ("REPEAT");
	case ASN_V2_PARSER_OP_KIND_UNWRAP: return ("UNWRAP");
	case ASN_V2_PARSER_OP_KIND_WRAP: return ("WRAP");
	case ASN_V2_PARSER_OP_KIND_COMPACT: return ("COMPACT");
	default: return ("unknown");
	}
}

typedef struct s_asn_v2_parser_op {
	t_asn_v2_parser_op_kind kind;
} t_asn_v2_parser_op;

struct s_asn_v2_parser_ctx;
typedef bool (*t_func_asn_v2_parse)(struct s_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);

typedef struct s_asn_v2_parser_func {
	t_func_asn_v2_parse call;
	const char *name;
} t_asn_v2_parser_func;

#define ASN_V2_MAX_P_OPS 64
#define ASN_V2_MAX_P_FUNCS 64

typedef struct s_asn_v2_parser_ctx {
	t_asn_v2_parser_op ops[ASN_V2_MAX_P_OPS];
	size_t op_idx;
	t_asn_v2_parser_func funcs[ASN_V2_MAX_P_FUNCS];
	size_t func_idx;
	struct {
		int depth;
		bool done;
	} params;
} t_asn_v2_parser_ctx;

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
	{"AMPERSAND",     '&'},
};
const size_t SINGLE_CHAR_LEX_COUNT = sizeof(SINGLE_CHAR_LEXES)/sizeof(*SINGLE_CHAR_LEXES);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static inline const char *__asn1_v2_strip_parse_func_prefix(const char *name)
{
	// const char prefix[16] = "__asn1_v2_parse_";
    // return (ft_strncmp(name, prefix, sizeof(prefix)) == 0 ? name + sizeof(prefix) : name);
	return (name + 16);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

const char *__asn1_v2_token_dumpb(t_asn_v2_token token, char *buf, size_t bufsize);

bool __asn1_v2_token_is_bound(char c)
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
		token->kind = ASN_V2_TOKEN_KIND_EMPTY;
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
					// Ensure this not an ELLIPSIS since this DOT could be part of the range: (NUMBER..NUMBER)
					if (s[i+1] != '.') {
						t.kind = ASN_V2_TOKEN_KIND_REAL_NUMBER;
						i++;
						while (i < slen && ft_isdigit(s[i])) i++;
					}
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
				if (__asn1_v2_token_is_bound(s[i])) {
					t.end = t.start + i;
					goto label_ok;
				}
				break;
			}
		}
	}

	const char *lex = ASN_V2_LEX_ELLIPSIS;
	if (ft_strneq(s, ASN_V2_LEX_ELLIPSIS, ft_strlen(lex))) {
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
				if (__asn1_v2_token_is_bound(s[i])) {
					t.kind = ASN_V2_TOKEN_KIND_NAME;
					t.end = t.start + i;
					goto label_ok;
				}
				break;
			}
			if (s[i] == '-' && is_prev_hyphen) break;
			is_prev_hyphen = (s[i] == '-');
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
				if (__asn1_v2_token_is_bound(s[i])) {
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
	// char cbuf[1024];
	// SSL_LOG(TRACE, "next token: pos: % 5zu: %-12s: %s", t.start, __asn1_v2_get_token_kind_name(t.kind), __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	*token = t;
	if (t.kind == ASN_V2_TOKEN_KIND_COMMENT) {
		return (__asn1_v2_token_next(token));
	}
	return true;
}

t_asn_v2_token __asn1_v2_token_get_next(t_asn_v2_token token)
{
	if (__asn1_v2_token_next(&token)) return (token);
	return ((t_asn_v2_token){0});
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static t_asn_v2_token *__tokens;
static size_t __token_count;

static void __asn1_v2_token_array_init(size_t slen)
{
	SSL_ALLOC(__tokens, (slen+1) * sizeof(t_asn_v2_token));
	return;
}

static t_asn_v2_token *__asn1_v2_token_create(void)
{
	t_asn_v2_token *token = NULL;
	SSL_ALLOC(token, sizeof(t_asn_v2_token));
	*token = (t_asn_v2_token){0};
	return (token);
}

static t_asn_v2_token *__asn1_v2_token_clone(t_asn_v2_token *src)
{
	t_asn_v2_token *dst = __asn1_v2_token_create();
	*dst = *src;
	return (dst);
}

static void __asn1_v2_token_delete(t_asn_v2_token *token)
{
	*token = (t_asn_v2_token){0};
	SSL_FREE(token);
}

static bool __asn1_v2_token_value_eq(t_asn_v2_token token, const char *s)
{
	size_t slen = ft_strlen(s);
	size_t len = MIN(slen, token.end - token.start);
	return (ft_strneq(token.s + token.start, s, len));
}

static bool __asn1_v2_token_eq(t_asn_v2_token a, t_asn_v2_token b)
{
	return (
		a.kind == b.kind
		&& a.s == b.s
		&& a.slen == b.slen
		&& a.start == b.start
		&& a.end == b.end
	);
}

static char *__asn1_v2_token_dumps(t_asn_v2_token token)
{
	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 256);
	ft_ostr_appendf(&ostring, "pos=%zu,kind=%s,value=", token.start, __asn1_v2_get_token_kind_name(token.kind));
	ft_ostr_append(&ostring, token.s + token.start, token.end - token.start);
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

const char *__asn1_v2_token_dumpb(t_asn_v2_token token, char *buf, size_t bufsize)
{
	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, bufsize);
	ft_ostr_appendf(&ostring, "pos=%zu,kind=%s,value=", token.start, __asn1_v2_get_token_kind_name(token.kind));
	ft_ostr_append(&ostring, token.s + token.start, token.end - token.start);
	size_t len = MIN(bufsize, ostring.size+1);
	ft_strncpy(buf, (const char *)ostring.content, len);
	buf[len-1] = 0;
	ft_ostr_clear(&ostring);
	return (buf);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static t_asn_v2_cst *__asn1_v2_cst_create(void)
{
	t_asn_v2_cst *cst = NULL;
	SSL_ALLOC(cst, sizeof(t_asn_v2_cst));
	*cst = (t_asn_v2_cst){0};
	return (cst);
}

static t_asn_v2_cst *__asn1_v2_cst_create_leaf(t_asn_v2_token token)
{
	t_asn_v2_cst *cst = __asn1_v2_cst_create();
	cst->kind = ASN_V2_DATA_KIND_LEAF;
	cst->token = __asn1_v2_token_clone(&token);
	return (cst);
}

static t_asn_v2_cst *__asn1_v2_cst_create_list(void)
{
	t_asn_v2_cst *cst = __asn1_v2_cst_create();
	cst->kind = ASN_V2_DATA_KIND_LIST;
	return (cst);
}

static t_asn_v2_cst *__asn1_v2_cst_create_wrap(void)
{
	t_asn_v2_cst *cst = __asn1_v2_cst_create();
	cst->kind = ASN_V2_DATA_KIND_WRAP;
	return (cst);
}

static t_asn_v2_cst *__asn1_v2_cst_create_null(void)
{
	t_asn_v2_cst *cst = __asn1_v2_cst_create();
	cst->kind = ASN_V2_DATA_KIND_NULL;
	return (cst);
}

static t_asn_v2_cst *__asn1_v2_cst_create_empty(void)
{
	t_asn_v2_cst *cst = __asn1_v2_cst_create();
	cst->kind = ASN_V2_DATA_KIND_EMPTY;
	return (cst);
}

static void __asn1_v2_cst_add_parser_info(t_asn_v2_cst *cst, const char *func_name)
{
	cst->parser = __asn1_v2_strip_parse_func_prefix(func_name);
}

static void __asn1_v2_cst_add_child(t_asn_v2_cst *root, t_asn_v2_cst *child)
{
	t_node_v2 *node = root->base.nodes;
	if (NULL == node) {
		root->base.nodes = &child->base;
		return;
	}
	while (node && node->next) {
		node = node->next;
	}
	node->next = &child->base;
}

static void __asn1_v2_cst_move_children(t_asn_v2_cst *dst, t_asn_v2_cst *src)
{
	t_node_v2 *node = dst->base.nodes;
	if (NULL == node) {
		dst->base.nodes = src->base.nodes;
		src->base.nodes = NULL;
		return;
	}
	while (node && node->next) {
		node = node->next;
	}
	node->next = src->base.nodes;
	src->base.nodes = NULL;
}

static void __asn1_v2_cst_node_delete_adapter(t_node_v2 *node)
{
	t_asn_v2_cst *cst = container_of(node, t_asn_v2_cst, base);
	if (cst->token != NULL) __asn1_v2_token_delete(cst->token);
	SSL_FREE(cst);
}

static void __asn1_v2_cst_delete(t_asn_v2_cst *cst)
{
	ft_ntree_v2_del(&cst->base, __asn1_v2_cst_node_delete_adapter);
}

static t_node_v2 *__asn1_v2_cst_node_copy_adapter(const t_node_v2 *node)
{
	t_asn_v2_cst *src = container_of(node, t_asn_v2_cst, base);
	t_asn_v2_cst *clone = __asn1_v2_cst_create();
	clone->kind = src->kind;
	clone->parser = src->parser;
	if (NULL != src->token) clone->token = __asn1_v2_token_clone(src->token);
	return (&clone->base);
}

t_asn_v2_cst *__asn1_v2_cst_clone(const t_asn_v2_cst *src)
{
	if (NULL == src) return (NULL);
	t_node_v2 *clone = ft_ntree_v2_clone(&src->base, __asn1_v2_cst_node_copy_adapter);
	assert(clone != NULL);
	return (container_of(clone, t_asn_v2_cst, base));
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static void __asn1_v2_parser_op_copy(const t_asn_v2_parser_op *src, t_asn_v2_parser_op *dst)
{
	*dst = *src;
}

static void __asn1_v2_parser_func_copy(const t_asn_v2_parser_func *src, t_asn_v2_parser_func *dst)
{
	*dst = *src;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static void __asn1_v2_parser_ctx_copy_params(const t_asn_v2_parser_ctx *src, t_asn_v2_parser_ctx *dst)
{
	dst->params = src->params;
}

static void __asn1_v2_parser_ctx_copy_ops(const t_asn_v2_parser_ctx *src, t_asn_v2_parser_ctx *dst)
{
	ft_memcpy(dst->ops, src->ops, src->op_idx * sizeof(t_asn_v2_parser_op));
}

static void __asn1_v2_parser_ctx_copy_funcs(const t_asn_v2_parser_ctx *src, t_asn_v2_parser_ctx *dst)
{
	ft_memcpy(dst->funcs, src->funcs, src->func_idx * sizeof(t_asn_v2_parser_func));
}

static void __asn1_v2_parser_ctx_copy(const t_asn_v2_parser_ctx *src, t_asn_v2_parser_ctx *dst)
{
	*dst = *src;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

typedef struct s_call_h_entry {
	const char *name;
	t_asn_v2_token token;
} t_call_h_entry;

static t_call_h_entry __call_history[1024] = {0};
static size_t __call_history_idx = 0;

static void __asn1_v2_call_history_entry_push(t_asn_v2_token token, const char *name)
{
	if (__call_history_idx >= 1024) {
		SSL_LOG(WARN, "call stack limit reached");
		__call_history_idx = 0;
	}
	t_call_h_entry *e = __call_history + __call_history_idx;
	e->name = name;
	e->token = token;
	__call_history_idx++;
}

static void __asn1_v2_call_history_entry_pop(void)
{
	if (__call_history_idx > 0) {
		__call_history_idx--;
	}
}

static char *__asn1_v2_call_stack_dumps(void)
{
	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 2048);
	for (size_t i = __call_history_idx; i > 0; i--) {
		t_call_h_entry *e = __call_history + (i-1);
		char cbuf[256];
		ft_ostr_appendf(&ostring, "    %-40s --- %s\n", e->name, __asn1_v2_token_dumpb(e->token, cbuf, sizeof(cbuf)));
	}
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

static void __asn1_v2_call_stack_dump(void)
{
	for (size_t i = __call_history_idx; i > 0; i--) {
		t_call_h_entry *e = __call_history + (i-1);
		char cbuf[256];
		ft_dprintf(2, "\t%-40s --- %s\n", e->name, __asn1_v2_token_dumpb(e->token, cbuf, sizeof(cbuf)));
	}
	ft_dprintf(2, "\n");
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static uint64_t __token_ht_hash(uintptr_t p) { return (p); }
static bool __token_ht_eq(uintptr_t a, uintptr_t b) { return (a == b); }

DEFINE_QHT_HFS(token_ht, uintptr_t, uintptr_t, 128, __token_ht_hash, __token_ht_eq);

static size_t __token_history_len;
static token_ht *__token_history;

static void __asn1_v2_token_history_init(size_t size)
{
	SSL_ALLOCZ(__token_history, size * sizeof(token_ht));
	__token_history_len = size;
}

static uintptr_t __asn1_v2_token_history_get(size_t idx, uintptr_t p)
{
	if (idx < __token_history_len) {
		token_ht *h = __token_history + idx;
		return (token_ht_get_or_default(h, p, 0x0));
	}
	return 0x0;
}

static bool __asn1_v2_token_history_set(size_t idx, uintptr_t p, uintptr_t value)
{
	if (idx < __token_history_len) {
		token_ht *h = __token_history + idx;
		return (token_ht_set(h, p, value));
	}
	return false;
}

static bool __asn1_v2_token_history_unset(size_t idx, uintptr_t p)
{
	if (idx < __token_history_len) {
		token_ht *h = __token_history + idx;
		return (token_ht_unset(h, p));
	}
	return false;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static uint64_t __choice_ht_hash(uintptr_t p) { return (p); }
static bool __choice_ht_eq(uintptr_t a, uintptr_t b) { return (a == b); }

DEFINE_QHT_HFS(choice_ht, uintptr_t, int, 128, __choice_ht_hash, __choice_ht_eq);

static choice_ht *__choice_history;
static size_t __choice_history_len;

static void __asn1_v2_choice_history_init(size_t size)
{
	SSL_ALLOCZ(__choice_history, size * sizeof(choice_ht));
	__choice_history_len = size;
	return;
}

static int __asn1_v2_choice_history_get(size_t idx)
{
	if (idx < __choice_history_len) {
		choice_ht *h = __choice_history + idx;
		return (choice_ht_get_or_default(h, __running_chksum, -1));
	}
	return (-1);
}

static bool __asn1_v2_choice_history_set(size_t idx, int choice_idx)
{
	if (idx < __choice_history_len) {
		choice_ht *h = __choice_history + idx;
		return (choice_ht_set(h, __running_chksum, choice_idx));
	}
	return false;
}

static bool __asn1_v2_choice_history_reset(size_t idx, int choice_idx)
{
	if (idx < __choice_history_len) {
		choice_ht *h = __choice_history + idx;
		return (choice_ht_reset(h, __running_chksum, choice_idx));
	}
	return false;
}

static bool __asn1_v2_choice_history_unset(size_t idx)
{
	if (idx < __choice_history_len) {
		choice_ht *h = __choice_history + idx;
		return (choice_ht_unset(h, __running_chksum));
	}
	return false;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

DEFINE_QHT(pstats_ht, uintptr_t, size_t);

static pstats_ht __pstats;

static void __asn1_v2_parser_stats_init(size_t size)
{
	pstats_ht_init(&__pstats, size);
}

static void __asn1_v2_parser_stats_incr_ncalls(uintptr_t p)
{
	if (pstats_ht_has(&__pstats, p)) {
		size_t ncalls = pstats_ht_get(&__pstats, p);
		(void)pstats_ht_reset(&__pstats, p, ncalls + 1);
	}
	(void)pstats_ht_set(&__pstats, p, 1);
}

static void __asn1_v2_parser_stats_add_ncalls(uintptr_t p, size_t ncalls)
{
	if (pstats_ht_has(&__pstats, p)) {
		size_t cur_ncalls = pstats_ht_get(&__pstats, p);
		(void)pstats_ht_reset(&__pstats, p, cur_ncalls + ncalls);
	}
	(void)pstats_ht_set(&__pstats, p, ncalls);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static bool __asn1_v2_parser_op_run(t_asn_v2_parser_ctx *ctx, const char *name, t_asn_v2_token *token, t_asn_v2_cst **cst);
static void __asn1_v2_parser_op_push_call(t_asn_v2_parser_ctx *ctx);
static void __asn1_v2_parser_op_push_repeat(t_asn_v2_parser_ctx *ctx);
static void __asn1_v2_parser_op_push_unwrap(t_asn_v2_parser_ctx *ctx);
static void __asn1_v2_parser_op_push_wrap(t_asn_v2_parser_ctx *ctx);
static void __asn1_v2_parser_op_push_compact(t_asn_v2_parser_ctx *ctx);
static void __asn1_v2_parser_op_push_optional(t_asn_v2_parser_ctx *ctx);
static void __asn1_v2_parser_op_push_sequence(t_asn_v2_parser_ctx *ctx);
static void __asn1_v2_parser_op_push_choice(t_asn_v2_parser_ctx *ctx);
static void __asn1_v2_parser_op_push_null(t_asn_v2_parser_ctx *ctx);

static t_asn_v2_parser_func __asn1_v2_parser_call_pop(t_asn_v2_parser_ctx *ctx);
static void __asn1_v2_parser_call_push(t_asn_v2_parser_ctx *ctx, t_asn_v2_parser_func func);

static t_asn_v2_parser_op __asn1_v2_parser_op_pop(t_asn_v2_parser_ctx *ctx);
static t_asn_v2_parser_op *__asn1_v2_parser_op_peek(t_asn_v2_parser_ctx *ctx);
static void __asn1_v2_parser_op_push(t_asn_v2_parser_ctx *ctx, t_asn_v2_parser_op op);

static uintptr_t __cur_func_addr = 0x0;
static size_t __total_ncalls;

static bool __asn1_v2_parser_func_call(t_asn_v2_parser_ctx *ctx, const char *name, t_asn_v2_token *token, t_asn_v2_cst **cst, t_asn_v2_parser_func func)
{
	__total_ncalls++;

	uintptr_t old_func_addr = __cur_func_addr;
	uintptr_t old_chksum = __running_chksum;

	__running_chksum += (uintptr_t)func.call;
	__cur_func_addr = (uintptr_t)func.call;

	ctx->params.depth++;
	bool b = func.call(ctx, token, cst);
	ctx->params.depth--;

	__running_chksum = old_chksum;
	__cur_func_addr = old_func_addr;
	return (b);
}

static bool __asn1_v2_parser_op_run(t_asn_v2_parser_ctx *ctx, const char *name, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	__running_chksum++;

	t_asn_v2_parser_op op = __asn1_v2_parser_op_pop(ctx);

	switch (op.kind) {
	case ASN_V2_PARSER_OP_KIND_CALL: {
		t_asn_v2_parser_func func = __asn1_v2_parser_call_pop(ctx);
		t_asn_v2_token call_token = *token;
		bool has_tokens = (call_token.kind != ASN_V2_TOKEN_KIND_EOF);

		if (ctx->params.done || !has_tokens) {
			goto label_error;
		}
		{
			char cbuf[1024];
			SSL_LOG(TRACE, "%*sCALL : %s => %s --- [%s]", ctx->params.depth, "", name, func.name, __asn1_v2_token_dumpb(*token, cbuf, sizeof(cbuf)));
		}

		bool call_result = false;

		uintptr_t p = __asn1_v2_token_history_get(token->idx, __cur_func_addr);
		if (p != 0x0 && p == (uintptr_t)func.call) {
			char cbuf[1024];
			SSL_LOG(CRIT, "call cycle at token: %s, call: %s => %s", __asn1_v2_token_dumpb(*token, cbuf, sizeof(cbuf)), name, func.name);
			SSL_LOG(CRIT, "call stack dump:");
			__asn1_v2_call_stack_dump();
			abort();
		}
		else {
			__asn1_v2_call_history_entry_push(*token, func.name);
			(void)__asn1_v2_token_history_set(token->idx, __cur_func_addr, (uintptr_t)func.call);

			t_asn_v2_parser_ctx call_ctx = {0};
			__asn1_v2_parser_ctx_copy_params(ctx, &call_ctx);

			call_result = __asn1_v2_parser_func_call(&call_ctx, name, &call_token, cst, func);

			(void)__asn1_v2_token_history_unset(token->idx, __cur_func_addr);
			__asn1_v2_call_history_entry_pop();

			if (call_result) {
				*token = call_token;
			}
		}

		if (call_result) {
			__asn1_v2_cst_add_parser_info(*cst, func.name);
		}

		if (call_result) goto label_ok;
		goto label_error;
	}
	case ASN_V2_PARSER_OP_KIND_SEQUENCE: {
		/* op_1 && [op_2 && [op_3 && [...]]] */
		t_asn_v2_token old_token = *token;
		t_asn_v2_cst *root = __asn1_v2_cst_create_list();

		// Push SEQUENCE op back to the top of the stack.
		__asn1_v2_parser_op_push_sequence(ctx);

		bool b = true;
		bool bi = false;
		t_asn_v2_cst *icst = NULL;
		t_asn_v2_parser_op *iop = __asn1_v2_parser_op_peek(ctx);

		if (__global_flags.alternative) {
			// Explore alternative paths (different choices).
			// We are here because upstream parser requested other possible matches if any.
			// NOTE: Each CHOICE has a state that keeps previously selected choice for a given path.
			while (iop->kind == ASN_V2_PARSER_OP_KIND_SEQUENCE) {
				__asn1_v2_parser_op_pop(ctx);

				icst = NULL;
				bi = __asn1_v2_parser_op_run(ctx, name, token, &icst);
				if (bi) {
					__asn1_v2_cst_add_child(root, icst);
				}
				b = b && bi;
				if (!b) {
					// Fail fast.
					ctx->params.done = true;
				}
				iop = __asn1_v2_parser_op_peek(ctx);
				assert(iop != NULL);
			}

			// Handle last op in the sequence.
			icst = NULL;
			bi = __asn1_v2_parser_op_run(ctx, name, token, &icst);
			if (bi) {
				__asn1_v2_cst_add_child(root, icst);
			}
			b = b && bi;
		}

		else /* if (!__global_flags.alternative) */ {
			// Parse with lookahead.
			// Retry previous OP using alternative path if lookahead fails.
			// NOTE: Each CHOICE has a state that keeps previously selected choice for a given path.
			t_asn_v2_parser_ctx ctx_snapshot;
			t_asn_v2_token token_snapshot;
			bool retry = false;
			int retries = 0;

			while (iop->kind == ASN_V2_PARSER_OP_KIND_SEQUENCE) {
				if (!ctx->params.done) {
					token_snapshot = *token;
					__asn1_v2_parser_ctx_copy(ctx, &ctx_snapshot);
				}
				__asn1_v2_parser_op_pop(ctx);

				__global_flags.alternative = false;
				__global_flags.rerouted = false;

				if (retry) {
					SSL_LOG(TRACE, "retry parsing #%d: origin checksum: %#x, parser: %s", retry, __running_chksum, name);

					__global_flags.alternative = true;
					icst = NULL;
					bi = __asn1_v2_parser_op_run(ctx, name, token, &icst);
					if (bi) {
						if (!__global_flags.rerouted) {
							// Downstream did not fulfill reroute request.
							// This means all paths have already been explored in the downstream.
							// Give up.
							bi = false;
							__asn1_v2_cst_delete(icst);
						}
					}
				}
				else {
					icst = NULL;
					bi = __asn1_v2_parser_op_run(ctx, name, token, &icst);
				}

				__global_flags.alternative = false;
				__global_flags.rerouted = false;

				b = b && bi;

				if (b) {
					// Brach out and try parsing remaining tokens. Recover to previous state if parsing fails.
					// If branch succeeds we are done with parsing.

					t_asn_v2_cst *lookahed_cst = NULL;
					t_asn_v2_parser_op *lookahead_iop = __asn1_v2_parser_op_peek(ctx);
					bi = __asn1_v2_parser_op_run(ctx, name, token, &lookahed_cst);

					if (bi) {
						b = b && bi;
						// Add node from previous successful run.
						__asn1_v2_cst_add_child(root, icst);
						// Then append all remaining nodes that must have been parsed by lookahed.
						if (lookahead_iop->kind == ASN_V2_PARSER_OP_KIND_SEQUENCE) {
							__asn1_v2_cst_move_children(root, lookahed_cst);
							__asn1_v2_cst_delete(lookahed_cst);
						}
						else {
							__asn1_v2_cst_add_child(root, lookahed_cst);
						}
						// We are done with parsing of the sequence.
						ctx->params.done = false;
						*cst = root;
						__asn1_v2_cst_add_parser_info(*cst, name);
						goto label_ok;
					}
					else {
						assert(NULL != icst);
						__asn1_v2_cst_delete(icst);

						if (retries > 100) {
							SSL_LOG(CRIT, "fatal: max retries limit reached");
							SSL_LOG(CRIT, "call stack dump:");
							__asn1_v2_call_stack_dump();
							abort();
						}
						else {
							// Recover ctx to the state before last run.
							*token = token_snapshot;
							__asn1_v2_parser_ctx_copy(&ctx_snapshot, ctx);
							retries++;
							retry = true;
						}
					}
				}
				else {
					// Fail fast.
					ctx->params.done = true;
				}

				iop = __asn1_v2_parser_op_peek(ctx);
				if (iop == NULL) break;
			}

			// Handle last op in the sequence.
			__global_flags.alternative = false;
			__global_flags.rerouted = false;
			icst = NULL;
			bi = __asn1_v2_parser_op_run(ctx, name, token, &icst);
			if (bi) {
				__asn1_v2_cst_add_child(root, icst);
			}
			b = b && bi;
		}
		ctx->params.done = false;

		if (b) {
			*cst = root;
			__asn1_v2_cst_add_parser_info(*cst, name);
			goto label_ok;
		}
		assert(*cst == NULL);
		__asn1_v2_cst_delete(root);
		*token = old_token;
		goto label_error;
	}
	case ASN_V2_PARSER_OP_KIND_CHOICE: {
		/* op_1 || [op_2 || [op_3 || [...]]] */
		// Push CHOICE op back to the top of the stack.
		__asn1_v2_parser_op_push_choice(ctx);

		t_asn_v2_token old_token = *token;
		t_asn_v2_cst *choice = NULL;
		t_asn_v2_cst *icst = NULL;
		int chosen_idx = -1;
		int cur_idx = 0;
		bool b = false;
		bool bi = false;

		// The request to choose other alternative may come from upstream.
		// If CHOICE op is ran again on the same token and through the same path we may choose next matching alternative.
		if (__global_flags.alternative) {
			chosen_idx = __asn1_v2_choice_history_get(token->idx); // Will return -1 if never ran before.
		}

		t_asn_v2_parser_op *iop = __asn1_v2_parser_op_peek(ctx);
		while (iop->kind == ASN_V2_PARSER_OP_KIND_CHOICE) {
			__asn1_v2_parser_op_pop(ctx);

			icst = NULL;
			bi = __asn1_v2_parser_op_run(ctx, name, token, &icst);
			if (bi) {
				// Downstream may have chosen to go different path.
				if (__global_flags.alternative
					&& !__global_flags.rerouted // If downstream has already chosen a different path we must not do that again.
				) {
					if (cur_idx > chosen_idx) {
						SSL_LOG(TRACE, "found alternative parsing path: origin checksum: %#x, parser: %s", __running_chksum, name);
						__global_flags.rerouted = true;
						ctx->params.done = true;
						chosen_idx = cur_idx;
						choice = icst;
						// Remember the choice we've made this time for this origin.
						(void)__asn1_v2_choice_history_reset(old_token.idx, chosen_idx);
					}
					else {
						// We must have already explored this call path.
						bi = false;
						assert(NULL != icst);
						__asn1_v2_cst_delete(icst);
					}
				}
				else {
					ctx->params.done = true;
					chosen_idx = cur_idx;
					choice = icst;
					// Set the choice we've made if we haven't done that already.
					(void)__asn1_v2_choice_history_set(old_token.idx, chosen_idx);
				}
			}
			b = b || bi;

			iop = __asn1_v2_parser_op_peek(ctx);
			assert(iop != NULL);
			cur_idx++;
		}

		// Run last op.
		icst = NULL;
		bi = __asn1_v2_parser_op_run(ctx, name, token, &icst);
		if (bi) {
			// Downstream may have chosen to go different path.
			if (__global_flags.alternative
				&& !__global_flags.rerouted // If downstream has already chosen a different path we must not do that again.
			) {
				if (cur_idx > chosen_idx) {
					SSL_LOG(TRACE, "found alternative parsing path: origin checksum: %#x, parser: %s", __running_chksum, name);
					__global_flags.rerouted = true;
					ctx->params.done = true;
					chosen_idx = cur_idx;
					choice = icst;
					// Remember the choice we've made this time for this origin.
					(void)__asn1_v2_choice_history_reset(old_token.idx, chosen_idx);
				}
				else {
					// We must have already explored this call path.
					bi = false;
					assert(NULL != icst);
					__asn1_v2_cst_delete(icst);
				}
			}
			else {
				ctx->params.done = true;
				chosen_idx = cur_idx;
				choice = icst;
				// Set the choice we've made if we haven't done that already.
				(void)__asn1_v2_choice_history_set(old_token.idx, chosen_idx);
			}
		}
		b = b || bi;
		ctx->params.done = false;

		if (b) {
			*cst = choice;
			__asn1_v2_cst_add_parser_info(*cst, name);
			goto label_ok;
		}
		assert(choice == NULL);
		assert(*cst == NULL);
		*token = old_token;
		goto label_error;
	}
	case ASN_V2_PARSER_OP_KIND_OPTIONAL: {
		/* op || TRUE */
		bool b = __asn1_v2_parser_op_run(ctx, name, token, cst);
		if (!b) {
			assert(*cst == NULL);
			*cst = __asn1_v2_cst_create_null();
			__asn1_v2_cst_add_parser_info(*cst, name);
		}
		goto label_ok;
	}
	case ASN_V2_PARSER_OP_KIND_OR_EMPTY: {
		/* op || TRUE */
		bool b = __asn1_v2_parser_op_run(ctx, name, token, cst);
		if (!b) {
			assert(*cst == NULL);
			*cst = __asn1_v2_cst_create_empty();
			__asn1_v2_cst_add_parser_info(*cst, name);
		}
		goto label_ok;
	}
	case ASN_V2_PARSER_OP_KIND_REPEAT: {
		/* op[, op[, op[...]]] */
		t_asn_v2_cst *root = __asn1_v2_cst_create_list();
		t_asn_v2_token old_token = *token;
		bool b = true;
		size_t count = 0;
		while (b) {
			old_token = *token;
			t_asn_v2_cst *icst = NULL;
			t_asn_v2_parser_ctx rctx;
			__asn1_v2_parser_ctx_copy(ctx, &rctx);
			b = __asn1_v2_parser_op_run(&rctx, name, token, &icst);
			if (b) {
				__asn1_v2_cst_add_child(root, icst);
				count++;
				if (__asn1_v2_token_eq(*token, old_token)) {
					// No progress: optional or empty token was parsed.
					break;
				}
			} else {
				assert(icst == NULL);
			}
		}
		if (count > 0) {
			*cst = root;
			goto label_ok;
		}
		__asn1_v2_cst_delete(root);
		*token = old_token;
		goto label_error;
	}
	case ASN_V2_PARSER_OP_KIND_UNWRAP: {
		/* [op_1, op_2, ...] => op_1, op_2, ... */
		t_asn_v2_cst *wrapped = NULL;
		bool b = __asn1_v2_parser_op_run(ctx, name, token, &wrapped);
		if (b) {
			if (NULL != wrapped->base.nodes) {
				*cst = container_of(wrapped->base.nodes, t_asn_v2_cst, base);
				wrapped->base.nodes = NULL;
				__asn1_v2_cst_delete(wrapped);
			}
			else {
				*cst = wrapped;
			}
			goto label_ok;
		}
		goto label_error;
	}
	case ASN_V2_PARSER_OP_KIND_WRAP: {
		/* op_1, op_2, ... => [op_1, op_2, ...] */
		t_asn_v2_cst *to_wrap = NULL;
		bool b = __asn1_v2_parser_op_run(ctx, name, token, &to_wrap);
		if (b) {
			*cst = __asn1_v2_cst_create_wrap();
			(*cst)->base.nodes = &to_wrap->base;
			__asn1_v2_cst_add_parser_info(*cst, name);
			goto label_ok;
		}
		goto label_error;
	}
	case ASN_V2_PARSER_OP_KIND_COMPACT: {
		/* [op_1, op_none, op_3, ...] => [op_1, op_3, ...] */
		t_list list = {0};
		bool b = __asn1_v2_parser_op_run(ctx, name, token, cst);
		if (b) {
			t_node_v2 *nodes = (*cst)->base.nodes;
			(*cst)->base.nodes = NULL;
			t_node_v2 *n = nodes;
			while (NULL != n) {
				t_node_v2 *next = n->next;
				n->next = NULL;
				t_asn_v2_cst *icst = container_of(n, t_asn_v2_cst, base);
				if (icst->kind == ASN_V2_DATA_KIND_NULL || icst->kind == ASN_V2_DATA_KIND_EMPTY) {
					__asn1_v2_cst_delete(icst);
				} else {
					ft_list_append(&list, n);
				}
				n = next;
			}
			(*cst)->base.nodes = list.first;
			goto label_ok;
		}
		goto label_error;
	}
	case ASN_V2_PARSER_OP_KIND_NULL: {
		/* dummy */
		*cst = __asn1_v2_cst_create_null();
		goto label_ok;
	}
	default: {
		UNREACHABLE("__asn1_v2_parser_op_run");
	}}

label_error:
	__running_chksum--;
	return false;

label_ok:
	__running_chksum--;
	return true;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static t_asn_v2_parser_func __asn1_v2_parser_call_pop(t_asn_v2_parser_ctx *ctx)
{
	assert(ctx->func_idx > 0);
	ctx->func_idx--;
	return (ctx->funcs[ctx->func_idx]);
}

static void __asn1_v2_parser_call_push(t_asn_v2_parser_ctx *ctx, t_asn_v2_parser_func func)
{
	assert(ctx->func_idx < ASN_V2_MAX_P_FUNCS);
	ctx->funcs[ctx->func_idx] = func;
	ctx->func_idx++;
}

static t_asn_v2_parser_op __asn1_v2_parser_op_pop(t_asn_v2_parser_ctx *ctx)
{
	assert(ctx->op_idx > 0);
	ctx->op_idx--;
	return (ctx->ops[ctx->op_idx]);
}

static t_asn_v2_parser_op *__asn1_v2_parser_op_peek(t_asn_v2_parser_ctx *ctx)
{
	if (ctx->op_idx == 0) return NULL;
	return (ctx->ops + ctx->op_idx-1);
}

static void __asn1_v2_parser_op_push(t_asn_v2_parser_ctx *ctx, t_asn_v2_parser_op op)
{
	assert(ctx->op_idx < ASN_V2_MAX_P_OPS);
	ctx->ops[ctx->op_idx] = op;
	ctx->op_idx++;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static void __asn1_v2_parser_op_push_call(t_asn_v2_parser_ctx *ctx)
{
	__asn1_v2_parser_op_push(ctx, (t_asn_v2_parser_op){ .kind = ASN_V2_PARSER_OP_KIND_CALL, });
}

static void __asn1_v2_parser_op_push_repeat(t_asn_v2_parser_ctx *ctx)
{
	__asn1_v2_parser_op_push(ctx, (t_asn_v2_parser_op){ .kind = ASN_V2_PARSER_OP_KIND_REPEAT, });
}

static void __asn1_v2_parser_op_push_unwrap(t_asn_v2_parser_ctx *ctx)
{
	__asn1_v2_parser_op_push(ctx, (t_asn_v2_parser_op){ .kind = ASN_V2_PARSER_OP_KIND_UNWRAP, });
}

static void __asn1_v2_parser_op_push_wrap(t_asn_v2_parser_ctx *ctx)
{
	__asn1_v2_parser_op_push(ctx, (t_asn_v2_parser_op){ .kind = ASN_V2_PARSER_OP_KIND_WRAP, });
}

static void __asn1_v2_parser_op_push_compact(t_asn_v2_parser_ctx *ctx)
{
	__asn1_v2_parser_op_push(ctx, (t_asn_v2_parser_op){ .kind = ASN_V2_PARSER_OP_KIND_COMPACT, });
}

static void __asn1_v2_parser_op_push_optional(t_asn_v2_parser_ctx *ctx)
{
	__asn1_v2_parser_op_push(ctx, (t_asn_v2_parser_op){ .kind = ASN_V2_PARSER_OP_KIND_OPTIONAL, });
}

static void __asn1_v2_parser_op_push_or_empty(t_asn_v2_parser_ctx *ctx)
{
	__asn1_v2_parser_op_push(ctx, (t_asn_v2_parser_op){ .kind = ASN_V2_PARSER_OP_KIND_OR_EMPTY, });
}

static void __asn1_v2_parser_op_push_sequence(t_asn_v2_parser_ctx *ctx)
{
	__asn1_v2_parser_op_push(ctx, (t_asn_v2_parser_op){ .kind = ASN_V2_PARSER_OP_KIND_SEQUENCE, });
}

static void __asn1_v2_parser_op_push_choice(t_asn_v2_parser_ctx *ctx)
{
	__asn1_v2_parser_op_push(ctx, (t_asn_v2_parser_op){ .kind = ASN_V2_PARSER_OP_KIND_CHOICE, });
}

static void __asn1_v2_parser_op_push_null(t_asn_v2_parser_ctx *ctx)
{
	__asn1_v2_parser_op_push(ctx, (t_asn_v2_parser_op){ .kind = ASN_V2_PARSER_OP_KIND_NULL, });
}

#define PUSH_OP_CALL(ctx, fn) \
	((void)__asn1_v2_parser_op_push_call((ctx)), \
		(void)__asn1_v2_parser_call_push((ctx), (t_asn_v2_parser_func){ .call = (fn), .name = #fn }))

#define PUSH_OP_OPTIONAL(ctx, op) \
	((void)(op), \
		(void)__asn1_v2_parser_op_push_optional((ctx)))

#define PUSH_OP_OR_EMPTY(ctx, op) \
	((void)(op), \
		(void)__asn1_v2_parser_op_push_or_empty((ctx)))

#define PUSH_OP_REPEAT(ctx, op) \
	((void)(op), \
		(void)__asn1_v2_parser_op_push_repeat((ctx)))

#define PUSH_OP_UNWRAP(ctx, op) \
	((void)(op), \
		(void)__asn1_v2_parser_op_push_unwrap((ctx)))

#define PUSH_OP_WRAP(ctx, op) \
	((void)(op), \
		(void)__asn1_v2_parser_op_push_wrap((ctx)))

#define PUSH_OP_COMPACT(ctx, op) \
	((void)(op), \
		(void)__asn1_v2_parser_op_push_compact((ctx)))

#define PUSH_OP_SEQ_1(ctx, op) \
	PUSH_OP_COMPACT(ctx, \
		((void)(__asn1_v2_parser_op_push_null(ctx)), \
			(void)(op), \
			(void)__asn1_v2_parser_op_push_sequence((ctx)))

#define PUSH_OP_SEQ_2(ctx, left, right) \
	((void)(right), \
		(void)(left), \
		(void)__asn1_v2_parser_op_push_sequence((ctx)))

#define PUSH_OP_SEQ_3(ctx,i1,i2,i3) \
	PUSH_OP_SEQ_2((ctx),(i1),PUSH_OP_SEQ_2((ctx),(i2),(i3)))
#define PUSH_OP_SEQ_4(ctx,i1,i2,i3,i4)\
	PUSH_OP_SEQ_2((ctx),(i1),PUSH_OP_SEQ_3((ctx),(i2),(i3),(i4)))
#define PUSH_OP_SEQ_5(ctx,i1,i2,i3,i4,i5)\
	PUSH_OP_SEQ_2((ctx),(i1),PUSH_OP_SEQ_4((ctx),(i2),(i3),(i4),(i5)))
#define PUSH_OP_SEQ_6(ctx,i1,i2,i3,i4,i5,i6)\
	PUSH_OP_SEQ_2((ctx),(i1),PUSH_OP_SEQ_5((ctx),(i2),(i3),(i4),(i5),(i6)))
#define PUSH_OP_SEQ_7(ctx,i1,i2,i3,i4,i5,i6,i7)\
	PUSH_OP_SEQ_2((ctx),(i1),PUSH_OP_SEQ_6((ctx),(i2),(i3),(i4),(i5),(i6),(i7)))
#define PUSH_OP_SEQ_8(ctx,i1,i2,i3,i4,i5,i6,i7,i8)\
	PUSH_OP_SEQ_2((ctx),(i1),PUSH_OP_SEQ_7((ctx),(i2),(i3),(i4),(i5),(i6),(i7),(i8)))
#define PUSH_OP_SEQ_9(ctx,i1,i2,i3,i4,i5,i6,i7,i8,i9)\
	PUSH_OP_SEQ_2((ctx),(i1),PUSH_OP_SEQ_8((ctx),(i2),(i3),(i4),(i5),(i6),(i7),(i8),(i9)))
#define PUSH_OP_SEQ_10(ctx,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10)\
	PUSH_OP_SEQ_2((ctx),(i1),PUSH_OP_SEQ_9((ctx),(i2),(i3),(i4),(i5),(i6),(i7),(i8),(i9),(i10)))

#define PUSH_OP_CHOICE(ctx, left, right) \
	((void)(right), \
		(void)(left), \
		(void)__asn1_v2_parser_op_push_choice((ctx)))

#define PUSH_OP_CHOICE_3(ctx,i1,i2,i3) \
	PUSH_OP_CHOICE((ctx),(i1),PUSH_OP_CHOICE((ctx),(i2),(i3)))
#define PUSH_OP_CHOICE_4(ctx,i1,i2,i3,i4)\
	PUSH_OP_CHOICE((ctx),(i1),PUSH_OP_CHOICE_3((ctx),(i2),(i3),(i4)))
#define PUSH_OP_CHOICE_5(ctx,i1,i2,i3,i4,i5)\
	PUSH_OP_CHOICE((ctx),(i1),PUSH_OP_CHOICE_4((ctx),(i2),(i3),(i4),(i5)))
#define PUSH_OP_CHOICE_6(ctx,i1,i2,i3,i4,i5,i6)\
	PUSH_OP_CHOICE((ctx),(i1),PUSH_OP_CHOICE_5((ctx),(i2),(i3),(i4),(i5),(i6)))
#define PUSH_OP_CHOICE_7(ctx,i1,i2,i3,i4,i5,i6,i7)\
	PUSH_OP_CHOICE((ctx),(i1),PUSH_OP_CHOICE_6((ctx),(i2),(i3),(i4),(i5),(i6),(i7)))
#define PUSH_OP_CHOICE_8(ctx,i1,i2,i3,i4,i5,i6,i7,i8)\
	PUSH_OP_CHOICE((ctx),(i1),PUSH_OP_CHOICE_7((ctx),(i2),(i3),(i4),(i5),(i6),(i7),(i8)))
#define PUSH_OP_CHOICE_9(ctx,i1,i2,i3,i4,i5,i6,i7,i8,i9)\
	PUSH_OP_CHOICE((ctx),(i1),PUSH_OP_CHOICE_8((ctx),(i2),(i3),(i4),(i5),(i6),(i7),(i8),(i9)))
#define PUSH_OP_CHOICE_10(ctx,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10)\
	PUSH_OP_CHOICE((ctx),(i1),PUSH_OP_CHOICE_9((ctx),(i2),(i3),(i4),(i5),(i6),(i7),(i8),(i9),(i10)))

#define PUSH_OP_SEQ_OF_DELIMITED(ctx, item_op, delim_op) \
	PUSH_OP_SEQ_2(ctx, \
		PUSH_OP_CALL(ctx, item_op), \
		PUSH_OP_OPTIONAL(ctx, \
			PUSH_OP_UNWRAP(ctx, PUSH_OP_REPEAT(ctx, \
				PUSH_OP_UNWRAP(ctx, PUSH_OP_SEQ_2(ctx, \
					PUSH_OP_CALL(ctx, delim_op), \
					PUSH_OP_CALL(ctx, item_op)))))))

#define PUSH_OP_SEQ_OF(ctx, item_op) \
	PUSH_OP_SEQ_2(ctx, \
		PUSH_OP_CALL(ctx, item_op), \
		PUSH_OP_OPTIONAL(ctx, \
			PUSH_OP_UNWRAP(ctx, PUSH_OP_REPEAT(ctx, \
				PUSH_OP_CALL(ctx, item_op)))))

#define RUN_OP(ctx, token, cst) \
	__asn1_v2_parser_op_run((ctx), __func__, (token), (cst))

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static bool __asn1_v2_parse_char(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst, char c)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_SINGLE_CHAR) {
		if (t.s[t.start] == c) {
			if (c == '\n') {
				SSL_LOG(TRACE, "match: SINGLE_CHAR '\\n'");
			} else {
				SSL_LOG(TRACE, "match: SINGLE_CHAR '%c'", c);
			}
			*cst = __asn1_v2_cst_create_leaf(t);
			*token = __tokens[token->idx+1];
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

static bool __asn1_v2_parse_token_L_CURLY_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)  { return (__asn1_v2_parse_char(ctx, token, cst, '{')); }
static bool __asn1_v2_parse_token_R_CURLY_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)  { return (__asn1_v2_parse_char(ctx, token, cst, '}')); }
static bool __asn1_v2_parse_token_L_ANGLE_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)  { return (__asn1_v2_parse_char(ctx, token, cst, '<')); }
static bool __asn1_v2_parse_token_R_ANGLE_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)  { return (__asn1_v2_parse_char(ctx, token, cst, '>')); }
static bool __asn1_v2_parse_token_COMMA(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_char(ctx, token, cst, ',')); }
static bool __asn1_v2_parse_token_DOT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)           { return (__asn1_v2_parse_char(ctx, token, cst, '.')); }
static bool __asn1_v2_parse_token_SLASH(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_char(ctx, token, cst, '/')); }
static bool __asn1_v2_parse_token_L_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)        { return (__asn1_v2_parse_char(ctx, token, cst, '(')); }
static bool __asn1_v2_parse_token_R_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)        { return (__asn1_v2_parse_char(ctx, token, cst, ')')); }
static bool __asn1_v2_parse_token_L_SQUARE_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst) { return (__asn1_v2_parse_char(ctx, token, cst, '[')); }
static bool __asn1_v2_parse_token_R_SQUARE_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst) { return (__asn1_v2_parse_char(ctx, token, cst, ']')); }
static bool __asn1_v2_parse_token_HYPHEN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)        { return (__asn1_v2_parse_char(ctx, token, cst, '-')); }
static bool __asn1_v2_parse_token_COLON(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_char(ctx, token, cst, ':')); }
static bool __asn1_v2_parse_token_EQUAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_char(ctx, token, cst, '=')); }
static bool __asn1_v2_parse_token_DOUBLE_QUOTE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)  { return (__asn1_v2_parse_char(ctx, token, cst, '"')); }
static bool __asn1_v2_parse_token_QUOTE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_char(ctx, token, cst, '\'')); }
static bool __asn1_v2_parse_token_SEMICOLON(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)     { return (__asn1_v2_parse_char(ctx, token, cst, ';')); }
static bool __asn1_v2_parse_token_AT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)            { return (__asn1_v2_parse_char(ctx, token, cst, '@')); }
static bool __asn1_v2_parse_token_PIPE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_char(ctx, token, cst, '|')); }
static bool __asn1_v2_parse_token_BANG(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_char(ctx, token, cst, '!')); }
static bool __asn1_v2_parse_token_CARET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_char(ctx, token, cst, '^')); }
static bool __asn1_v2_parse_token_AMPERSAND(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)     { return (__asn1_v2_parse_char(ctx, token, cst, '&')); }
static bool __asn1_v2_parse_token_ASTERISK(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)      { return (__asn1_v2_parse_char(ctx, token, cst, '*')); }

static bool __asn1_v2_parse_keyword(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst, const char *kw)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_KEYWORD) {
		const char *s = t.s + t.start;
		size_t slen = t.end - t.start;
		size_t kwlen = ft_strlen(kw);
		if (kwlen == slen && ft_strneq(s, kw, kwlen)) {
			SSL_LOG(TRACE, "match: KEYWORD '%s'", kw);
			*cst = __asn1_v2_cst_create_leaf(t);
			*token = __tokens[token->idx+1];
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: KEYWORD '%s': %s", kw, __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_keyword_ABSENT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)           { return (__asn1_v2_parse_keyword(ctx, token, cst, "ABSENT")); }
static bool __asn1_v2_parse_keyword_ENCODED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_keyword(ctx, token, cst, "ENCODED")); }
static bool __asn1_v2_parse_keyword_INTERSECTION(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)     { return (__asn1_v2_parse_keyword(ctx, token, cst, "INTERSECTION")); }
static bool __asn1_v2_parse_keyword_SEQUENCE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_keyword(ctx, token, cst, "SEQUENCE")); }
static bool __asn1_v2_parse_keyword_ABSTRACT_SYNTAX(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)  { return (__asn1_v2_parse_keyword(ctx, token, cst, "ABSTRACT-SYNTAX")); }
static bool __asn1_v2_parse_keyword_ENCODING_CONTROL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst) { return (__asn1_v2_parse_keyword(ctx, token, cst, "ENCODING-CONTROL")); }
static bool __asn1_v2_parse_keyword_ISO646String(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)     { return (__asn1_v2_parse_keyword(ctx, token, cst, "ISO646String")); }
static bool __asn1_v2_parse_keyword_SET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)              { return (__asn1_v2_parse_keyword(ctx, token, cst, "SET")); }
static bool __asn1_v2_parse_keyword_ALL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)              { return (__asn1_v2_parse_keyword(ctx, token, cst, "ALL")); }
static bool __asn1_v2_parse_keyword_END(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)              { return (__asn1_v2_parse_keyword(ctx, token, cst, "END")); }
static bool __asn1_v2_parse_keyword_MAX(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)              { return (__asn1_v2_parse_keyword(ctx, token, cst, "MAX")); }
static bool __asn1_v2_parse_keyword_SETTINGS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_keyword(ctx, token, cst, "SETTINGS")); }
static bool __asn1_v2_parse_keyword_APPLICATION(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)      { return (__asn1_v2_parse_keyword(ctx, token, cst, "APPLICATION")); }
static bool __asn1_v2_parse_keyword_ENUMERATED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)       { return (__asn1_v2_parse_keyword(ctx, token, cst, "ENUMERATED")); }
static bool __asn1_v2_parse_keyword_MIN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)              { return (__asn1_v2_parse_keyword(ctx, token, cst, "MIN")); }
static bool __asn1_v2_parse_keyword_SIZE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)             { return (__asn1_v2_parse_keyword(ctx, token, cst, "SIZE")); }
static bool __asn1_v2_parse_keyword_AUTOMATIC(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)        { return (__asn1_v2_parse_keyword(ctx, token, cst, "AUTOMATIC")); }
static bool __asn1_v2_parse_keyword_EXCEPT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)           { return (__asn1_v2_parse_keyword(ctx, token, cst, "EXCEPT")); }
static bool __asn1_v2_parse_keyword_MINUS_INFINITY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)   { return (__asn1_v2_parse_keyword(ctx, token, cst, "MINUS-INFINITY")); }
static bool __asn1_v2_parse_keyword_STRING(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)           { return (__asn1_v2_parse_keyword(ctx, token, cst, "STRING")); }
static bool __asn1_v2_parse_keyword_BEGIN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)            { return (__asn1_v2_parse_keyword(ctx, token, cst, "BEGIN")); }
static bool __asn1_v2_parse_keyword_EXPLICIT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_keyword(ctx, token, cst, "EXPLICIT")); }
static bool __asn1_v2_parse_keyword_NOT_A_NUMBER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)     { return (__asn1_v2_parse_keyword(ctx, token, cst, "NOT-A-NUMBER")); }
static bool __asn1_v2_parse_keyword_SYNTAX(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)           { return (__asn1_v2_parse_keyword(ctx, token, cst, "SYNTAX")); }
static bool __asn1_v2_parse_keyword_BIT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)              { return (__asn1_v2_parse_keyword(ctx, token, cst, "BIT")); }
static bool __asn1_v2_parse_keyword_EXPORTS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_keyword(ctx, token, cst, "EXPORTS")); }
static bool __asn1_v2_parse_keyword_NULL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)             { return (__asn1_v2_parse_keyword(ctx, token, cst, "NULL")); }
static bool __asn1_v2_parse_keyword_T61String(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)        { return (__asn1_v2_parse_keyword(ctx, token, cst, "T61String")); }
static bool __asn1_v2_parse_keyword_BMPString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)        { return (__asn1_v2_parse_keyword(ctx, token, cst, "BMPString")); }
static bool __asn1_v2_parse_keyword_EXTENSIBILITY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)    { return (__asn1_v2_parse_keyword(ctx, token, cst, "EXTENSIBILITY")); }
static bool __asn1_v2_parse_keyword_NumericString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)    { return (__asn1_v2_parse_keyword(ctx, token, cst, "NumericString")); }
static bool __asn1_v2_parse_keyword_TAGS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)             { return (__asn1_v2_parse_keyword(ctx, token, cst, "TAGS")); }
static bool __asn1_v2_parse_keyword_BOOLEAN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_keyword(ctx, token, cst, "BOOLEAN")); }
static bool __asn1_v2_parse_keyword_EXTERNAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_keyword(ctx, token, cst, "EXTERNAL")); }
static bool __asn1_v2_parse_keyword_OBJECT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)           { return (__asn1_v2_parse_keyword(ctx, token, cst, "OBJECT")); }
static bool __asn1_v2_parse_keyword_TeletexString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)    { return (__asn1_v2_parse_keyword(ctx, token, cst, "TeletexString")); }
static bool __asn1_v2_parse_keyword_BY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)               { return (__asn1_v2_parse_keyword(ctx, token, cst, "BY")); }
static bool __asn1_v2_parse_keyword_FALSE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)            { return (__asn1_v2_parse_keyword(ctx, token, cst, "FALSE")); }
static bool __asn1_v2_parse_keyword_ObjectDescriptor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst) { return (__asn1_v2_parse_keyword(ctx, token, cst, "ObjectDescriptor")); }
static bool __asn1_v2_parse_keyword_TIME(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)             { return (__asn1_v2_parse_keyword(ctx, token, cst, "TIME")); }
static bool __asn1_v2_parse_keyword_CHARACTER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)        { return (__asn1_v2_parse_keyword(ctx, token, cst, "CHARACTER")); }
static bool __asn1_v2_parse_keyword_FROM(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)             { return (__asn1_v2_parse_keyword(ctx, token, cst, "FROM")); }
static bool __asn1_v2_parse_keyword_OCTET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)            { return (__asn1_v2_parse_keyword(ctx, token, cst, "OCTET")); }
static bool __asn1_v2_parse_keyword_TIME_OF_DAY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)      { return (__asn1_v2_parse_keyword(ctx, token, cst, "TIME-OF-DAY")); }
static bool __asn1_v2_parse_keyword_CHOICE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)           { return (__asn1_v2_parse_keyword(ctx, token, cst, "CHOICE")); }
static bool __asn1_v2_parse_keyword_GeneralizedTime(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)  { return (__asn1_v2_parse_keyword(ctx, token, cst, "GeneralizedTime")); }
static bool __asn1_v2_parse_keyword_OF(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)               { return (__asn1_v2_parse_keyword(ctx, token, cst, "OF")); }
static bool __asn1_v2_parse_keyword_TRUE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)             { return (__asn1_v2_parse_keyword(ctx, token, cst, "TRUE")); }
static bool __asn1_v2_parse_keyword_CLASS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)            { return (__asn1_v2_parse_keyword(ctx, token, cst, "CLASS")); }
static bool __asn1_v2_parse_keyword_GeneralString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)    { return (__asn1_v2_parse_keyword(ctx, token, cst, "GeneralString")); }
static bool __asn1_v2_parse_keyword_OID_IRI(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_keyword(ctx, token, cst, "OID-IRI")); }
static bool __asn1_v2_parse_keyword_TYPE_IDENTIFIER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)  { return (__asn1_v2_parse_keyword(ctx, token, cst, "TYPE-IDENTIFIER")); }
static bool __asn1_v2_parse_keyword_COMPONENT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)        { return (__asn1_v2_parse_keyword(ctx, token, cst, "COMPONENT")); }
static bool __asn1_v2_parse_keyword_GraphicString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)    { return (__asn1_v2_parse_keyword(ctx, token, cst, "GraphicString")); }
static bool __asn1_v2_parse_keyword_OPTIONAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_keyword(ctx, token, cst, "OPTIONAL")); }
static bool __asn1_v2_parse_keyword_UNION(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)            { return (__asn1_v2_parse_keyword(ctx, token, cst, "UNION")); }
static bool __asn1_v2_parse_keyword_COMPONENTS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)       { return (__asn1_v2_parse_keyword(ctx, token, cst, "COMPONENTS")); }
static bool __asn1_v2_parse_keyword_IA5String(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)        { return (__asn1_v2_parse_keyword(ctx, token, cst, "IA5String")); }
static bool __asn1_v2_parse_keyword_PATTERN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_keyword(ctx, token, cst, "PATTERN")); }
static bool __asn1_v2_parse_keyword_UNIQUE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)           { return (__asn1_v2_parse_keyword(ctx, token, cst, "UNIQUE")); }
static bool __asn1_v2_parse_keyword_CONSTRAINED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)      { return (__asn1_v2_parse_keyword(ctx, token, cst, "CONSTRAINED")); }
static bool __asn1_v2_parse_keyword_IDENTIFIER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)       { return (__asn1_v2_parse_keyword(ctx, token, cst, "IDENTIFIER")); }
static bool __asn1_v2_parse_keyword_PDV(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)              { return (__asn1_v2_parse_keyword(ctx, token, cst, "PDV")); }
static bool __asn1_v2_parse_keyword_UNIVERSAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)        { return (__asn1_v2_parse_keyword(ctx, token, cst, "UNIVERSAL")); }
static bool __asn1_v2_parse_keyword_CONTAINING(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)       { return (__asn1_v2_parse_keyword(ctx, token, cst, "CONTAINING")); }
static bool __asn1_v2_parse_keyword_IMPLICIT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_keyword(ctx, token, cst, "IMPLICIT")); }
static bool __asn1_v2_parse_keyword_PLUS_INFINITY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)    { return (__asn1_v2_parse_keyword(ctx, token, cst, "PLUS-INFINITY")); }
static bool __asn1_v2_parse_keyword_UniversalString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)  { return (__asn1_v2_parse_keyword(ctx, token, cst, "UniversalString")); }
static bool __asn1_v2_parse_keyword_DATE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)             { return (__asn1_v2_parse_keyword(ctx, token, cst, "DATE")); }
static bool __asn1_v2_parse_keyword_IMPLIED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_keyword(ctx, token, cst, "IMPLIED")); }
static bool __asn1_v2_parse_keyword_PRESENT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_keyword(ctx, token, cst, "PRESENT")); }
static bool __asn1_v2_parse_keyword_UTCTime(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_keyword(ctx, token, cst, "UTCTime")); }
static bool __asn1_v2_parse_keyword_DATE_TIME(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)        { return (__asn1_v2_parse_keyword(ctx, token, cst, "DATE-TIME")); }
static bool __asn1_v2_parse_keyword_IMPORTS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_keyword(ctx, token, cst, "IMPORTS")); }
static bool __asn1_v2_parse_keyword_PrintableString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)  { return (__asn1_v2_parse_keyword(ctx, token, cst, "PrintableString")); }
static bool __asn1_v2_parse_keyword_UTF8String(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)       { return (__asn1_v2_parse_keyword(ctx, token, cst, "UTF8String")); }
static bool __asn1_v2_parse_keyword_DEFAULT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_keyword(ctx, token, cst, "DEFAULT")); }
static bool __asn1_v2_parse_keyword_INCLUDES(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_keyword(ctx, token, cst, "INCLUDES")); }
static bool __asn1_v2_parse_keyword_PRIVATE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_keyword(ctx, token, cst, "PRIVATE")); }
static bool __asn1_v2_parse_keyword_VideotexString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)   { return (__asn1_v2_parse_keyword(ctx, token, cst, "VideotexString")); }
static bool __asn1_v2_parse_keyword_DEFINITIONS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)      { return (__asn1_v2_parse_keyword(ctx, token, cst, "DEFINITIONS")); }
static bool __asn1_v2_parse_keyword_INSTANCE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_keyword(ctx, token, cst, "INSTANCE")); }
static bool __asn1_v2_parse_keyword_REAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)             { return (__asn1_v2_parse_keyword(ctx, token, cst, "REAL")); }
static bool __asn1_v2_parse_keyword_VisibleString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)    { return (__asn1_v2_parse_keyword(ctx, token, cst, "VisibleString")); }
static bool __asn1_v2_parse_keyword_DURATION(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_keyword(ctx, token, cst, "DURATION")); }
static bool __asn1_v2_parse_keyword_INSTRUCTIONS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)     { return (__asn1_v2_parse_keyword(ctx, token, cst, "INSTRUCTIONS")); }
static bool __asn1_v2_parse_keyword_RELATIVE_OID(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)     { return (__asn1_v2_parse_keyword(ctx, token, cst, "RELATIVE-OID")); }
static bool __asn1_v2_parse_keyword_WITH(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)             { return (__asn1_v2_parse_keyword(ctx, token, cst, "WITH")); }
static bool __asn1_v2_parse_keyword_EMBEDDED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)         { return (__asn1_v2_parse_keyword(ctx, token, cst, "EMBEDDED")); }
static bool __asn1_v2_parse_keyword_INTEGER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_keyword(ctx, token, cst, "INTEGER")); }
static bool __asn1_v2_parse_keyword_RELATIVE_OID_IRI(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst) { return (__asn1_v2_parse_keyword(ctx, token, cst, "RELATIVE-OID-IRI")); }
static bool __asn1_v2_parse_keyword_ANY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)              { return (__asn1_v2_parse_keyword(ctx, token, cst, "ANY")); }
static bool __asn1_v2_parse_keyword_DEFINED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)          { return (__asn1_v2_parse_keyword(ctx, token, cst, "DEFINED")); }

static bool __asn1_v2_parse_token_ASSIGNMENT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_ASSIGNMENT) {
		SSL_LOG(TRACE, "match: ASSIGNMENT");
		*cst = __asn1_v2_cst_create_leaf(t);
		*token = __tokens[token->idx+1];
		return true;
	}
	SSL_LOG(TRACE, "no match: ASSIGNMENT: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_token_RANGE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_RANGE) {
		SSL_LOG(TRACE, "match: RANGE");
		*cst = __asn1_v2_cst_create_leaf(t);
		*token = __tokens[token->idx+1];
		return true;
	}
	SSL_LOG(TRACE, "no match: RANGE: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_token_ELLIPSIS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_ELLIPSIS) {
		SSL_LOG(TRACE, "match: ELLIPSIS");
		*cst = __asn1_v2_cst_create_leaf(t);
		*token = __tokens[token->idx+1];
		return true;
	}
	SSL_LOG(TRACE, "no match: ELLIPSIS: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_token_L_VER_BRACKET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_LVER_BRACKET) {
		SSL_LOG(TRACE, "match: LVER_BRACKET");
		*cst = __asn1_v2_cst_create_leaf(t);
		*token = __tokens[token->idx+1];
		return true;
	}
	SSL_LOG(TRACE, "no match: LVER_BRACKET: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_token_R_VER_BRACKET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_RVER_BRACKET) {
		SSL_LOG(TRACE, "match: RVER_BRACKET");
		*cst = __asn1_v2_cst_create_leaf(t);
		*token = __tokens[token->idx+1];
		return true;
	}
	SSL_LOG(TRACE, "no match: RVER_BRACKET: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_bstring(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_BSTRING) {
		SSL_LOG(TRACE, "match: BSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*cst = __asn1_v2_cst_create_leaf(t);
		*token = __tokens[token->idx+1];
		return true;
	}
	SSL_LOG(TRACE, "no match: BSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_hstring(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_HSTRING) {
		SSL_LOG(TRACE, "match: HSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*cst = __asn1_v2_cst_create_leaf(t);
		*token = __tokens[token->idx+1];
		return true;
	}
	SSL_LOG(TRACE, "no match: HSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_cstring(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_CSTRING) {
		SSL_LOG(TRACE, "match: CSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*cst = __asn1_v2_cst_create_leaf(t);
		*token = __tokens[token->idx+1];
		return true;
	}
	SSL_LOG(TRACE, "no match: CSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_tstring(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_CSTRING) {
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
		*cst = __asn1_v2_cst_create_leaf(t);
		*token = __tokens[token->idx+1];
		return true;
	}

label_error:
	SSL_LOG(TRACE, "no match: TSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_number(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_NUMBER) {
		SSL_LOG(TRACE, "match: NUMBER: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*cst = __asn1_v2_cst_create_leaf(t);
		*token = __tokens[token->idx+1];
		return true;
	}
	SSL_LOG(TRACE, "no match: NUMBER: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_realnumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_REAL_NUMBER) {
		SSL_LOG(TRACE, "match: REAL_NUMBER: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*cst = __asn1_v2_cst_create_leaf(t);
		*token = __tokens[token->idx+1];
		return true;
	}
	SSL_LOG(TRACE, "no match: REAL_NUMBER: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_upper_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_NAME) {
		size_t i = t.start;
		while (i < t.end && ft_isalpha_upper(t.s[i])) i++;
		if (i == t.end) {
			SSL_LOG(TRACE, "match: UPPER_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
			*cst = __asn1_v2_cst_create_leaf(t);
			*token = __tokens[token->idx+1];
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: UPPER_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_lower_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_NAME) {
		size_t i = t.start;
		while (i < t.end && ft_isalpha_lower(t.s[i])) i++;
		if (i == t.end) {
			SSL_LOG(TRACE, "match: LOWER_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
			*cst = __asn1_v2_cst_create_leaf(t);
			*token = __tokens[token->idx+1];
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: LOWER_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_upper_first_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_NAME) {
		if (ft_isalpha_upper(t.s[t.start])) {
			SSL_LOG(TRACE, "match: UPPER_FIRST_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
			*cst = __asn1_v2_cst_create_leaf(t);
			*token = __tokens[token->idx+1];
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: UPPER_FIRST_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_lower_first_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_NAME) {
		if (ft_isalpha_lower(t.s[t.start])) {
			SSL_LOG(TRACE, "match: LOWER_FIRST_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
			*cst = __asn1_v2_cst_create_leaf(t);
			*token = __tokens[token->idx+1];
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: LOWER_FIRST_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_integer_unicode_label(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_NUMBER) {
		t.kind = ASN_V2_TOKEN_KIND_INT_UNI_LBL;
		SSL_LOG(TRACE, "match: integerUnicodeLabel: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*cst = __asn1_v2_cst_create_leaf(t);
		*token = __tokens[token->idx+1];
		return true;
	}
	SSL_LOG(TRACE, "no match: integerUnicodeLabel: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_non_integer_unicode_label(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	char cbuf[1024];
	t_asn_v2_token t = __tokens[token->idx];
	UNUSED(ctx);

	if (t.kind == ASN_V2_TOKEN_KIND_NON_INT_UNI_LBL) {
		SSL_LOG(TRACE, "match: non-integerUnicodeLabel: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*cst = __asn1_v2_cst_create_leaf(t);
		*token = __tokens[token->idx+1];
		return true;
	}
	SSL_LOG(TRACE, "no match: non-integerUnicodeLabel: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_empty(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	t_asn_v2_token t = *token;
	t.end = t.start;
	t.kind = ASN_V2_TOKEN_KIND_EMPTY;
	*cst = __asn1_v2_cst_create_leaf(t);
	return true;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static bool __asn1_v2_parse_not_impl_fatal(const char *name)
{
	SSL_LOG(CRIT, "not implemented: %s", name);
	char *stack_dumps = __asn1_v2_call_stack_dumps();
	SSL_LOG(TRACE, "call stack:\n%s", stack_dumps);
	SSL_FREE(stack_dumps);
	// abort();
	return false;
}

static bool __asn1_v2_parse_empty(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_identifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_valuereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_typereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_objectclassreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_objectreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_objectsetreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_typefieldreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_modulereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_xmlasn1typename(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);

static bool __asn1_v2_parse_EncodingReferenceDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TagDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExtensionDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NameForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinitiveNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinitiveNameAndNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinitiveObjIdComponent(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinitiveObjIdComponentList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinitiveOID(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_IRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinitiveOIDAndIRI(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinitiveIdentification(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ModuleIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Reference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Symbol(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SymbolList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SymbolsExported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Exports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_AssignedIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_GlobalModuleReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SelectionOption(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SymbolsFromModule(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SymbolsFromModuleList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SymbolsImported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Imports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExternalTypeReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExternalValueReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ItemId(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ItemSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_AbsoluteReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_xmlasn1typename(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NonParametrizedTypeName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedValueSetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NamedBit(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NamedBitList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_BitStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_BooleanType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_RestrictedCharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_UnrestrictedCharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_CharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_AlternativeTypeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExtensionAdditionAlternativeGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExtensionAdditionAlternative(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExtensionAdditionAlternativeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExtensionAdditionAlternatives(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_AlternativeTypeLists(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ChoiceType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DateType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DateTimeType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DurationType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_EmbeddedPDVType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_EnumerationItem(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Enumeration(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExceptionIdentification(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExceptionSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Enumerations(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_EnumeratedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExternalType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_InstanceOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SignedNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NamedNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NamedNumberList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_IntegerType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_IRIType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NullType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectClassFieldType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectIdentifierType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_OctetStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_RealType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_RelativeIRIType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_RelativeOIDType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExtensionAndException(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_OptionalExtensionMarker(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ComponentType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ComponentTypeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_VersionNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExtensionAdditionGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExtensionAddition(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExtensionAdditionList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExtensionAdditions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ComponentTypeLists(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SequenceType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SequenceOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SetOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ClassNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Class(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_EncodingReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Tag(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TaggedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_EncodingInstruction(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_EncodingPrefix(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_EncodingPrefixedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_PrefixedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TimeType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TimeOfDayType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_BuiltinType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_UsefulType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SelectionType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TypeFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ValueSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ReferencedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SubtypeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ConstraintSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Constraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ConstrainedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NamedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_IdentifierList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_BitStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLBitStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_BooleanValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLBooleanValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Tuple(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Quadruple(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_CharsDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_CharSyms(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_CharacterStringList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_RestrictedCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_UnrestrictedCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_CharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ChoiceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLChoiceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_EmbeddedPDVValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_EnumeratedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLEnumeratedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExternalValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLExternalValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_InstanceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_IntegerValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLIntegerValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_FirstArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SubsequentArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_IRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NullValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLNullValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_OctetStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLOctetStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NameAndNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjIdComponents(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjIdComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NumericRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SpecialRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_RealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_FirstRelativeArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_RelativeIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLRelativeIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_RelativeOIDComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_RelativeOIDComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_RelativeOIDValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLRelativeOIDValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ComponentValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SequenceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLSequenceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NamedValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SequenceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLSequenceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SetValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLSetValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SetOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLSetOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_PrefixedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLPrefixedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TimeValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLTimeValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_BuiltinValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ValueFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ReferencedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectClassFieldValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NamedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLNamedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLTypedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_XMLValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SingleValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Includes(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ContainedSubtype(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_UpperEndValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_LowerEndValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_UpperEndpoint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_LowerEndpoint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ValueRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_PermittedAlphabet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SizeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TypeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_PresenceConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ValueConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ComponentConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_NamedConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_MultipleTypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_InnerTypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_PatternConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_PropertyAndSettingPair(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_PropertySettingsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_PropertySettings(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DurationRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TimePointRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_RecurrenceRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Elements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Elements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Exclusions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Intersections(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_IntersectionMark(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_UnionMark(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Unions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ElementSetSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ElementSetSpecs(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ValueSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ValueSetTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectClassAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectSetAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_AssignmentList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ModuleBody(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Module(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);

static bool __asn1_v2_parse_Literal(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_RequiredToken(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_OptionalGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TokenOrGroupSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SyntaxList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExternalObjectClassReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExternalObjectReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ExternalObjectSetReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_UsefulObjectClassReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinedObjectClass(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinedObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinedObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_PrimitiveFieldName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_FieldName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectSetOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectSetFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ValueSetOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_VariableTypeValueSetFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_FixedTypeValueSetFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ValueOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_VariableTypeValueFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_FixedTypeValueFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TypeOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TypeFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_FieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_WithSyntaxSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_FieldSpecList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectClassDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectClass(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectClassAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Setting(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinedSyntaxToken(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinedSyntaxTokenList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefinedSyntax(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_FieldSettingList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DefaultSyntax(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_FieldSetting(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Object(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectSetSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectSetAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectSetElements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ReferencedObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ValueFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ValueSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_TypeFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_InformationFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ObjectSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);

static bool __asn1_v2_parse_ActualParameter(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ActualParameters(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ActualParameterList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedObjectClass(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedValueSetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SimpleDefinedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_SimpleDefinedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DummyReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_DummyGovernor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Governor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParamGovernor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Parameter(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_Parameters(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedObjectSetAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedObjectAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedObjectClassAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedValueSetTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);
static bool __asn1_v2_parse_ParameterizedTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst);

static bool __asn1_v2_parse_identifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_lower_first_name);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_valuereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_typereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_encodingreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_objectclassreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_name);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_objectreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_objectsetreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_objectfieldreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AMPERSAND),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectreference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_typefieldreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AMPERSAND),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_valuefieldreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AMPERSAND),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_valuesetfieldreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AMPERSAND),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_objectsetfieldreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AMPERSAND),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetreference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_modulereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EncodingReferenceDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// encodingreference INSTRUCTIONS | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INSTRUCTIONS)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TagDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// EXPLICIT TAGS | IMPLICIT TAGS | AUTOMATIC TAGS | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CHOICE_3(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXPLICIT),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IMPLICIT),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_AUTOMATIC)
			),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TAGS)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// EXTENSIBILITY IMPLIED | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXTENSIBILITY),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IMPLIED)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NameForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier
	PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// number
	PUSH_OP_CALL(ctx, __asn1_v2_parse_number);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveNameAndNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier "(" DefinitiveNumberForm ")"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveObjIdComponent(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// NameForm | DefinitiveNumberForm | DefinitiveNameAndNumberForm
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NameForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveNumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveNameAndNumberForm)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveObjIdComponentList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefinitiveObjIdComponent | DefinitiveObjIdComponent DefinitiveObjIdComponentList
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_DefinitiveObjIdComponent);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveOID(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" DefinitiveObjIdComponentList "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveObjIdComponentList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveOIDAndIRI(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefinitiveOID IRIValue
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveOID),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_IRIValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveIdentification(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefinitiveOIDandIRI | DefinitiveOID | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_CHOICE(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveOIDAndIRI),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveOID)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ModuleIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// modulereference DefinitiveIdentification
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveIdentification)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Reference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// typereference | valuereference | objectclassreference | objectreference | objectsetreference
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectclassreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetreference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Symbol(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Reference | ParameterizedReference
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Reference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedReference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Symbol | Symbol "," SymbolList
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_Symbol, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolsExported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SymbolList | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolList)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Exports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// EXPORTS SymbolsExported ";" | EXPORTS ALL ";" | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXPORTS),
			PUSH_OP_CHOICE(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolsExported),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ALL)
			),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SEMICOLON)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_AssignedIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ObjectIdentifierValue | DefinedValue | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_CHOICE(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectIdentifierValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_GlobalModuleReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// modulereference AssignedIdentifier
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_AssignedIdentifier)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SelectionOption(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// empty | WITH "SUCCESSORS" | WITH "DESCENDANTS"
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_WITH),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_name),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolsFromModule(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SymbolList FROM GlobalModuleReference SelectionOption
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_FROM),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_GlobalModuleReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SelectionOption)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolsFromModuleList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SymbolsFromModule | SymbolsFromModuleList SymbolsFromModule
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_SymbolsFromModule);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolsImported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SymbolsFromModuleList | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolsFromModuleList)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Imports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// IMPORTS SymbolsImported ";" | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IMPORTS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolsImported),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SEMICOLON)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalTypeReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// modulereference "." typereference
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalValueReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// modulereference "." valuereference
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ItemId(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ItemSpec
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ItemSpec);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ComponentId(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier | number | "*"
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASTERISK)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ItemSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// typereference | ItemId "." ComponentId
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ItemId),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentId)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_AbsoluteReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "@" ModuleIdentifier "." ItemSpec
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ModuleIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ItemSpec)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_xmlasn1typename(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_NonParametrizedTypeName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ExternalTypeReference | typereference | xmlasn1typename
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalTypeReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_xmlasn1typename)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ExternalTypeReference | typereference | ParameterizedType | ParameterizedValueSetType
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalTypeReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedValueSetType)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ExternalValueReference | valuereference | ParameterizedValue
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalValueReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedBit(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier "(" number ")" | identifier "(" DefinedValue ")"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
		PUSH_OP_CHOICE(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedBitList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// NamedBit | NamedBitList "," NamedBit
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_NamedBit, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BitStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// BIT STRING | BIT STRING "{" NamedBitList "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BIT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_STRING),
		PUSH_OP_OPTIONAL(ctx,
			PUSH_OP_UNWRAP(ctx, PUSH_OP_SEQ_3(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedBitList),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
			))
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BooleanType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BOOLEAN);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RestrictedCharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// See Rec. ITU-T X.680
	// Definition of restricted character string types.

	// BMPString | GeneralString | GraphicString | IA5String | ISO646String | NumericString | PrintableString | TeletexString
	// | T61String | UniversalString | UTF8String | VideotexString | VisibleString
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CHOICE_10(ctx,
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
		PUSH_OP_CHOICE_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UTF8String),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_VideotexString),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_VisibleString)
		)
	);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UnrestrictedCharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CHARACTER),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_STRING)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// RestrictedCharacterStringType | UnrestrictedCharacterStringType
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_RestrictedCharacterStringType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UnrestrictedCharacterStringType)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_AlternativeTypeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// NamedType | AlternativeTypeList "," NamedType
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_NamedType, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionAlternativeGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "[[" VersionNumber AlternativeTypeList "]]"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_VER_BRACKET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_VersionNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_AlternativeTypeList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_VER_BRACKET)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionAlternative(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ExtensionAdditionAlternativesGroup | NamedType
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionAlternativeGroup),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionAlternativeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ExtensionAdditionAlternative | ExtensionAdditionAlternativeList "," ExtensionAdditionAlternative
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_ExtensionAdditionAlternative, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionAlternatives(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "," ExtensionAdditionAlternativeList | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionAlternativeList)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_AlternativeTypeLists(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// RootAlternativeTypeList
	// | RootAlternativeTypeList "," ExtensionAndException ExtensionAdditionAlternatives OptionalExtensionMarker
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_AlternativeTypeList),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
			PUSH_OP_SEQ_4(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionAlternatives),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker)
			)
		))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ChoiceType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// CHOICE "{" AlternativeTypeLists "}"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CHOICE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_AlternativeTypeLists),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DateType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DATE);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DateTimeType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DATE_TIME);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DurationType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DURATION);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EmbeddedPDVType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EMBEDDED),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PDV)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EnumerationItem(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier | NamedNumber
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedNumber)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Enumeration(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// EnumerationItem | EnumerationItem "," Enumeration
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_EnumerationItem, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExceptionIdentification(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SignedNumber | DefinedValue | Type ":" Value
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SignedNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_ExceptionSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "!" ExceptionIdentification | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_BANG),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionIdentification)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Enumerations(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// RootEnumeration
	// | RootEnumeration "," "..." ExceptionSpec
	// | RootEnumeration "," "..." ExceptionSpec "," AdditionalEnumeration
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumeration),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
			PUSH_OP_SEQ_4(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionSpec),
				PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
					PUSH_OP_SEQ_2(ctx,
						PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
						PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumeration)
					)
				))
			)
		))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EnumeratedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ENUMERATED "{" Enumerations "}"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ENUMERATED),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumerations),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXTERNAL);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_InstanceOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// INSTANCE OF DefinedObjectClass
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INSTANCE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SignedNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// number | "-" number
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_token_HYPHEN)),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier "(" SignedNumber ")" | identifier "(" DefinedValue ")"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
		PUSH_OP_CHOICE(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SignedNumber),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedNumberList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// NamedNumber | NamedNumberList "," NamedNumber
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_NamedNumber, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IntegerType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// INTEGER | INTEGER "{" NamedNumberList "}"
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INTEGER),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
			PUSH_OP_SEQ_3(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedNumberList),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
			)
		))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IRIType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OID_IRI);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NullType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_NULL);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectClassFieldType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefinedObjectClass "." FieldName
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectIdentifierType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OBJECT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IDENTIFIER)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_OctetStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OCTET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_STRING)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RealType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_REAL);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeIRIType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_RELATIVE_OID_IRI);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeOIDType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_RELATIVE_OID);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAndException(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "..." | "..." ExceptionSpec
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionSpec))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_OptionalExtensionMarker(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "," "..." | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ComponentType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// NamedType | NamedType OPTIONAL | NamedType DEFAULT Value | COMPONENTS OF Type
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType),
			PUSH_OP_OPTIONAL(ctx, PUSH_OP_CHOICE(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL),
				PUSH_OP_UNWRAP(ctx, PUSH_OP_SEQ_2(ctx,
					PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFAULT),
					PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
				))
			))
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_COMPONENTS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ComponentTypeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ComponentType | ComponentTypeList "," ComponentType
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_ComponentType, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_VersionNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// empty | number ":"
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "[[" VersionNumber ComponentTypeList "]]"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_VER_BRACKET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_VersionNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_VER_BRACKET)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_ExtensionAddition(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ComponentType | ExtensionAdditionGroup
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionGroup)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_ExtensionAdditionList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ExtensionAddition | ExtensionAdditionList "," ExtensionAddition
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_ExtensionAddition, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "," ExtensionAdditionList | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionList)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionEndMarker(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "," "..."
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ComponentTypeLists(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// RootComponentTypeList
	// | RootComponentTypeList "," ExtensionAndException ExtensionAdditions OptionalExtensionMarker
	// | RootComponentTypeList "," ExtensionAndException ExtensionAdditions ExtensionEndMarker "," RootComponentTypeList
	// | ExtensionAndException ExtensionAdditions ExtensionEndMarker "," RootComponentTypeList
	// | ExtensionAndException ExtensionAdditions OptionalExtensionMarker
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList),
			PUSH_OP_OPTIONAL(ctx,
				PUSH_OP_UNWRAP(ctx, PUSH_OP_SEQ_4(ctx,
					PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
					PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
					PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditions),
					PUSH_OP_CHOICE(ctx,
						PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker),
						PUSH_OP_UNWRAP(ctx, PUSH_OP_SEQ_3(ctx,
							PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionEndMarker),
							PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
							PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList)
						))
					)
				))
			)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditions),
			PUSH_OP_CHOICE(ctx,
				PUSH_OP_SEQ_3(ctx,
					PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionEndMarker),
					PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
					PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList)
				),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker)
			)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SequenceType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SEQUENCE "{" "}"
	// | SEQUENCE "{" ExtensionAndException OptionalExtensionMarker "}"
	// | SEQUENCE "{" ComponentTypeLists "}"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_OPTIONAL(ctx,
			PUSH_OP_CHOICE(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeLists),
				PUSH_OP_UNWRAP(ctx, PUSH_OP_SEQ_2(ctx,
					PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
					PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker)
				))
			)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SequenceOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SEQUENCE OF Type | SEQUENCE OF NamedType
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
		PUSH_OP_CHOICE(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SET "{" "}"
	// | SET "{" ExtensionAndException OptionalExtensionMarker "}"
	// | SET "{" ComponentTypeLists "}"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_OPTIONAL(ctx,
			PUSH_OP_CHOICE(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeLists),
				PUSH_OP_UNWRAP(ctx, PUSH_OP_SEQ_2(ctx,
					PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
					PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker)
				))
			)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SetOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SET OF Type | SET OF NamedType
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
		PUSH_OP_CHOICE(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ClassNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// number | DefinedValue
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Class(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// UNIVERSAL | APPLICATION | PRIVATE | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_CHOICE_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UNIVERSAL),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_APPLICATION),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PRIVATE)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EncodingReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// encodingreference ":" | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_encodingreference),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Tag(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "[" EncodingReference Class ClassNumber "]"
	PUSH_OP_SEQ_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_SQUARE_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Class),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ClassNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_SQUARE_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TaggedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Tag Type | Tag IMPLICIT Type | Tag EXPLICIT Type
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Tag),
		PUSH_OP_OPTIONAL(ctx,
			PUSH_OP_CHOICE(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IMPLICIT),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXPLICIT)
			)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EncodingInstruction(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_EncodingPrefix(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "[" EncodingReference EncodingInstruction "]"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_SQUARE_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingInstruction),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_SQUARE_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EncodingPrefixedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// EncodingPrefix Type
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingPrefix),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PrefixedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// TaggedType | EncodingPrefixedType
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TaggedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingPrefixedType)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TimeType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TIME);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TimeOfDayType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TIME_OF_DAY);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

// This is legacy and included only for backward compatibility.
static bool __asn1_v2_parse_AnyType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ANY | ANY DEFINED BY identifier
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ANY),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
			PUSH_OP_SEQ_3(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFINED),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BY),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier)
			)
		))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BuiltinType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// BitStringType | BooleanType | CharacterStringType | ChoiceType | DateType | DateTimeType | DurationType |
	// EmbeddedPDVType | EnumeratedType | ExternalType | InstanceOfType | IntegerType | IRIType | NullType |
	// ObjectClassFieldType | ObjectIdentifierType | OctetStringType | RealType | RelativeIRIType | RelativeOIDType |
	// SequenceType | SequenceOfType | SetType | SetOfType | PrefixedType | TimeType | TimeOfDayType | AnyType

	t_asn_v2_token t = *token;
	const char *s = t.s + t.start;

	if (t.kind == ASN_V2_TOKEN_KIND_KEYWORD) {
		if      (ft_strneq(s, "BIT",              ft_strlen("BIT")              )) PUSH_OP_CALL(ctx, __asn1_v2_parse_BitStringType);
		else if (ft_strneq(s, "OCTET",            ft_strlen("OCTET")            )) PUSH_OP_CALL(ctx, __asn1_v2_parse_OctetStringType);
		else if (ft_strneq(s, "SEQUENCE",         ft_strlen("SEQUENCE")         )) PUSH_OP_CHOICE(ctx,
																						PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceType),
																						PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceOfType));
		else if (ft_strneq(s, "CHOICE",           ft_strlen("CHOICE")           )) PUSH_OP_CALL(ctx, __asn1_v2_parse_ChoiceType);
		else if (ft_strneq(s, "SET",              ft_strlen("SET")              )) PUSH_OP_CHOICE(ctx,
																						PUSH_OP_CALL(ctx, __asn1_v2_parse_SetType),
																						PUSH_OP_CALL(ctx, __asn1_v2_parse_SetOfType));
		else if (ft_strneq(s, "BOOLEAN",          ft_strlen("BOOLEAN")          )) PUSH_OP_CALL(ctx, __asn1_v2_parse_BooleanType);
		else if (ft_strneq(s, "INTEGER",          ft_strlen("INTEGER")          )) PUSH_OP_CALL(ctx, __asn1_v2_parse_IntegerType);
		else if (ft_strneq(s, "OBJECT",           ft_strlen("OBJECT")           )) PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectIdentifierType);
		else if (ft_strneq(s, "RELATIVE_OID_IRI", ft_strlen("RELATIVE_OID_IRI") )) PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeIRIType);
		else if (ft_strneq(s, "RELATIVE_OID",     ft_strlen("RELATIVE_OID")     )) PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeOIDType);
		else if (ft_strneq(s, "NULL",             ft_strlen("NULL")             )) PUSH_OP_CALL(ctx, __asn1_v2_parse_NullType);
		else if (ft_strneq(s, "CHARACTER",        ft_strlen("CHARACTER")        )) PUSH_OP_CALL(ctx, __asn1_v2_parse_CharacterStringType);
		else if (ft_strneq(s, "DATE",             ft_strlen("DATE")             )) PUSH_OP_CALL(ctx, __asn1_v2_parse_DateType);
		else if (ft_strneq(s, "DATE_TIME",        ft_strlen("DATE_TIME")        )) PUSH_OP_CALL(ctx, __asn1_v2_parse_DateTimeType);
		else if (ft_strneq(s, "DURATION",         ft_strlen("DURATION")         )) PUSH_OP_CALL(ctx, __asn1_v2_parse_DurationType);
		else if (ft_strneq(s, "EMBEDDED",         ft_strlen("EMBEDDED")         )) PUSH_OP_CALL(ctx, __asn1_v2_parse_EmbeddedPDVType);
		else if (ft_strneq(s, "ENUMERATED",       ft_strlen("ENUMERATED")       )) PUSH_OP_CALL(ctx, __asn1_v2_parse_EnumeratedType);
		else if (ft_strneq(s, "EXTERNAL",         ft_strlen("EXTERNAL")         )) PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalType);
		else if (ft_strneq(s, "INSTANCE",         ft_strlen("INSTANCE")         )) PUSH_OP_CALL(ctx, __asn1_v2_parse_InstanceOfType);
		else if (ft_strneq(s, "OID_IRI",          ft_strlen("OID_IRI")          )) PUSH_OP_CALL(ctx, __asn1_v2_parse_IRIType);
		else if (ft_strneq(s, "REAL",             ft_strlen("REAL")             )) PUSH_OP_CALL(ctx, __asn1_v2_parse_RealType);
		else if (ft_strneq(s, "TIME",             ft_strlen("TIME")             )) PUSH_OP_CALL(ctx, __asn1_v2_parse_TimeType);
		else if (ft_strneq(s, "TIME_OF_DAY",      ft_strlen("TIME_OF_DAY")      )) PUSH_OP_CALL(ctx, __asn1_v2_parse_TimeOfDayType);
		else if (ft_strneq(s, "ANY",              ft_strlen("ANY")              )) PUSH_OP_CALL(ctx, __asn1_v2_parse_AnyType);
		else return false;
	}
	else if (t.kind == ASN_V2_TOKEN_KIND_SINGLE_CHAR) {
		if (ft_strneq(s, "[", 1)) PUSH_OP_CALL(ctx, __asn1_v2_parse_PrefixedType);
		else return false;
	}
	else {
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClassFieldType);
	}

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;

	// // Full scan.
	// PUSH_OP_CHOICE_3(ctx,
	// 	PUSH_OP_CHOICE_10(ctx,
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_BitStringType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_BooleanType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_CharacterStringType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_ChoiceType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_DateType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_DateTimeType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_DurationType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_EmbeddedPDVType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_EnumeratedType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalType)
	// 	),
	// 	PUSH_OP_CHOICE_10(ctx,
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_InstanceOfType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_IntegerType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_IRIType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_NullType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClassFieldType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectIdentifierType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_OctetStringType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_RealType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeIRIType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeOIDType)
	// 	),
	// 	PUSH_OP_CHOICE_8(ctx,
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceOfType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_SetType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_SetOfType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_PrefixedType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_TimeType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_TimeOfDayType),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_AnyType)
	// 	)
	// );
}

static bool __asn1_v2_parse_UsefulType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// GeneralizedTime | UTCTime | ObjectDescriptor
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_GeneralizedTime),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UTCTime),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ObjectDescriptor)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SelectionType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier "<" Type
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_ANGLE_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ReferencedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefinedType | UsefulType | SelectionType | TypeFromObject | ValueSetFromObjects
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UsefulType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SelectionType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeFromObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSetFromObjects)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SubtypeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ElementSetSpecs
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpecs);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ContentsConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// CONTAINING Type | ENCODED BY Value | CONTAINING Type ENCODED BY Value
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CONTAINING),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
			PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
				PUSH_OP_SEQ_3(ctx,
					PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ENCODED),
					PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BY),
					PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
				)
			))
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ENCODED),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BY),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ComponentIdList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier "." +
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_identifier, __asn1_v2_parse_token_DOT);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Level(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "." Level | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_token_DOT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_AtNotation(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "@" ComponentIdList | "@" "." Level ComponentIdList
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AT),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
			PUSH_OP_SEQ_2(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_Level)
			)
		)),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentIdList)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ComponentRelationConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" DefinedObjectSet "}" "{" AtNotation "," + "}"
	PUSH_OP_SEQ_6(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectSet),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_UNWRAP(ctx,
			PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_AtNotation, __asn1_v2_parse_token_COMMA)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SimpleTableConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ObjectSet
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSet);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TableConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SimpleTableConstraint | ComponentRelationConstraint
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SimpleTableConstraint),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentRelationConstraint)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UserDefinedConstraintParameter(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Governor ":" Value | Governor ":" Object | DefinedObjectSet | Type | DefinedObjectClass
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Governor),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON),
			PUSH_OP_CHOICE(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_Value),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_Object)
			)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectSet),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UserDefinedConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// CONSTRAINED BY "{" UserDefinedConstraintParameter "," * "}"
	PUSH_OP_SEQ_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CONSTRAINED),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BY),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
			PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_UserDefinedConstraintParameter, __asn1_v2_parse_token_COMMA)
		)),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_GeneralConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// UserDefinedConstraint | TableConstraint | ContentsConstraint
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UserDefinedConstraint),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TableConstraint),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ContentsConstraint)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ConstraintSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SubtypeConstraint | GeneralConstraint
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_GeneralConstraint),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SubtypeConstraint)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Constraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "(" ConstraintSpec ExceptionSpec ")"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ConstraintSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeWithConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SET Constraint OF Type
	// | SET Constraint OF NamedType
	// | SET SizeConstraint OF Type
	// | SET SizeConstraint OF NamedType
	// | SEQUENCE Constraint OF Type
	// | SEQUENCE Constraint OF NamedType
	// | SEQUENCE SizeConstraint OF Type
	// | SEQUENCE SizeConstraint OF NamedType
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CHOICE(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE)
		),
		PUSH_OP_CHOICE(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SizeConstraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
		PUSH_OP_CHOICE(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ConstrainedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	/* UNUSED */
	// Type Constraint | TypeWithConstraint
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeWithConstraint)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Type ::= BuiltinType | ReferencedType | ConstrainedType
	// PUSH_OP_CHOICE_3(ctx,
	// 	PUSH_OP_CALL(ctx, __asn1_v2_parse_BuiltinType),
	// 	PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedType),
	// 	PUSH_OP_CALL(ctx, __asn1_v2_parse_ConstrainedType)
	// );

	/* Optimized grammar. */
	// Type ::= BuiltinType Constraint? | ReferencedType Constraint? | TypeWithConstraint

	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_BuiltinType),
			PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint))
		),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedType),
			PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint))
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeWithConstraint)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier Type
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IdentifierList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier | IdentifierList "," identifier
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_identifier, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BitStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// bstring | hstring | "{" IdentifierList "}" | "{" "}" | CONTAINING Value
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_bstring),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_hstring),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_IdentifierList)),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CONTAINING),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLBitStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_BooleanValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// TRUE | FALSE
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TRUE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_FALSE)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLBooleanValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_Tuple(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" TableColumn "," TableRow "," Row "," Cell "}"
	PUSH_OP_SEQ_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Quadruple(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" Group "," Plane "," Row "," Cell "}"
	PUSH_OP_SEQ_9(ctx,
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
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharsDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// cstring | Quadruple | Tuple | DefinedValue
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_cstring),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Quadruple),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Tuple),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharSyms(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// CharsDefn | CharSyms "," CharsDefn
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_CharsDefn, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharacterStringList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" CharSyms "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_CharSyms),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_RestrictedCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// See Rec. ITU-T X.680
	// Definition of restricted character string types.
	// cstring | CharacterStringList | Quadruple | Tuple
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_cstring),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_CharacterStringList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Quadruple),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Tuple)
	);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UnrestrictedCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SequenceValue
	PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// RestrictedCharacterStringValue | UnrestrictedCharacterStringValue
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_RestrictedCharacterStringValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UnrestrictedCharacterStringValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_ChoiceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier ":" Value
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLChoiceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_EmbeddedPDVValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SequenceValue
	PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EnumeratedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier
	PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLEnumeratedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_ExternalValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SequenceValue
	PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLExternalValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_InstanceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Value
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Value);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLInstanceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_IntegerValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SignedNumber | identifier
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SignedNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLIntegerValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_ArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// integerUnicodeLabel | non-integerUnicodeLabel
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_integer_unicode_label),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_non_integer_unicode_label)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FirstArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "/" ArcIdentifier
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SLASH),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ArcIdentifier)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SubsequentArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "/" ArcIdentifier SubsequentArcIdentifier | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SLASH),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ArcIdentifier),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SubsequentArcIdentifier)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// """ FirstArcIdentifier SubsequentArcIdentifier """
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FirstArcIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SubsequentArcIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}


static bool __asn1_v2_parse_NullValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// NULL
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_NULL);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLNullValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}


static bool __asn1_v2_parse_OctetStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// bstring | hstring | CONTAINING Value
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_bstring),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_hstring),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CONTAINING),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLOctetStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_NumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// number | DefinedValue
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NameAndNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier "(" NumberForm ")"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjIdComponents(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// NameForm | NumberForm | NameAndNumberForm | DefinedValue
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NameForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NameAndNumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjIdComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ObjIdComponents | ObjIdComponents ObjIdComponentsList
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_ObjIdComponents);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" ObjIdComponentsList "}" | "{" DefinedValue ObjIdComponentsList "}"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjIdComponentsList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_NumericRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// realnumber | "-" realnumber | SequenceValue
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_token_HYPHEN)),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_realnumber)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_SpecialRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// PLUS-INFINITY | MINUS-INFINITY | NOT-A-NUMBER
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PLUS_INFINITY),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_MINUS_INFINITY),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_NOT_A_NUMBER)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_RealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// NumericRealValue | SpecialRealValue
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NumericRealValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SpecialRealValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_FirstRelativeArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ArcIdentifier
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ArcIdentifier);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// """ FirstRelativeArcIdentifier SubsequentArcIdentifier """
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FirstRelativeArcIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SubsequentArcIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLRelativeIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_RelativeOIDComponents(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// NumberForm | NameAndNumberForm | DefinedValue
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NameAndNumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeOIDComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// RelativeOIDComponents | RelativeOIDComponents RelativeOIDComponentsList
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_RelativeOIDComponents);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeOIDValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" RelativeOIDComponentsList "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeOIDComponentsList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLRelativeOIDValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_ComponentValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// NamedValue | ComponentValueList "," NamedValue
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_NamedValue, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SequenceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" ComponentValueList "}" | "{" "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentValueList)),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLSequenceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_ValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Value | ValueList "," Value
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_Value, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// NamedValue | ComponentValueList "," NamedValue
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_NamedValue, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SequenceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" ValueList "}" | "{" NamedValueList "}" | "{" "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_OPTIONAL(ctx,
			PUSH_OP_CHOICE(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueList),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedValueList)
			)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLSequenceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_SetValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" ComponentValueList "}" | "{" "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentValueList)),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLSetValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_SetOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" ValueList "}" | "{" NamedValueList "}" | "{" "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CHOICE(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedValueList)
		)),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLSetOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_PrefixedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Value
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Value);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_XMLPrefixedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_TimeValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// tstring
	PUSH_OP_CALL(ctx, __asn1_v2_parse_tstring);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLTimeValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

// This is legacy and included only for backward compatibility.
static bool __asn1_v2_parse_AnyValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	/* Corrected grammar that includes COLON between Type and Value tokens. */
	// Type : Value
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BuiltinValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// BitStringValue | BooleanValue | CharacterStringValue | ChoiceValue | EmbeddedPDVValue | EnumeratedValue |
	// ExternalValue | InstanceOfValue | IntegerValue | IRIValue | NullValue | ObjectIdentifierValue | OctetStringValue |
	// RealValue | RelativeIRIValue | RelativeOIDValue | SequenceValue | SequenceOfValue | SetValue | SetOfValue |
	// PrefixedValue | TimeValue | AnyValue
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CHOICE_9(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_BitStringValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_BooleanValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_CharacterStringValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ChoiceValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_EmbeddedPDVValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_EnumeratedValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalValue),
			// PUSH_OP_CALL(ctx, __asn1_v2_parse_InstanceOfValue), // This is effectively a Value
			PUSH_OP_CALL(ctx, __asn1_v2_parse_IntegerValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_IRIValue)
		),
		PUSH_OP_CHOICE_9(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NullValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_OctetStringValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectIdentifierValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_RealValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeIRIValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeOIDValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceOfValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SetValue)
		),
		PUSH_OP_CHOICE_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SetOfValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TimeValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_AnyValue)
			// PUSH_OP_CALL(ctx, __asn1_v2_parse_PrefixedValue) // This is effectively a Value.
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ReferencedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefinedValue | ValueFromObject
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueFromObject)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_OpenTypeFieldVal(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Type ":" Value
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FixedTypeFieldVal(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// BuiltinValue | ReferencedValue
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_BuiltinValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectClassFieldValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// OpenTypeFieldVal | FixedTypeFieldVal
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_OpenTypeFieldVal),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FixedTypeFieldVal)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// BuiltinValue | ReferencedValue | ObjectClassFieldValue
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_BuiltinValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClassFieldValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier Value
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_XMLValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// XMLBuiltinValue | XMLObjectClassFieldValue
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_XMLNamedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "<" & identifier ">" XMLValue "</" & identifier ">"
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_TypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// typereference "::=" Type
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// valuereference Type "::=" Value
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_XMLTypedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_XMLValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// valuereference "::=" XMLTypedValue
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_XMLTypedValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SingleValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Value
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Value);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Includes(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// INCLUDES | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INCLUDES)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_ContainedSubtype(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Includes Type
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Includes),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UpperEndValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Value | MAX
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_MAX),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_LowerEndValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Value | MIN
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_MIN),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UpperEndpoint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// UpperEndValue | "<" UpperEndValue
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_ANGLE_BRKT)),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UpperEndValue)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_LowerEndpoint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// LowerEndValue | LowerEndValue "<"
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_LowerEndValue),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_ANGLE_BRKT))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// LowerEndpoint ".." UpperEndpoint
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_LowerEndpoint),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_RANGE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UpperEndpoint)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PermittedAlphabet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// FROM Constraint
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_FROM),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SizeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SIZE Constraint
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SIZE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Type
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Type);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PresenceConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// PRESENT | ABSENT | OPTIONAL | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_CHOICE_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PRESENT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ABSENT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Constraint | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ComponentConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ValueConstraint PresenceConstraint
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueConstraint),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_PresenceConstraint)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// identifier ComponentConstraint
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentConstraint)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// NamedConstraint | NamedConstraint "," TypeConstraints
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_NamedConstraint, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_MultipleTypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// FullSpecification | PartialSpecification
	// FullSpecification ::= "{" TypeConstraints "}"
	// PartialSpecification ::= "{" "..." "," TypeConstraints "}"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
			PUSH_OP_SEQ_2(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA)
			)
		)),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeConstraints),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_InnerTypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// WITH COMPONENT SingleTypeConstraint
	// | WITH COMPONENTS MultipleTypeConstraints
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_WITH),
		PUSH_OP_CHOICE(ctx,
			PUSH_OP_SEQ_2(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_COMPONENT),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
			),
			PUSH_OP_SEQ_2(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_COMPONENTS),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_MultipleTypeConstraints)
			)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PatternConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// PATTERN Value
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PATTERN),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PropertyAndSettingPair(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// PropertyName "=" SettingName
	// PropertyName ::= psname
	// SettingName ::= psname
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_EQUAL),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PropertySettingsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// PropertyAndSettingPair | PropertySettingsList PropertyAndSettingPair
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_PropertyAndSettingPair);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PropertySettings(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SETTINGS simplestring
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SETTINGS),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_cstring)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DurationRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ValueRange
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueRange);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TimePointRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ValueRange
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueRange);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RecurrenceRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ValueRange
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueRange);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SubtypeElements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
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

	t_asn_v2_token t = *token;
	bool is_keyword = (t.kind == ASN_V2_TOKEN_KIND_KEYWORD);

	if      (is_keyword && __asn1_v2_token_value_eq(t, "SIZE"))     PUSH_OP_CALL(ctx, __asn1_v2_parse_SizeConstraint);
	else if (is_keyword && __asn1_v2_token_value_eq(t, "WITH"))     PUSH_OP_CALL(ctx, __asn1_v2_parse_InnerTypeConstraints);
	else if (is_keyword && __asn1_v2_token_value_eq(t, "PATTERN"))  PUSH_OP_CALL(ctx, __asn1_v2_parse_PatternConstraint);
	else if (is_keyword && __asn1_v2_token_value_eq(t, "SETTINGS")) PUSH_OP_CALL(ctx, __asn1_v2_parse_PropertySettings);
	else if (is_keyword && __asn1_v2_token_value_eq(t, "FROM"))     PUSH_OP_CALL(ctx, __asn1_v2_parse_PermittedAlphabet);
	else {
		PUSH_OP_CHOICE_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SingleValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ContainedSubtype),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueRange),
			// PUSH_OP_CALL(ctx, __asn1_v2_parse_DurationRange),    // Same as ValueRange
			// PUSH_OP_CALL(ctx, __asn1_v2_parse_TimePointRange),   // Same as ValueRange
			// PUSH_OP_CALL(ctx, __asn1_v2_parse_RecurrenceRange)   // Same as ValueRange
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeConstraint)
		);
	}
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Elements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SubtypeElements | ObjectSetElements | "(" ElementSetSpec ")"
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SubtypeElements),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSetElements),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Exclusions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// EXCEPT Elements
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXCEPT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Elements)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IntersectionElements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Elements | Elems Exclusions
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Elements),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_Exclusions))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IntersectionMark(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "^" | INTERSECTION
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_CARET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INTERSECTION)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Intersections(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// IntersectionElements | Intersections IntersectionMark IntersectionElements

	/* Following grammar literally will cause infinite recursion. */

	// PUSH_OP_CHOICE(ctx,
	// 	PUSH_OP_CALL(ctx, __asn1_v2_parse_IntersectionElements),
	// 	PUSH_OP_SEQ_3(ctx,
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_Intersections),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_IntersectionMark),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_IntersectionElements)
	// 	)
	// );

	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_IntersectionElements, __asn1_v2_parse_IntersectionMark);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UnionMark(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "|" | UNION
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_PIPE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UNION)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Unions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Intersections | UElems UnionMark Intersections

	/* Following grammar literally will cause infinite recursion. */

	// PUSH_OP_CHOICE(ctx,
	// 	PUSH_OP_CALL(ctx, __asn1_v2_parse_Intersections),
	// 	PUSH_OP_SEQ_3(ctx,
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_Unions),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_UnionMark),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_Intersections)
	// 	)
	// );

	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_Intersections, __asn1_v2_parse_UnionMark);

	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ElementSetSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Unions | ALL Exclusions
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Unions),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ALL),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Exclusions)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ElementSetSpecs(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// RootElementSetSpec
	// | RootElementSetSpec "," "..."
	// | RootElementSetSpec "," "..." "," AdditionalElementSetSpec

	// PUSH_OP_CHOICE_3(ctx,
	// 	PUSH_OP_SEQ_5(ctx,
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec)
	// 	),
	// 	PUSH_OP_SEQ_3(ctx,
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
	// 		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS)
	// 	),
	// 	PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec)
	// );

	/* Optimized grammar */
	// RootElementSetSpec ("," "..." ("," AdditionalElementSetSpec)?)?

	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
			PUSH_OP_SEQ_3(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
				PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
				PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
					PUSH_OP_SEQ_2(ctx,
						PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
						PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec)
					)
				))
			)
		))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" ElementSetSpecs "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpecs),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueSetTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// typereference Type "::=" ValueSet
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSet)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_XMLFixedTypeFieldVal(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// XMLBuiltinValue
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_XMLOpenTypeFieldVal(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// XMLTypedValue | xmlhstring
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_XMLObjectClassFieldValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// XMLOpenTypeFieldVal | XMLFixedTypeFieldVal
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_ReferencedObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefinedObject | ParameterizedObject | DefinedObjectSet | ParameterizedObjectSet
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectSet),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedObjectSet)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ReferencedObjects "." FieldName
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ReferencedObjects "." FieldName
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ReferencedObjects "." FieldName
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ReferencedObjects "." FieldName
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ReferencedObjects "." FieldName
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_InformationFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ValueFromObject | ValueSetFromObjects | TypeFromObject | ObjectFromObject | ObjectSetFromObjects
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueFromObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSetFromObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeFromObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectFromObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSetFromObjects)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}


static bool __asn1_v2_parse_Literal(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// word | ","
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_name),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RequiredToken(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Literal | PrimitiveFieldName
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Literal),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_PrimitiveFieldName)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_OptionalGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "[" TokenOrGroupSpec empty + "]"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_SQUARE_BRKT),
		PUSH_OP_UNWRAP(ctx,
			PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_TokenOrGroupSpec)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_SQUARE_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TokenOrGroupSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// RequiredToken | OptionalGroup
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_RequiredToken),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalGroup)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SyntaxList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" TokenOrGroupSpec empty + "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_UNWRAP(ctx,
			PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_TokenOrGroupSpec)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalObjectClassReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// modulereference "." objectclassreference
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectclassreference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalObjectReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// modulereference "." objectreference
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectreference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalObjectSetReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// modulereference "." objectsetreference
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetreference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UsefulObjectClassReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// TYPE-IDENTIFIER | ABSTRACT-SYNTAX
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TYPE_IDENTIFIER),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ABSTRACT_SYNTAX)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedObjectClass(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ExternalObjectClassReference | objectclassreference | UsefulObjectClassReference
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalObjectClassReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectclassreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UsefulObjectClassReference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ExternalObjectReference | objectreference
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalObjectReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectreference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ExternalObjectSetReference | objectsetreference
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalObjectSetReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetreference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PrimitiveFieldName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// typefieldreference | valuefieldreference | valuesetfieldreference | objectfieldreference | objectsetfieldreference
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typefieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuefieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuesetfieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectfieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetfieldreference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FieldName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// PrimitiveFieldName "." +
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_PrimitiveFieldName, __asn1_v2_parse_token_DOT);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSetOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// OPTIONAL | DEFAULT ObjectSet
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFAULT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSet)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSetFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// objectsetfieldreference DefinedObjectClass ObjectSetOptionalitySpec?
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetfieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSetOptionalitySpec))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// OPTIONAL | DEFAULT Object
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFAULT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Object)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// objectfieldreference DefinedObjectClass ObjectOptionalitySpec?
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectfieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectOptionalitySpec))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueSetOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// OPTIONAL | DEFAULT ValueSet
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFAULT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSet)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_VariableTypeValueSetFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// valuesetfieldreference FieldName ValueSetOptionalitySpec?
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuesetfieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSetOptionalitySpec))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FixedTypeValueSetFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// valuesetfieldreference Type ValueSetOptionalitySpec?
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuesetfieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSetOptionalitySpec))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// OPTIONAL | DEFAULT Value
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFAULT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_VariableTypeValueFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// valuefieldreference FieldName ValueOptionalitySpec?
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuefieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueOptionalitySpec))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FixedTypeValueFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// valuefieldreference Type UNIQUE? ValueOptionalitySpec?
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuefieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UNIQUE)),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueOptionalitySpec))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// OPTIONAL | DEFAULT Type
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFAULT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// typefieldreference TypeOptionalitySpec?
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typefieldreference),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeOptionalitySpec))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// TypeFieldSpec | FixedTypeValueFieldSpec | VariableTypeValueFieldSpec | FixedTypeValueSetFieldSpec
	// | VariableTypeValueSetFieldSpec | ObjectFieldSpec | ObjectSetFieldSpec
	PUSH_OP_CHOICE_7(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeFieldSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FixedTypeValueFieldSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_VariableTypeValueFieldSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FixedTypeValueSetFieldSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_VariableTypeValueSetFieldSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectFieldSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSetFieldSpec)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_WithSyntaxSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// WITH SYNTAX SyntaxList
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_WITH),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SYNTAX),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SyntaxList)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FieldSpecList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// FieldSpec | FieldSpec "," FieldSpecList
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_FieldSpec, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectClassDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// CLASS "{" FieldSpec "," + "}" WithSyntaxSpec?
	PUSH_OP_SEQ_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CLASS),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldSpecList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_WithSyntaxSpec))
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectClass(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefinedObjectClass | ObjectClassDefn | ParameterizedObjectClass
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClassDefn),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedObjectClass)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectClassAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// objectclassreference "::=" ObjectClass
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectclassreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClass)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Setting(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Type | Value | ValueSet | Object | ObjectSet
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSet),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Object),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSet)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedSyntaxToken(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Literal | Setting
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Literal),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Setting)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedSyntaxTokenList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefinedSyntaxToken | DefinedSyntaxToken DefinedSyntaxTokenList
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_DefinedSyntaxToken);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedSyntax(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" DefinedSyntaxToken empty * "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedSyntaxTokenList)
		)),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FieldSettingList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// FieldSetting | FieldSetting "," FieldSettingList
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_FieldSetting, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefaultSyntax(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" FieldSetting "," * "}"
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
				PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldSettingList)
			)),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FieldSetting(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// PrimitiveFieldName Setting
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_PrimitiveFieldName),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Setting)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefaultSyntax | DefinedSyntax
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefaultSyntax),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedSyntax)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Object(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefinedObject | ObjectDefn | ObjectFromObject | ParameterizedObject
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectDefn),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectFromObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedObject)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// objectreference DefinedObjectClass "::=" Object
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Assignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Object)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSetElements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Object | DefinedObjectSet | ObjectSetFromObjects | ParameterizedObjectSet
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Object),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectSet),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSetFromObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedObjectSet)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSetSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// RootElementSetSpec
	// | RootElementSetSpec "," "..."
	// | RootElementSetSpec "," "..." "," AdditionalElementSetSpec
	// | "..." "," AdditionalElementSetSpec
	// | "..."
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_SEQ_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" ObjectSetSpec "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSetSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSetAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// objectsetreference DefinedObjectClass "::=" ObjectSet
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Assignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSet)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ActualParameter(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Type | Value | ValueSet | DefinedObjectClass | Object | ObjectSet
	PUSH_OP_CHOICE_6(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSet),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Object),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSet)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ActualParameters(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Parameter | Parameter "," Parameters
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_ActualParameter, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ActualParameterList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" ActualParameter "," + "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_UNWRAP(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameters)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefinedObject ActualParameterList
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameterList)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefinedObjectSet ActualParameterList
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectSet),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameterList)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedObjectClass(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DefinedObjectClass ActualParameterList
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameterList)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedValueSetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SimpleDefinedType ActualParameterList
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SimpleDefinedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameterList)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SimpleDefinedValue ActualParameterList
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SimpleDefinedValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameterList)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// SimpleDefinedType ActualParameterList
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SimpleDefinedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameterList)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SimpleDefinedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ExternalValueReference | valuereference
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalValueReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SimpleDefinedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ExternalTypeReference | typereference
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalTypeReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Reference | Reference "{" "}"
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Reference),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Reference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DummyReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Reference
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Reference);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DummyGovernor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// DummyReference
	PUSH_OP_CALL(ctx, __asn1_v2_parse_DummyReference);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Governor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Type | DefinedObjectClass
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParamGovernor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Governor | DummyGovernor
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Governor),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DummyGovernor)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Parameter(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ParamGovernor ":" DummyReference | DummyReference
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ParamGovernor),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DummyReference)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DummyReference)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Parameters(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Parameter | Parameter "," Parameters
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_Parameter, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// "{" Parameter "," + "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_UNWRAP(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Parameters)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedObjectSetAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// objectsetreference ParameterList DefinedObjectClass "::=" ObjectSet
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSet)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedObjectAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// objectreference ParameterList DefinedObjectClass "::=" Object
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Object)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedObjectClassAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// objectclassreference ParameterList "::=" ObjectClass
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectclassreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClass)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedValueSetTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// typereference ParameterList Type "::=" ValueSet
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSet)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// valuereference ParameterList Type "::=" Value
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// typereference ParameterList "::=" Type
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ParameterizedTypeAssignment | ParameterizedValueAssignment | ParameterizedValueSetTypeAssignment
	// | ParameterizedObjectClassAssignment | ParameterizedObjectAssignment | ParameterizedObjectSetAssignment
	PUSH_OP_CHOICE_6(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedTypeAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedValueAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedValueSetTypeAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedObjectClassAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedObjectAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedObjectSetAssignment)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// TypeAssignment | ValueAssignment | XMLValueAssignment | ValueSetTypeAssignment |
	// ObjectClassAssignment | ObjectAssignment | ObjectSetAssignment | ParameterizedAssignment
	PUSH_OP_CHOICE_8(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_XMLValueAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSetTypeAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClassAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSetAssignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedAssignment)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_AssignmentList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Assignment | AssignmentList Assignment
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_Assignment);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ModuleBody(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// Exports Imports AssignmentList | empty
	PUSH_OP_OR_EMPTY(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Exports),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Imports),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_AssignmentList)
		)
	);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ModuleDefinition(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ModuleIdentifier DEFINITIONS EncodingReferenceDefault TagDefault ExtensionDefault "::="
	// BEGIN ModuleBody EncodingControlSections END
	PUSH_OP_SEQ_9(ctx,
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
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_root(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_cst **cst)
{
	// ModuleDefinition | ModuleDefinition ModuleDefinitionList
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_ModuleDefinition);
	if (RUN_OP(ctx, token, cst)) {
		return true;
	}
	return false;
}

int asn1_v2_parse(const char *s, size_t slen, t_asn_v2_cst **cst)
{
	if (NULL == s) return (SSL_ERR);

	__asn1_v2_token_array_init(slen);

	t_asn_v2_token token = {.s = s, .slen = slen,};
	size_t idx = 0;
	while (__asn1_v2_token_next(&token)) {
		__tokens[idx] = token;
		__tokens[idx].idx = idx;
		idx++;
	}
	__tokens[idx] = (t_asn_v2_token){.s = s, .slen = slen, .start = slen, .end = slen, .kind = ASN_V2_TOKEN_KIND_EOF};
	__token_count = idx;

	__asn1_v2_token_history_init(__token_count);
	__asn1_v2_choice_history_init(__token_count);

	t_asn_v2_parser_ctx ctx = {0};
	token = __tokens[0];

	bool ok = __asn1_v2_parse_root(&ctx, &token, cst);

	SSL_FREE(__tokens);
	SSL_FREE(__token_history);
	SSL_FREE(__choice_history);

	if (!ok) return (SSL_ERR);

	if (token.kind != ASN_V2_TOKEN_KIND_EOF) {
		SSL_LOG(ERROR, "parsing is not complete at pos: %zu", token.start);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static char *__asn1_v2_cst_node_dumps(const t_node_v2 *node, int depth)
{
	if (NULL == node) return (ft_strdup("null"));
	t_asn_v2_cst *cst = container_of(node, t_asn_v2_cst, base);
	char *dumps = NULL;
	if (NULL != cst->token) {
		char *toks = __asn1_v2_token_dumps(*cst->token);
		ft_asprintf(&dumps, "%*skind=%s,parser=%s,token=%s", depth, "", __asn1_v2_get_cst_kind_name(cst->kind), cst->parser, toks);
	}
	else {
		ft_asprintf(&dumps, "%*skind=%s,parser=%s", depth, "", __asn1_v2_get_cst_kind_name(cst->kind), cst->parser);
	}
	return (dumps);
}

static void __asn1_v2_cst_node_dump(const t_node_v2 *node, int depth)
{
	if (NULL == node) return;
	t_asn_v2_cst *cst = container_of(node, t_asn_v2_cst, base);
	t_asn_v2_token *t = cst->token;
	if (NULL != t && t->end >= t->start) {
		ft_fprintf(&io_v2_buffered_stdout_writer, "%*skind=%s,parser=%s,token(%s)=%.*s\n",
			depth, "", __asn1_v2_get_cst_kind_name(cst->kind), cst->parser, __asn1_v2_get_token_kind_name(t->kind), (int)(t->end-t->start), t->s+t->start);
	}
	else {
		ft_fprintf(&io_v2_buffered_stdout_writer, "%*skind=%s,parser=%s\n", depth, "", __asn1_v2_get_cst_kind_name(cst->kind), cst->parser);
	}
}

char *asn1_v2_cst_dumps(const t_asn_v2_cst *cst)
{
	return (ft_ntree_v2_dumps(&cst->base, __asn1_v2_cst_node_dumps));
}

void asn1_v2_cst_dump(const t_asn_v2_cst *cst)
{
	ft_ntree_v2_dump(&cst->base, __asn1_v2_cst_node_dump);
	io_v2_flush(&io_v2_buffered_stdout_writer);
}
