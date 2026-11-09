#include <common.h>
#include <logger.h>
#include <asn1_v2.h>
#include <libft.h>

static const char *__asn1_v2_get_ast_kind_name(t_asn_v2_ast_kind kind)
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
	default:								return "unknown";
	}
}

typedef struct s_asn_v2_parser_ctx {
	t_list ops;
	t_list funcs;
	bool done;
} t_asn_v2_parser_ctx;

typedef bool (*t_func_asn_v2_parse)(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);

typedef enum e_asn_v2_parser_op_kind {
	ASN_V2_PARSER_OP_KIND_NONE = 0,
	ASN_V2_PARSER_OP_KIND_CALL,
	ASN_V2_PARSER_OP_KIND_OPTIONAL,
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
	case ASN_V2_PARSER_OP_KIND_NONE: return ("NONE");
	case ASN_V2_PARSER_OP_KIND_CALL: return ("CALL");
	case ASN_V2_PARSER_OP_KIND_OPTIONAL: return ("OPTIONAL");
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
	{"AMPERSAND",     '&'},
};
const size_t SINGLE_CHAR_LEX_COUNT = sizeof(SINGLE_CHAR_LEXES)/sizeof(*SINGLE_CHAR_LEXES);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static inline const char *__asn1_v2_strip_parse_func_prefix(const char *name)
{
	const char prefix[16] = "__asn1_v2_parse_";
    return (ft_strncmp(name, prefix, sizeof(prefix)) == 0 ? name + sizeof(prefix) : name);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

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
				if (__asn1_v2_token_is_bound(s[i])) {
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
				if (__asn1_v2_token_is_bound(s[i])) {
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

static char *__asn1_v2_token_dumps(t_asn_v2_token token)
{
	assert(token.end >= token.start);
	char *toks = ft_strsub(token.s, token.start, token.end-token.start);
	char *dumps = NULL;
	ft_sprintf(&dumps, "\"%s\"(%s)", toks, __asn1_v2_get_token_kind_name(token.kind));
	return (dumps);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static t_asn_v2_ast *__asn1_v2_ast_create(void)
{
	t_asn_v2_ast *ast = NULL;
	SSL_ALLOC(ast, sizeof(t_asn_v2_ast));
	*ast = (t_asn_v2_ast){0};
	return (ast);
}

static t_asn_v2_ast *__asn1_v2_ast_create_leaf(t_asn_v2_token token)
{
	t_asn_v2_ast *ast = __asn1_v2_ast_create();
	ast->kind = ASN_V2_DATA_KIND_LEAF;
	ast->token = __asn1_v2_token_clone(&token);
	return (ast);
}

static t_asn_v2_ast *__asn1_v2_ast_create_list(void)
{
	t_asn_v2_ast *ast = __asn1_v2_ast_create();
	ast->kind = ASN_V2_DATA_KIND_LIST;
	return (ast);
}

static t_asn_v2_ast *__asn1_v2_ast_create_wrap(void)
{
	t_asn_v2_ast *ast = __asn1_v2_ast_create();
	ast->kind = ASN_V2_DATA_KIND_WRAP;
	return (ast);
}

static t_asn_v2_ast *__asn1_v2_ast_create_null(void)
{
	t_asn_v2_ast *ast = __asn1_v2_ast_create();
	ast->kind = ASN_V2_DATA_KIND_NULL;
	return (ast);
}

static void __asn1_v2_ast_add_parser_info(t_asn_v2_ast *ast, const char *func_name)
{
	ast->parser = __asn1_v2_strip_parse_func_prefix(func_name);
}

static void __asn1_v2_ast_add_child(t_asn_v2_ast *root, t_asn_v2_ast *child)
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

static void __asn1_v2_ast_move_children(t_asn_v2_ast *dst, t_asn_v2_ast *src)
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

static void __asn1_v2_ast_delete_adapter(t_node_v2 *node)
{
	t_asn_v2_ast *ast = container_of(node, t_asn_v2_ast, base);
	SSL_FREE(ast);
}

static void __asn1_v2_ast_delete(t_asn_v2_ast *ast)
{
	ft_ntree_v2_del(&ast->base, __asn1_v2_ast_delete_adapter);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static t_asn_v2_parser_op *__asn1_v2_parser_op_create(void)
{
	t_asn_v2_parser_op *op = NULL;
	SSL_ALLOC(op, sizeof(t_asn_v2_parser_op));
	return (op);
}
static void __asn1_v2_parser_op_delete(t_asn_v2_parser_op *op)
{
	SSL_FREE(op);
}
static void __asn1_v2_parser_op_copy(const t_asn_v2_parser_op *src, t_asn_v2_parser_op *dst)
{
	*dst = *src;
}

static t_asn_v2_parser_func *__asn1_v2_parser_func_create(void)
{
	t_asn_v2_parser_func *func = NULL;
	SSL_ALLOC(func, sizeof(t_asn_v2_parser_func));
	return (func);
}
static void __asn1_v2_parser_func_delete(t_asn_v2_parser_func *func)
{
	SSL_FREE(func);
}
static void __asn1_v2_parser_func_copy(const t_asn_v2_parser_func *src, t_asn_v2_parser_func *dst)
{
	*dst = *src;
}

static void __asn1_v2_parser_op_delete_adapter(void *p) { __asn1_v2_parser_op_delete((t_asn_v2_parser_op *)p); }
static void __asn1_v2_parser_func_delete_adapter(void *p) { __asn1_v2_parser_func_delete((t_asn_v2_parser_func *)p); }

static void *__asn1_v2_parser_op_copy_adapter(const void *p)
{
	const t_asn_v2_parser_op *src = p;
	t_asn_v2_parser_op *dst = __asn1_v2_parser_op_create();
	__asn1_v2_parser_op_copy(src, dst);
	return (dst);
}
static void *__asn1_v2_parser_func_copy_adapter(const void *p)
{
	const t_asn_v2_parser_func *src = p;
	t_asn_v2_parser_func *dst = __asn1_v2_parser_func_create();
	__asn1_v2_parser_func_copy(src, dst);
	return (dst);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static void __asn1_v2_parser_ctx_copy(const t_asn_v2_parser_ctx *src, t_asn_v2_parser_ctx *dst)
{
	dst->done = src->done;
	ft_list_copy_all_content(&src->ops, &dst->ops, __asn1_v2_parser_op_copy_adapter);
	ft_list_copy_all_content(&src->funcs, &dst->funcs, __asn1_v2_parser_func_copy_adapter);
}

static void __asn1_v2_parser_ctx_clear(t_asn_v2_parser_ctx *ctx)
{
	ft_list_clear_all_content(&ctx->ops, __asn1_v2_parser_op_delete_adapter);
	ft_list_clear_all_content(&ctx->funcs, __asn1_v2_parser_func_delete_adapter);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

typedef struct s_call_h_entry {
	const char *name;
	t_asn_v2_token token;
} t_call_h_entry;

static t_list __call_history = {0};

static char *__asn1_v2_call_stack_dumps(void)
{
	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 2048);
	t_list_next next = {0};
	void *content = NULL;
	while (ft_list_next_content(&__call_history, &next, &content)) {
		t_call_h_entry *e = content;
		ft_ostr_appendf(&ostring, "    %-40s\n", e->name);
	}
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

static size_t __token_history_len;
static t_htbl_v2 *__token_history;

static void __asn1_v2_token_history_init(size_t slen)
{
	SSL_ALLOC(__token_history, slen * sizeof(t_htbl_v2));
	__token_history_len = slen;
	return;
}

static bool __asn1_v2_token_history_set(size_t idx, const char *name, bool boolean)
{
	if (idx < __token_history_len) {
		t_htbl_v2 *h = __token_history + idx;
		if (h->size == 0) ft_htbl_v2_init(h, 1024);
		return (ft_htbl_v2_set(h, name, (void *)boolean));
	}
	return false;
}

static bool __asn1_v2_token_history_unset(size_t idx, const char *name)
{
	if (idx < __token_history_len) {
		t_htbl_v2 *h = __token_history + idx;
		if (h->size > 0) {
			return (ft_htbl_v2_unset(h, name, NULL));
		}
	}
	return false;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static bool __asn1_v2_parser_op_run(t_asn_v2_parser_ctx *ctx, const char *name, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	t_asn_v2_parser_op *op = ft_list_pop_content(&ctx->ops);
	if (NULL == op) return false;

	switch (op->kind) {
	case ASN_V2_PARSER_OP_KIND_CALL: {
		t_asn_v2_parser_func *func = ft_list_pop_content(&ctx->funcs);
		t_asn_v2_token cur_token = *token;

		if (ctx->done) {
			__asn1_v2_parser_func_delete(func);
			return false;
		}
		SSL_LOG(TRACE, "CALL : %s => %s", name, func->name);

		bool skip_call = false;
		bool call_result = false;

		if (__call_history.size > 0) {
			t_list tmp_list = {0};
			t_call_h_entry *be = NULL;
			while (__call_history.size > 0) {
				be = ft_list_pop_content(&__call_history);
				ft_list_prepend_ref(&tmp_list, be);
				if (cur_token.start == be->token.start && ft_streq(func->name, be->name)) {
					skip_call = true;
					break;
				}
			}
			while (tmp_list.size > 0) {
				be = ft_list_pop_ref(&tmp_list);
				ft_list_prepend_content(&__call_history, be);
			}
		}

		if (!skip_call) {
			t_call_h_entry *e = NULL;
			SSL_ALLOC(e, sizeof(t_call_h_entry));
			e->name = func->name;
			e->token = cur_token;
			ft_list_prepend_content(&__call_history, e);

			bool no_cycle = __asn1_v2_token_history_set(token->start, func->name, true);
			if (no_cycle) {
				t_asn_v2_parser_ctx call_ctx = {0};
				call_result = func->call(&call_ctx, token, ast);
			}
			else {
				// Fail early when cyclical call dependency is detected.
				char cbuf[1024];
				SSL_LOG(CRIT, "cyclical call dependency: last consumed token: %s, func: %s", __asn1_v2_token_dumpb(cur_token, cbuf, sizeof(cbuf)), func->name);
				char *stack_dumps = __asn1_v2_call_stack_dumps();
				SSL_LOG(CRIT, "call stack:\n%s", stack_dumps);
				SSL_FREE(stack_dumps);
				abort();
			}
			if (!__asn1_v2_token_history_unset(cur_token.start, func->name)) {
				UNREACHABLE("__asn1_v2_parser_op_run");
			}

			e = ft_list_pop_content(&__call_history);
			SSL_FREE(e);
		}
		else {
			char cbuf[1024];
			SSL_LOG(TRACE, "skipping call: last consumed token: %s, func: %s", __asn1_v2_token_dumpb(cur_token, cbuf, sizeof(cbuf)), func->name);
		}

		// if (!call_result && op->kind == ASN_V2_PARSER_OP_KIND_OPTIONAL) {
		// 	call_result = true;
		// }
		if (call_result) {
			__asn1_v2_ast_add_parser_info(*ast, func->name);
		}
		__asn1_v2_parser_func_delete(func);

		return (call_result);
	}
	case ASN_V2_PARSER_OP_KIND_SEQUENCE: {
		t_asn_v2_token old_token = *token;
		t_asn_v2_ast *root = __asn1_v2_ast_create_list();

		// char cbuf[1024];
		// SSL_LOG(WARN, "iop:first:%p:token:%s", op, __asn1_v2_token_dumpb(*token, cbuf, sizeof(cbuf)));

		t_asn_v2_ast *iast = NULL;
		bool b = __asn1_v2_parser_op_run(ctx, name, token, &iast);
		if (!ctx->done) ctx->done = !b;
		if (b) {
			__asn1_v2_ast_add_child(root, iast);
		}

		t_asn_v2_parser_op *iop = ft_list_first_content(&ctx->ops);
		assert(iop != NULL);
		bool bi = false;
		while (iop->kind == ASN_V2_PARSER_OP_KIND_SEQUENCE) {
			iop = ft_list_pop_content(&ctx->ops);
			iast = NULL;
			// SSL_LOG(WARN, "iop:seq:%p:token:%s", iop, __asn1_v2_token_dumpb(*token, cbuf, sizeof(cbuf)));
			bi = __asn1_v2_parser_op_run(ctx, name, token, &iast);
			if (bi) {
				__asn1_v2_ast_add_child(root, iast);
			} else {
				__asn1_v2_ast_delete(iast);
			}
			b = b && bi;
			if (!ctx->done) ctx->done = !b;
			iop = ft_list_first_content(&ctx->ops);
			assert(iop != NULL);
		}

		bi = __asn1_v2_parser_op_run(ctx, name, token, &iast);
		if (bi) {
			__asn1_v2_ast_add_child(root, iast);
		}
		b = b && bi;

		ctx->done = false;
		if (b) {
			*ast = root;
			__asn1_v2_ast_add_parser_info(*ast, name);
			return true;
		}
		__asn1_v2_ast_delete(root);
		*token = old_token;
		return false;
	}
	case ASN_V2_PARSER_OP_KIND_CHOICE: {
		t_asn_v2_ast *last = NULL;
		t_asn_v2_ast *rast = NULL;
		bool b1 = __asn1_v2_parser_op_run(ctx, name, token, &last);
		if (!ctx->done) ctx->done = b1;
		bool b2 = __asn1_v2_parser_op_run(ctx, name, token, &rast);
		ctx->done = false;
		if (b1 || b2) {
			*ast = (b1) ? last : rast;
			__asn1_v2_ast_add_parser_info(*ast, name);
			return true;
		}
		if (b1) __asn1_v2_ast_delete(last);
		if (b2) __asn1_v2_ast_delete(rast);
		return false;
	}
	case ASN_V2_PARSER_OP_KIND_OPTIONAL: {
		bool b = __asn1_v2_parser_op_run(ctx, name, token, ast);
		if (!b) {
			*ast = __asn1_v2_ast_create_null();
			__asn1_v2_ast_add_parser_info(*ast, name);
		}
		return true;
	}
	case ASN_V2_PARSER_OP_KIND_REPEAT: {
		t_asn_v2_ast *root = __asn1_v2_ast_create_list();
		t_asn_v2_token old_token = *token;
		// t_list list = {0};
		bool b = true;
		size_t count = 0;
		while (b) {
			t_asn_v2_ast *iast = NULL;
			t_asn_v2_parser_ctx rctx = {0};
			__asn1_v2_parser_ctx_copy(ctx, &rctx);
			b = __asn1_v2_parser_op_run(&rctx, name, token, &iast);
			__asn1_v2_parser_ctx_clear(&rctx);
			if (b) {
				// ft_list_append(&list, &iast->base);
				__asn1_v2_ast_add_child(root, iast);
				count++;
			}
		}
		if (count > 0) {
			// *ast = container_of(list.first, t_asn_v2_ast, base);
			*ast = root;
			// __asn1_v2_ast_add_parser_info(*ast, name);
			return true;
		}
		// ft_list_clear(&list, __asn1_v2_ast_delete_adapter);
		__asn1_v2_ast_delete(root);
		*token = old_token;
		return false;
	}
	case ASN_V2_PARSER_OP_KIND_UNWRAP: {
		t_asn_v2_ast *wrapped = NULL;
		bool b = __asn1_v2_parser_op_run(ctx, name, token, &wrapped);
		if (b) {
			if (NULL != wrapped->base.nodes) {
				*ast = container_of(wrapped->base.nodes, t_asn_v2_ast, base);
				wrapped->base.nodes = NULL;
				__asn1_v2_ast_delete(wrapped);
			}
			else {
				*ast = wrapped;
			}
		}
		return (b);
	}
	case ASN_V2_PARSER_OP_KIND_WRAP: {
		t_asn_v2_ast *to_wrap = NULL;
		bool b = __asn1_v2_parser_op_run(ctx, name, token, &to_wrap);
		if (b) {
			*ast = __asn1_v2_ast_create_wrap();
			(*ast)->base.nodes = &to_wrap->base;
			__asn1_v2_ast_add_parser_info(*ast, name);
		}
		return (b);
	}
	case ASN_V2_PARSER_OP_KIND_COMPACT: {
		t_list list = {0};
		bool b = __asn1_v2_parser_op_run(ctx, name, token, ast);
		if (b) {
			t_node_v2 *nodes = (*ast)->base.nodes;
			(*ast)->base.nodes = NULL;
			t_node_v2 *n = nodes;
			while (NULL != n) {
				t_node_v2 *next = n->next;
				n->next = NULL;
				t_asn_v2_ast *iast = container_of(n, t_asn_v2_ast, base);
				if (iast->kind == ASN_V2_DATA_KIND_NULL || iast->kind == ASN_V2_DATA_KIND_EMPTY) {
					__asn1_v2_ast_delete(iast);
				} else {
					ft_list_append(&list, n);
				}
				n = next;
			}
			(*ast)->base.nodes = list.first;
		}
		return (b);
	}
	case ASN_V2_PARSER_OP_KIND_NONE: {
		*ast = __asn1_v2_ast_create_null();
		return (true);
	}
	default: {
		UNREACHABLE("__asn1_v2_parser_op_run");
	}}
	return false;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static void __asn1_v2_parser_op_push_call(t_asn_v2_parser_ctx *ctx, t_func_asn_v2_parse f, const char *name)
{
	t_asn_v2_parser_op *op = __asn1_v2_parser_op_create();
	op->kind = ASN_V2_PARSER_OP_KIND_CALL;
	ft_list_append_content(&ctx->ops, op);

	t_asn_v2_parser_func *func = __asn1_v2_parser_func_create();
	func->call = f;
	func->name = name;
	ft_list_append_content(&ctx->funcs, func);
}

static void __asn1_v2_parser_op_push_repeat(t_asn_v2_parser_ctx *ctx)
{
	t_asn_v2_parser_op *op = __asn1_v2_parser_op_create();
	op->kind = ASN_V2_PARSER_OP_KIND_REPEAT;
	ft_list_append_content(&ctx->ops, op);
}

static void __asn1_v2_parser_op_push_unwrap(t_asn_v2_parser_ctx *ctx)
{
	t_asn_v2_parser_op *op = __asn1_v2_parser_op_create();
	op->kind = ASN_V2_PARSER_OP_KIND_UNWRAP;
	ft_list_append_content(&ctx->ops, op);
}

static void __asn1_v2_parser_op_push_wrap(t_asn_v2_parser_ctx *ctx)
{
	t_asn_v2_parser_op *op = __asn1_v2_parser_op_create();
	op->kind = ASN_V2_PARSER_OP_KIND_WRAP;
	ft_list_append_content(&ctx->ops, op);
}

static void __asn1_v2_parser_op_push_compact(t_asn_v2_parser_ctx *ctx)
{
	t_asn_v2_parser_op *op = __asn1_v2_parser_op_create();
	op->kind = ASN_V2_PARSER_OP_KIND_COMPACT;
	ft_list_append_content(&ctx->ops, op);
}

static void __asn1_v2_parser_op_push_optional(t_asn_v2_parser_ctx *ctx)
{
	t_asn_v2_parser_op *op = __asn1_v2_parser_op_create();
	op->kind = ASN_V2_PARSER_OP_KIND_OPTIONAL;
	ft_list_append_content(&ctx->ops, op);
}

static void __asn1_v2_parser_op_push_sequence(t_asn_v2_parser_ctx *ctx)
{
	t_asn_v2_parser_op *op = __asn1_v2_parser_op_create();
	op->kind = ASN_V2_PARSER_OP_KIND_SEQUENCE;
	ft_list_append_content(&ctx->ops, op);
}

static void __asn1_v2_parser_op_push_or(t_asn_v2_parser_ctx *ctx)
{
	t_asn_v2_parser_op *op = __asn1_v2_parser_op_create();
	op->kind = ASN_V2_PARSER_OP_KIND_CHOICE;
	ft_list_append_content(&ctx->ops, op);
}

static void __asn1_v2_parser_op_push_none(t_asn_v2_parser_ctx *ctx)
{
	t_asn_v2_parser_op *op = __asn1_v2_parser_op_create();
	op->kind = ASN_V2_PARSER_OP_KIND_NONE;
	ft_list_append_content(&ctx->ops, op);
}

#define PUSH_OP_CALL(ctx, fn) \
	((void)__asn1_v2_parser_op_push_call((ctx), (fn), #fn))

#define PUSH_OP_OPTIONAL(ctx, op) \
	((void)__asn1_v2_parser_op_push_optional((ctx)), \
		(void)(op))

#define PUSH_OP_REPEAT(ctx, op) \
	((void)__asn1_v2_parser_op_push_repeat((ctx)), \
		(void)(op))

#define PUSH_OP_UNWRAP(ctx, op) \
	((void)__asn1_v2_parser_op_push_unwrap((ctx)), \
		(void)(op))

#define PUSH_OP_WRAP(ctx, op) \
	((void)__asn1_v2_parser_op_push_wrap((ctx)), \
		(void)(op))

#define PUSH_OP_COMPACT(ctx, op) \
	((void)__asn1_v2_parser_op_push_compact((ctx)), \
		(void)(op))

#define PUSH_OP_SEQ_1(ctx, op) \
	PUSH_OP_COMPACT(ctx, \
		((void)__asn1_v2_parser_op_push_sequence((ctx)), \
			(void)(op), \
			(void)__asn1_v2_parser_op_push_none(ctx)))

#define PUSH_OP_SEQ_2(ctx, left, right) \
	((void)__asn1_v2_parser_op_push_sequence((ctx)), \
		(void)(left), \
		(void)(right))

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
	((void)__asn1_v2_parser_op_push_or((ctx)), \
		(void)(left), \
		(void)(right))

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
					PUSH_OP_CALL(ctx, item_op)))))));

#define PUSH_OP_SEQ_OF(ctx, item_op) \
	PUSH_OP_SEQ_2(ctx, \
		PUSH_OP_CALL(ctx, item_op), \
		PUSH_OP_OPTIONAL(ctx, \
			PUSH_OP_UNWRAP(ctx, PUSH_OP_REPEAT(ctx, \
				PUSH_OP_CALL(ctx, item_op)))));

#define RUN_OP(ctx, token, ast) \
	__asn1_v2_parser_op_run((ctx), __func__, (token), (ast))

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static bool __asn1_v2_parse_char(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast, char c)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_SINGLE_CHAR) {
		if (t.s[t.start] == c) {
			if (c == '\n') {
				SSL_LOG(TRACE, "match: SINGLE_CHAR '\\n'");
			} else {
				SSL_LOG(TRACE, "match: SINGLE_CHAR '%c'", c);
			}
			*token = t;
			*ast = __asn1_v2_ast_create_leaf(t);
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

static bool __asn1_v2_parse_token_L_CURLY_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)  { return (__asn1_v2_parse_char(ctx, token, ast, '{')); }
static bool __asn1_v2_parse_token_R_CURLY_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)  { return (__asn1_v2_parse_char(ctx, token, ast, '}')); }
static bool __asn1_v2_parse_token_L_ANGLE_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)  { return (__asn1_v2_parse_char(ctx, token, ast, '<')); }
static bool __asn1_v2_parse_token_R_ANGLE_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)  { return (__asn1_v2_parse_char(ctx, token, ast, '>')); }
static bool __asn1_v2_parse_token_COMMA(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_char(ctx, token, ast, ',')); }
static bool __asn1_v2_parse_token_DOT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)           { return (__asn1_v2_parse_char(ctx, token, ast, '.')); }
static bool __asn1_v2_parse_token_SLASH(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_char(ctx, token, ast, '/')); }
static bool __asn1_v2_parse_token_L_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)        { return (__asn1_v2_parse_char(ctx, token, ast, '(')); }
static bool __asn1_v2_parse_token_R_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)        { return (__asn1_v2_parse_char(ctx, token, ast, ')')); }
static bool __asn1_v2_parse_token_L_SQUARE_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast) { return (__asn1_v2_parse_char(ctx, token, ast, '[')); }
static bool __asn1_v2_parse_token_R_SQUARE_BRKT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast) { return (__asn1_v2_parse_char(ctx, token, ast, ']')); }
static bool __asn1_v2_parse_token_HYPHEN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)        { return (__asn1_v2_parse_char(ctx, token, ast, '-')); }
static bool __asn1_v2_parse_token_COLON(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_char(ctx, token, ast, ':')); }
static bool __asn1_v2_parse_token_EQUAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_char(ctx, token, ast, '=')); }
static bool __asn1_v2_parse_token_DOUBLE_QUOTE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)  { return (__asn1_v2_parse_char(ctx, token, ast, '"')); }
static bool __asn1_v2_parse_token_QUOTE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_char(ctx, token, ast, '\'')); }
static bool __asn1_v2_parse_token_SEMICOLON(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)     { return (__asn1_v2_parse_char(ctx, token, ast, ';')); }
static bool __asn1_v2_parse_token_AT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)            { return (__asn1_v2_parse_char(ctx, token, ast, '@')); }
static bool __asn1_v2_parse_token_PIPE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_char(ctx, token, ast, '|')); }
static bool __asn1_v2_parse_token_BANG(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_char(ctx, token, ast, '!')); }
static bool __asn1_v2_parse_token_CARET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_char(ctx, token, ast, '^')); }
static bool __asn1_v2_parse_token_AMPERSAND(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)     { return (__asn1_v2_parse_char(ctx, token, ast, '&')); }
static bool __asn1_v2_parse_token_ASTERISK(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)      { return (__asn1_v2_parse_char(ctx, token, ast, '*')); }

static bool __asn1_v2_parse_keyword(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast, const char *kw)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_KEYWORD) {
		const char *s = t.s + t.start;
		size_t slen = t.end - t.start;
		size_t kwlen = ft_strlen(kw);
		if (kwlen == slen && ft_strneq(s, kw, kwlen)) {
			SSL_LOG(TRACE, "match: KEYWORD '%s'", kw);
			*token = t;
			*ast = __asn1_v2_ast_create_leaf(t);
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: KEYWORD '%s': %s", kw, __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_keyword_ABSENT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)           { return (__asn1_v2_parse_keyword(ctx, token, ast, "ABSENT")); }
static bool __asn1_v2_parse_keyword_ENCODED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_keyword(ctx, token, ast, "ENCODED")); }
static bool __asn1_v2_parse_keyword_INTERSECTION(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)     { return (__asn1_v2_parse_keyword(ctx, token, ast, "INTERSECTION")); }
static bool __asn1_v2_parse_keyword_SEQUENCE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_keyword(ctx, token, ast, "SEQUENCE")); }
static bool __asn1_v2_parse_keyword_ABSTRACT_SYNTAX(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)  { return (__asn1_v2_parse_keyword(ctx, token, ast, "ABSTRACT-SYNTAX")); }
static bool __asn1_v2_parse_keyword_ENCODING_CONTROL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast) { return (__asn1_v2_parse_keyword(ctx, token, ast, "ENCODING-CONTROL")); }
static bool __asn1_v2_parse_keyword_ISO646String(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)     { return (__asn1_v2_parse_keyword(ctx, token, ast, "ISO646String")); }
static bool __asn1_v2_parse_keyword_SET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)              { return (__asn1_v2_parse_keyword(ctx, token, ast, "SET")); }
static bool __asn1_v2_parse_keyword_ALL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)              { return (__asn1_v2_parse_keyword(ctx, token, ast, "ALL")); }
static bool __asn1_v2_parse_keyword_END(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)              { return (__asn1_v2_parse_keyword(ctx, token, ast, "END")); }
static bool __asn1_v2_parse_keyword_MAX(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)              { return (__asn1_v2_parse_keyword(ctx, token, ast, "MAX")); }
static bool __asn1_v2_parse_keyword_SETTINGS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_keyword(ctx, token, ast, "SETTINGS")); }
static bool __asn1_v2_parse_keyword_APPLICATION(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)      { return (__asn1_v2_parse_keyword(ctx, token, ast, "APPLICATION")); }
static bool __asn1_v2_parse_keyword_ENUMERATED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)       { return (__asn1_v2_parse_keyword(ctx, token, ast, "ENUMERATED")); }
static bool __asn1_v2_parse_keyword_MIN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)              { return (__asn1_v2_parse_keyword(ctx, token, ast, "MIN")); }
static bool __asn1_v2_parse_keyword_SIZE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)             { return (__asn1_v2_parse_keyword(ctx, token, ast, "SIZE")); }
static bool __asn1_v2_parse_keyword_AUTOMATIC(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)        { return (__asn1_v2_parse_keyword(ctx, token, ast, "AUTOMATIC")); }
static bool __asn1_v2_parse_keyword_EXCEPT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)           { return (__asn1_v2_parse_keyword(ctx, token, ast, "EXCEPT")); }
static bool __asn1_v2_parse_keyword_MINUS_INFINITY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)   { return (__asn1_v2_parse_keyword(ctx, token, ast, "MINUS-INFINITY")); }
static bool __asn1_v2_parse_keyword_STRING(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)           { return (__asn1_v2_parse_keyword(ctx, token, ast, "STRING")); }
static bool __asn1_v2_parse_keyword_BEGIN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)            { return (__asn1_v2_parse_keyword(ctx, token, ast, "BEGIN")); }
static bool __asn1_v2_parse_keyword_EXPLICIT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_keyword(ctx, token, ast, "EXPLICIT")); }
static bool __asn1_v2_parse_keyword_NOT_A_NUMBER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)     { return (__asn1_v2_parse_keyword(ctx, token, ast, "NOT-A-NUMBER")); }
static bool __asn1_v2_parse_keyword_SYNTAX(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)           { return (__asn1_v2_parse_keyword(ctx, token, ast, "SYNTAX")); }
static bool __asn1_v2_parse_keyword_BIT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)              { return (__asn1_v2_parse_keyword(ctx, token, ast, "BIT")); }
static bool __asn1_v2_parse_keyword_EXPORTS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_keyword(ctx, token, ast, "EXPORTS")); }
static bool __asn1_v2_parse_keyword_NULL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)             { return (__asn1_v2_parse_keyword(ctx, token, ast, "NULL")); }
static bool __asn1_v2_parse_keyword_T61String(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)        { return (__asn1_v2_parse_keyword(ctx, token, ast, "T61String")); }
static bool __asn1_v2_parse_keyword_BMPString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)        { return (__asn1_v2_parse_keyword(ctx, token, ast, "BMPString")); }
static bool __asn1_v2_parse_keyword_EXTENSIBILITY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)    { return (__asn1_v2_parse_keyword(ctx, token, ast, "EXTENSIBILITY")); }
static bool __asn1_v2_parse_keyword_NumericString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)    { return (__asn1_v2_parse_keyword(ctx, token, ast, "NumericString")); }
static bool __asn1_v2_parse_keyword_TAGS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)             { return (__asn1_v2_parse_keyword(ctx, token, ast, "TAGS")); }
static bool __asn1_v2_parse_keyword_BOOLEAN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_keyword(ctx, token, ast, "BOOLEAN")); }
static bool __asn1_v2_parse_keyword_EXTERNAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_keyword(ctx, token, ast, "EXTERNAL")); }
static bool __asn1_v2_parse_keyword_OBJECT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)           { return (__asn1_v2_parse_keyword(ctx, token, ast, "OBJECT")); }
static bool __asn1_v2_parse_keyword_TeletexString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)    { return (__asn1_v2_parse_keyword(ctx, token, ast, "TeletexString")); }
static bool __asn1_v2_parse_keyword_BY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)               { return (__asn1_v2_parse_keyword(ctx, token, ast, "BY")); }
static bool __asn1_v2_parse_keyword_FALSE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)            { return (__asn1_v2_parse_keyword(ctx, token, ast, "FALSE")); }
static bool __asn1_v2_parse_keyword_ObjectDescriptor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast) { return (__asn1_v2_parse_keyword(ctx, token, ast, "ObjectDescriptor")); }
static bool __asn1_v2_parse_keyword_TIME(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)             { return (__asn1_v2_parse_keyword(ctx, token, ast, "TIME")); }
static bool __asn1_v2_parse_keyword_CHARACTER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)        { return (__asn1_v2_parse_keyword(ctx, token, ast, "CHARACTER")); }
static bool __asn1_v2_parse_keyword_FROM(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)             { return (__asn1_v2_parse_keyword(ctx, token, ast, "FROM")); }
static bool __asn1_v2_parse_keyword_OCTET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)            { return (__asn1_v2_parse_keyword(ctx, token, ast, "OCTET")); }
static bool __asn1_v2_parse_keyword_TIME_OF_DAY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)      { return (__asn1_v2_parse_keyword(ctx, token, ast, "TIME-OF-DAY")); }
static bool __asn1_v2_parse_keyword_CHOICE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)           { return (__asn1_v2_parse_keyword(ctx, token, ast, "CHOICE")); }
static bool __asn1_v2_parse_keyword_GeneralizedTime(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)  { return (__asn1_v2_parse_keyword(ctx, token, ast, "GeneralizedTime")); }
static bool __asn1_v2_parse_keyword_OF(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)               { return (__asn1_v2_parse_keyword(ctx, token, ast, "OF")); }
static bool __asn1_v2_parse_keyword_TRUE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)             { return (__asn1_v2_parse_keyword(ctx, token, ast, "TRUE")); }
static bool __asn1_v2_parse_keyword_CLASS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)            { return (__asn1_v2_parse_keyword(ctx, token, ast, "CLASS")); }
static bool __asn1_v2_parse_keyword_GeneralString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)    { return (__asn1_v2_parse_keyword(ctx, token, ast, "GeneralString")); }
static bool __asn1_v2_parse_keyword_OID_IRI(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_keyword(ctx, token, ast, "OID-IRI")); }
static bool __asn1_v2_parse_keyword_TYPE_IDENTIFIER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)  { return (__asn1_v2_parse_keyword(ctx, token, ast, "TYPE-IDENTIFIER")); }
static bool __asn1_v2_parse_keyword_COMPONENT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)        { return (__asn1_v2_parse_keyword(ctx, token, ast, "COMPONENT")); }
static bool __asn1_v2_parse_keyword_GraphicString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)    { return (__asn1_v2_parse_keyword(ctx, token, ast, "GraphicString")); }
static bool __asn1_v2_parse_keyword_OPTIONAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_keyword(ctx, token, ast, "OPTIONAL")); }
static bool __asn1_v2_parse_keyword_UNION(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)            { return (__asn1_v2_parse_keyword(ctx, token, ast, "UNION")); }
static bool __asn1_v2_parse_keyword_COMPONENTS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)       { return (__asn1_v2_parse_keyword(ctx, token, ast, "COMPONENTS")); }
static bool __asn1_v2_parse_keyword_IA5String(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)        { return (__asn1_v2_parse_keyword(ctx, token, ast, "IA5String")); }
static bool __asn1_v2_parse_keyword_PATTERN(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_keyword(ctx, token, ast, "PATTERN")); }
static bool __asn1_v2_parse_keyword_UNIQUE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)           { return (__asn1_v2_parse_keyword(ctx, token, ast, "UNIQUE")); }
static bool __asn1_v2_parse_keyword_CONSTRAINED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)      { return (__asn1_v2_parse_keyword(ctx, token, ast, "CONSTRAINED")); }
static bool __asn1_v2_parse_keyword_IDENTIFIER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)       { return (__asn1_v2_parse_keyword(ctx, token, ast, "IDENTIFIER")); }
static bool __asn1_v2_parse_keyword_PDV(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)              { return (__asn1_v2_parse_keyword(ctx, token, ast, "PDV")); }
static bool __asn1_v2_parse_keyword_UNIVERSAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)        { return (__asn1_v2_parse_keyword(ctx, token, ast, "UNIVERSAL")); }
static bool __asn1_v2_parse_keyword_CONTAINING(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)       { return (__asn1_v2_parse_keyword(ctx, token, ast, "CONTAINING")); }
static bool __asn1_v2_parse_keyword_IMPLICIT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_keyword(ctx, token, ast, "IMPLICIT")); }
static bool __asn1_v2_parse_keyword_PLUS_INFINITY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)    { return (__asn1_v2_parse_keyword(ctx, token, ast, "PLUS-INFINITY")); }
static bool __asn1_v2_parse_keyword_UniversalString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)  { return (__asn1_v2_parse_keyword(ctx, token, ast, "UniversalString")); }
static bool __asn1_v2_parse_keyword_DATE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)             { return (__asn1_v2_parse_keyword(ctx, token, ast, "DATE")); }
static bool __asn1_v2_parse_keyword_IMPLIED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_keyword(ctx, token, ast, "IMPLIED")); }
static bool __asn1_v2_parse_keyword_PRESENT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_keyword(ctx, token, ast, "PRESENT")); }
static bool __asn1_v2_parse_keyword_UTCTime(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_keyword(ctx, token, ast, "UTCTime")); }
static bool __asn1_v2_parse_keyword_DATE_TIME(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)        { return (__asn1_v2_parse_keyword(ctx, token, ast, "DATE-TIME")); }
static bool __asn1_v2_parse_keyword_IMPORTS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_keyword(ctx, token, ast, "IMPORTS")); }
static bool __asn1_v2_parse_keyword_PrintableString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)  { return (__asn1_v2_parse_keyword(ctx, token, ast, "PrintableString")); }
static bool __asn1_v2_parse_keyword_UTF8String(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)       { return (__asn1_v2_parse_keyword(ctx, token, ast, "UTF8String")); }
static bool __asn1_v2_parse_keyword_DEFAULT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_keyword(ctx, token, ast, "DEFAULT")); }
static bool __asn1_v2_parse_keyword_INCLUDES(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_keyword(ctx, token, ast, "INCLUDES")); }
static bool __asn1_v2_parse_keyword_PRIVATE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_keyword(ctx, token, ast, "PRIVATE")); }
static bool __asn1_v2_parse_keyword_VideotexString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)   { return (__asn1_v2_parse_keyword(ctx, token, ast, "VideotexString")); }
static bool __asn1_v2_parse_keyword_DEFINITIONS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)      { return (__asn1_v2_parse_keyword(ctx, token, ast, "DEFINITIONS")); }
static bool __asn1_v2_parse_keyword_INSTANCE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_keyword(ctx, token, ast, "INSTANCE")); }
static bool __asn1_v2_parse_keyword_REAL(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)             { return (__asn1_v2_parse_keyword(ctx, token, ast, "REAL")); }
static bool __asn1_v2_parse_keyword_VisibleString(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)    { return (__asn1_v2_parse_keyword(ctx, token, ast, "VisibleString")); }
static bool __asn1_v2_parse_keyword_DURATION(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_keyword(ctx, token, ast, "DURATION")); }
static bool __asn1_v2_parse_keyword_INSTRUCTIONS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)     { return (__asn1_v2_parse_keyword(ctx, token, ast, "INSTRUCTIONS")); }
static bool __asn1_v2_parse_keyword_RELATIVE_OID(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)     { return (__asn1_v2_parse_keyword(ctx, token, ast, "RELATIVE-OID")); }
static bool __asn1_v2_parse_keyword_WITH(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)             { return (__asn1_v2_parse_keyword(ctx, token, ast, "WITH")); }
static bool __asn1_v2_parse_keyword_EMBEDDED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)         { return (__asn1_v2_parse_keyword(ctx, token, ast, "EMBEDDED")); }
static bool __asn1_v2_parse_keyword_INTEGER(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_keyword(ctx, token, ast, "INTEGER")); }
static bool __asn1_v2_parse_keyword_RELATIVE_OID_IRI(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast) { return (__asn1_v2_parse_keyword(ctx, token, ast, "RELATIVE-OID-IRI")); }
static bool __asn1_v2_parse_keyword_ANY(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)              { return (__asn1_v2_parse_keyword(ctx, token, ast, "ANY")); }
static bool __asn1_v2_parse_keyword_DEFINED(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)          { return (__asn1_v2_parse_keyword(ctx, token, ast, "DEFINED")); }

static bool __asn1_v2_parse_token_ASSIGNMENT(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_ASSIGNMENT) {
		SSL_LOG(TRACE, "match: ASSIGNMENT");
		*token = t;
		*ast = __asn1_v2_ast_create_leaf(t);
		return true;
	}
	SSL_LOG(TRACE, "no match: ASSIGNMENT: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_token_RANGE(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_RANGE) {
		SSL_LOG(TRACE, "match: RANGE");
		*token = t;
		*ast = __asn1_v2_ast_create_leaf(t);
		return true;
	}
	SSL_LOG(TRACE, "no match: RANGE: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_token_ELLIPSIS(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_ELLIPSIS) {
		SSL_LOG(TRACE, "match: ELLIPSIS");
		*token = t;
		*ast = __asn1_v2_ast_create_leaf(t);
		return true;
	}
	SSL_LOG(TRACE, "no match: ELLIPSIS: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_token_L_VER_BRACKET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_LVER_BRACKET) {
		SSL_LOG(TRACE, "match: LVER_BRACKET");
		*token = t;
		*ast = __asn1_v2_ast_create_leaf(t);
		return true;
	}
	SSL_LOG(TRACE, "no match: LVER_BRACKET: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_token_R_VER_BRACKET(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_RVER_BRACKET) {
		SSL_LOG(TRACE, "match: RVER_BRACKET");
		*token = t;
		*ast = __asn1_v2_ast_create_leaf(t);
		return true;
	}
	SSL_LOG(TRACE, "no match: RVER_BRACKET: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_bstring(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_BSTRING) {
		SSL_LOG(TRACE, "match: BSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*token = t;
		*ast = __asn1_v2_ast_create_leaf(t);
		return true;
	}
	SSL_LOG(TRACE, "no match: BSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_hstring(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_HSTRING) {
		SSL_LOG(TRACE, "match: HSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*token = t;
		*ast = __asn1_v2_ast_create_leaf(t);
		return true;
	}
	SSL_LOG(TRACE, "no match: HSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_cstring(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_CSTRING) {
		SSL_LOG(TRACE, "match: CSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*token = t;
		*ast = __asn1_v2_ast_create_leaf(t);
		return true;
	}
	SSL_LOG(TRACE, "no match: CSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_tstring(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

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
		*token = t;
		*ast = __asn1_v2_ast_create_leaf(t);
		return true;
	}

label_error:
	SSL_LOG(TRACE, "no match: TSTRING: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_number(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NUMBER) {
		SSL_LOG(TRACE, "match: NUMBER: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*token = t;
		*ast = __asn1_v2_ast_create_leaf(t);
		return true;
	}
	SSL_LOG(TRACE, "no match: NUMBER: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_realnumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_REAL_NUMBER) {
		SSL_LOG(TRACE, "match: REAL_NUMBER: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*token = t;
		*ast = __asn1_v2_ast_create_leaf(t);
		return true;
	}
	SSL_LOG(TRACE, "no match: REAL_NUMBER: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_upper_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NAME) {
		size_t i = t.start;
		while (i < t.end && ft_isalpha_upper(t.s[i])) i++;
		if (i == t.end) {
			SSL_LOG(TRACE, "match: UPPER_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
			*token = t;
			*ast = __asn1_v2_ast_create_leaf(t);
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: UPPER_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_lower_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NAME) {
		size_t i = t.start;
		while (i < t.end && ft_isalpha_lower(t.s[i])) i++;
		if (i == t.end) {
			SSL_LOG(TRACE, "match: LOWER_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
			*token = t;
			*ast = __asn1_v2_ast_create_leaf(t);
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: LOWER_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_upper_first_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NAME) {
		if (ft_isalpha_upper(t.s[t.start])) {
			SSL_LOG(TRACE, "match: UPPER_FIRST_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
			*token = t;
			*ast = __asn1_v2_ast_create_leaf(t);
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: UPPER_FIRST_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_lower_first_name(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NAME) {
		if (ft_isalpha_lower(t.s[t.start])) {
			SSL_LOG(TRACE, "match: LOWER_FIRST_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
			*token = t;
			*ast = __asn1_v2_ast_create_leaf(t);
			return true;
		}
	}
	SSL_LOG(TRACE, "no match: LOWER_FIRST_NAME: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_integer_unicode_label(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NUMBER) {
		t.kind = ASN_V2_TOKEN_KIND_INT_UNI_LBL;
		SSL_LOG(TRACE, "match: integerUnicodeLabel: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*token = t;
		*ast = __asn1_v2_ast_create_leaf(t);
		return true;
	}
	SSL_LOG(TRACE, "no match: integerUnicodeLabel: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_non_integer_unicode_label(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	char cbuf[1024];
	t_asn_v2_token t = *token;
	UNUSED(ctx);

	if (__asn1_v2_token_next(&t) && t.kind == ASN_V2_TOKEN_KIND_NON_INT_UNI_LBL) {
		SSL_LOG(TRACE, "match: non-integerUnicodeLabel: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
		*token = t;
		*ast = __asn1_v2_ast_create_leaf(t);
		return true;
	}
	SSL_LOG(TRACE, "no match: non-integerUnicodeLabel: %s", __asn1_v2_token_dumpb(t, cbuf, sizeof(cbuf)));
	return false;
}

static bool __asn1_v2_parse_empty(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	t_asn_v2_token t = *token;
	t.end = t.start;
	t.kind = ASN_V2_TOKEN_KIND_EMPTY;
	*ast = __asn1_v2_ast_create_leaf(t);
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

static bool __asn1_v2_parse_empty(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_identifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_valuereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_typereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_objectclassreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_objectreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_objectsetreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_typefieldreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_modulereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_xmlasn1typename(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);

static bool __asn1_v2_parse_EncodingReferenceDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TagDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExtensionDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NameForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinitiveNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinitiveNameAndNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinitiveObjIdComponent(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinitiveObjIdComponentList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinitiveOID(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_IRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinitiveOIDAndIRI(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinitiveIdentification(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ModuleIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Reference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Symbol(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SymbolList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SymbolsExported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Exports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_AssignedIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_GlobalModuleReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SelectionOption(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SymbolsFromModule(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SymbolsFromModuleList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SymbolsImported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Imports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExternalTypeReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExternalValueReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ItemId(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ItemSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_AbsoluteReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_xmlasn1typename(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NonParametrizedTypeName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedValueSetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NamedBit(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NamedBitList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_BitStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_BooleanType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_RestrictedCharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_UnrestrictedCharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_CharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_AlternativeTypeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExtensionAdditionAlternativeGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExtensionAdditionAlternative(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExtensionAdditionAlternativeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExtensionAdditionAlternatives(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_AlternativeTypeLists(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ChoiceType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DateType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DateTimeType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DurationType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_EmbeddedPDVType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_EnumerationItem(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Enumeration(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExceptionIdentification(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExceptionSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Enumerations(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_EnumeratedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExternalType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_InstanceOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SignedNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NamedNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NamedNumberList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_IntegerType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_IRIType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NullType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectClassFieldType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectIdentifierType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_OctetStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_RealType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_RelativeIRIType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_RelativeOIDType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExtensionAndException(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_OptionalExtensionMarker(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ComponentType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ComponentTypeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_VersionNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExtensionAdditionGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExtensionAddition(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExtensionAdditionList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExtensionAdditions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ComponentTypeLists(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SequenceType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SequenceOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SetOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ClassNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Class(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_EncodingReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Tag(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TaggedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_EncodingInstruction(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_EncodingPrefix(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_EncodingPrefixedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_PrefixedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TimeType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TimeOfDayType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_BuiltinType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_UsefulType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SelectionType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TypeFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ValueSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ReferencedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SubtypeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ConstraintSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Constraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ConstrainedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ConstrainedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NamedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_IdentifierList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_BitStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLBitStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_BooleanValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLBooleanValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Tuple(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Quadruple(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_CharsDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_CharSyms(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_CharacterStringList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_RestrictedCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_UnrestrictedCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_CharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ChoiceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLChoiceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_EmbeddedPDVValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_EnumeratedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLEnumeratedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExternalValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLExternalValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_InstanceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_IntegerValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLIntegerValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_FirstArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SubsequentArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_IRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NullValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLNullValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_OctetStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLOctetStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NameAndNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjIdComponents(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjIdComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NumericRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SpecialRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_RealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_FirstRelativeArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_RelativeIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLRelativeIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_RelativeOIDComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_RelativeOIDComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_RelativeOIDValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLRelativeOIDValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ComponentValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SequenceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLSequenceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NamedValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SequenceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLSequenceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SetValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLSetValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SetOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLSetOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_PrefixedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLPrefixedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TimeValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLTimeValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_BuiltinValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ValueFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ReferencedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectClassFieldValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NamedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLNamedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLTypedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_XMLValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SingleValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Includes(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ContainedSubtype(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_UpperEndValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_LowerEndValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_UpperEndpoint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_LowerEndpoint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ValueRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_PermittedAlphabet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SizeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TypeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_PresenceConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ValueConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ComponentConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_NamedConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_MultipleTypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_InnerTypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_PatternConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_PropertyAndSettingPair(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_PropertySettingsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_PropertySettings(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DurationRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TimePointRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_RecurrenceRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Elements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Elements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Exclusions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Intersections(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_IntersectionMark(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Intersections(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_UnionMark(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Unions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ElementSetSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ElementSetSpecs(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ValueSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ValueSetTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectClassAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectSetAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_AssignmentList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ModuleBody(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Module(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);

static bool __asn1_v2_parse_Literal(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_RequiredToken(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_OptionalGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TokenOrGroupSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TokenOrGroupSpecs(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SyntaxList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExternalObjectClassReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExternalObjectReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ExternalObjectSetReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_UsefulObjectClassReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinedObjectClass(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinedObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinedObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_PrimitiveFieldName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_FieldName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectSetOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectSetFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ValueSetOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_VariableTypeValueSetFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_FixedTypeValueSetFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ValueOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_VariableTypeValueFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_FixedTypeValueFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TypeOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TypeFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_FieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_WithSyntaxSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_FieldSpecList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectClassDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectClass(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectClassAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Setting(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinedSyntaxToken(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinedSyntaxTokenList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefinedSyntax(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_FieldSettingList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DefaultSyntax(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_FieldSetting(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Object(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectSetSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectSetAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectSetElements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ReferencedObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ValueFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ValueSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_TypeFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_InformationFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ObjectSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);

static bool __asn1_v2_parse_ActualParameter(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ActualParameters(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ActualParameterList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedObjectClass(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedValueSetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SimpleDefinedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_SimpleDefinedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DummyReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_DummyGovernor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Governor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParamGovernor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Parameter(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_Parameters(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedObjectSetAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedObjectAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedObjectClassAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedValueSetTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);
static bool __asn1_v2_parse_ParameterizedTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast);

static bool __asn1_v2_parse_identifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_lower_first_name);

	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_valuereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier);

	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_typereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name);

	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_encodingreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name);

	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_objectclassreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_name);

	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_objectreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference);

	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_objectsetreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference);

	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_objectfieldreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AMPERSAND),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectreference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_typefieldreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AMPERSAND),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_valuefieldreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AMPERSAND),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_valuesetfieldreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AMPERSAND),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_objectsetfieldreference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AMPERSAND),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetreference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_modulereference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference);

	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EncodingReferenceDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// encodingreference INSTRUCTIONS | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INSTRUCTIONS)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TagDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// EXPLICIT TAGS | IMPLICIT TAGS | AUTOMATIC TAGS | empty
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXPLICIT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TAGS)
		),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IMPLICIT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TAGS)
		),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_AUTOMATIC),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TAGS)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionDefault(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// EXTENSIBILITY IMPLIED | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXTENSIBILITY),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IMPLIED)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NameForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier
	PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// number
	PUSH_OP_CALL(ctx, __asn1_v2_parse_number);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveNameAndNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier "(" DefinitiveNumberForm ")"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveObjIdComponent(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// NameForm | DefinitiveNumberForm | DefinitiveNameAndNumberForm
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NameForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveNumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveNameAndNumberForm)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveObjIdComponentList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefinitiveObjIdComponent | DefinitiveObjIdComponent DefinitiveObjIdComponentList
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_DefinitiveObjIdComponent);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveOID(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" DefinitiveObjIdComponentList "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveObjIdComponentList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveOIDAndIRI(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefinitiveOID IRIValue
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveOID),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_IRIValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinitiveIdentification(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefinitiveOIDandIRI | DefinitiveOID | empty
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveOIDAndIRI),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveOID),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ModuleIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// modulereference DefinitiveIdentification
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinitiveIdentification)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Reference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// typereference | valuereference | objectclassreference | objectreference | objectsetreference
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectclassreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetreference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Symbol(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Reference | ParameterizedReference
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Reference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedReference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Symbol | Symbol "," SymbolList
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_Symbol, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolsExported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SymbolList | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Exports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// EXPORTS SymbolsExported ";" | EXPORTS ALL ";" | empty
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXPORTS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolsExported),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SEMICOLON)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXPORTS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ALL),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SEMICOLON)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_AssignedIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ObjectIdentifierValue | DefinedValue | empty
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectIdentifierValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_GlobalModuleReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// modulereference AssignedIdentifier
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_AssignedIdentifier)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SelectionOption(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// empty | WITH "SUCCESSORS" | WITH "DESCENDANTS"
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_WITH),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_name),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolsFromModule(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SymbolList FROM GlobalModuleReference SelectionOption
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_FROM),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_GlobalModuleReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SelectionOption)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolsFromModuleList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SymbolsFromModule | SymbolsFromModuleList SymbolsFromModule
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_SymbolsFromModule);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SymbolsImported(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SymbolsFromModuleList | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolsFromModuleList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Imports(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// IMPORTS SymbolsImported ";" | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IMPORTS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SymbolsImported),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SEMICOLON)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalTypeReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// modulereference "." typereference
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalValueReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// modulereference "." valuereference
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ItemId(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ItemSpec
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ItemSpec);

	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ComponentId(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier | number | "*"
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASTERISK)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ItemSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_AbsoluteReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "@" ModuleIdentifier "." ItemSpec
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_AT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ModuleIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ItemSpec)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_xmlasn1typename(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_NonParametrizedTypeName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ExternalTypeReference | typereference | xmlasn1typename
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalTypeReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_xmlasn1typename)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ExternalTypeReference | typereference | ParameterizedType | ParameterizedValueSetType
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalTypeReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedValueSetType)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ExternalValueReference | valuereference | ParameterizedValue
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalValueReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedBit(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier "(" number ")" | identifier "(" DefinedValue ")"
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
		),
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedBitList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// NamedBit | NamedBitList "," NamedBit
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_NamedBit, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BitStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// BIT STRING | BIT STRING "{" NamedBitList "}"
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BIT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_STRING),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedBitList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BIT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_STRING)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BooleanType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BOOLEAN);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RestrictedCharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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

	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UnrestrictedCharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CHARACTER),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_STRING)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharacterStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// RestrictedCharacterStringType | UnrestrictedCharacterStringType
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_RestrictedCharacterStringType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UnrestrictedCharacterStringType)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_AlternativeTypeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// NamedType | AlternativeTypeList "," NamedType
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_NamedType, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionAlternativeGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "[[" VersionNumber AlternativeTypeList "]]"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_VER_BRACKET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_VersionNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_AlternativeTypeList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_VER_BRACKET)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionAlternative(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ExtensionAdditionAlternativesGroup | NamedType
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionAlternativeGroup),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionAlternativeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ExtensionAdditionAlternative | ExtensionAdditionAlternativeList "," ExtensionAdditionAlternative
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_ExtensionAdditionAlternative, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionAlternatives(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "," ExtensionAdditionAlternativeList | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionAlternativeList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_AlternativeTypeLists(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// RootAlternativeTypeList
	// | RootAlternativeTypeList "," ExtensionAndException ExtensionAdditionAlternatives OptionalExtensionMarker
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_AlternativeTypeList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionAlternatives),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker)
		),
		PUSH_OP_SEQ_1(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_AlternativeTypeList)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ChoiceType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// CHOICE "{" AlternativeTypeLists "}"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CHOICE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_AlternativeTypeLists),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DateType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DATE);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DateTimeType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DATE_TIME);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DurationType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DURATION);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EmbeddedPDVType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EMBEDDED),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PDV)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EnumerationItem(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier | NamedNumber
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedNumber)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Enumeration(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// EnumerationItem | EnumerationItem "," Enumeration
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_EnumerationItem, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExceptionIdentification(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_ExceptionSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "!" ExceptionIdentification | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_BANG),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionIdentification)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Enumerations(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// RootEnumeration
	// | RootEnumeration "," "..." ExceptionSpec
	// | RootEnumeration "," "..." ExceptionSpec "," AdditionalEnumeration
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_SEQ_6(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumeration),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionSpec),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumeration)
		),
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumeration),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionSpec)
		),
		PUSH_OP_SEQ_1(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumeration)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EnumeratedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ENUMERATED "{" Enumerations "}"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ENUMERATED),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Enumerations),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXTERNAL);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_InstanceOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// INSTANCE OF DefinedObjectClass
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INSTANCE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SignedNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// number | "-" number
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_HYPHEN),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_number)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier "(" SignedNumber ")" | identifier "(" DefinedValue ")"
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SignedNumber),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
		),
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedNumberList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// NamedNumber | NamedNumberList "," NamedNumber
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_NamedNumber, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IntegerType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// INTEGER | INTEGER "{" NamedNumberList "}"
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INTEGER),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedNumberList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INTEGER)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IRIType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OID_IRI);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NullType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_NULL);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectClassFieldType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefinedObjectClass "." FieldName
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectIdentifierType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OBJECT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IDENTIFIER)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_OctetStringType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OCTET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_STRING)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RealType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_REAL);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeIRIType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_RELATIVE_OID_IRI);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeOIDType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_RELATIVE_OID);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAndException(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "..." | "..." ExceptionSpec
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionSpec)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS)
	);
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_OptionalExtensionMarker(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "," "..." | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ComponentType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// NamedType | NamedType OPTIONAL | NamedType DEFAULT Value | COMPONENTS OF Type
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_COMPONENTS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFAULT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ComponentTypeList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ComponentType | ComponentTypeList "," ComponentType
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_ComponentType, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_VersionNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// empty | number ":"
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditionGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "[[" VersionNumber ComponentTypeList "]]"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_VER_BRACKET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_VersionNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_VER_BRACKET)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_ExtensionAddition(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ComponentType | ExtensionAdditionGroup
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionGroup)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_ExtensionAdditionList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ExtensionAddition | ExtensionAdditionList "," ExtensionAddition
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_ExtensionAddition, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionAdditions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "," ExtensionAdditionList | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditionList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExtensionEndMarker(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "," "..."
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ComponentTypeLists(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// RootComponentTypeList
	// | RootComponentTypeList "," ExtensionAndException ExtensionAdditions OptionalExtensionMarker
	// | RootComponentTypeList "," ExtensionAndException ExtensionAdditions ExtensionEndMarker "," RootComponentTypeList
	// | ExtensionAndException ExtensionAdditions ExtensionEndMarker "," RootComponentTypeList
	// | ExtensionAndException ExtensionAdditions OptionalExtensionMarker
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_SEQ_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditions),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker)
		),
		PUSH_OP_SEQ_7(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditions),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionEndMarker),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList)
		),
		PUSH_OP_SEQ_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditions),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionEndMarker),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAdditions),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker)
		),
		PUSH_OP_SEQ_1(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeList)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SequenceType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SEQUENCE "{" "}"
	// | SEQUENCE "{" ExtensionAndException OptionalExtensionMarker "}"
	// | SEQUENCE "{" ComponentTypeLists "}"
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeLists),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SequenceOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SEQUENCE OF Type | SEQUENCE OF NamedType
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SET "{" "}"
	// | SET "{" ExtensionAndException OptionalExtensionMarker "}"
	// | SET "{" ComponentTypeLists "}"
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ExtensionAndException),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalExtensionMarker),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentTypeLists),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SetOfType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SET OF Type | SET OF NamedType
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ClassNumber(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// number | DefinedValue
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Class(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// UNIVERSAL | APPLICATION | PRIVATE | empty
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UNIVERSAL),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_APPLICATION),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PRIVATE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EncodingReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// encodingreference ":" | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_encodingreference),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Tag(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "[" EncodingReference Class ClassNumber "]"
	PUSH_OP_SEQ_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_SQUARE_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Class),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ClassNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_SQUARE_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TaggedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Tag Type | Tag IMPLICIT Type | Tag EXPLICIT Type
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Tag),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Tag),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_IMPLICIT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Tag),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXPLICIT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EncodingInstruction(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_EncodingPrefix(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "[" EncodingReference EncodingInstruction "]"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_SQUARE_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingInstruction),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_SQUARE_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EncodingPrefixedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// EncodingPrefix Type
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingPrefix),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PrefixedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// TaggedType | EncodingPrefixedType
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TaggedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_EncodingPrefixedType)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TimeType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TIME);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TimeOfDayType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TIME_OF_DAY);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

// This is legacy and included only for backward compatibility.
static bool __asn1_v2_parse_AnyType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ANY | ANY DEFINED BY identifier
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ANY),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFINED),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_BY),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ANY)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BuiltinType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// BitStringType | BooleanType | CharacterStringType | ChoiceType | DateType | DateTimeType | DurationType |
	// EmbeddedPDVType | EnumeratedType | ExternalType | InstanceOfType | IntegerType | IRIType | NullType |
	// ObjectClassFieldType | ObjectIdentifierType | OctetStringType | RealType | RelativeIRIType | RelativeOIDType |
	// SequenceType | SequenceOfType | SetType | SetOfType | PrefixedType | TimeType | TimeOfDayType | AnyType
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CHOICE_10(ctx,
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
		PUSH_OP_CHOICE_10(ctx,
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
		PUSH_OP_CHOICE_8(ctx,
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UsefulType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// GeneralizedTime | UTCTime | ObjectDescriptor
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_GeneralizedTime),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UTCTime),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ObjectDescriptor)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SelectionType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier "<" Type
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_ANGLE_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ReferencedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefinedType | UsefulType | SelectionType | TypeFromObject | ValueSetFromObjects
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UsefulType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SelectionType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeFromObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSetFromObjects)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SubtypeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ElementSetSpecs
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpecs);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_GeneralConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_ConstraintSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SubtypeConstraint | GeneralConstraint
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SubtypeConstraint),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_GeneralConstraint)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Constraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "(" ConstraintSpec ExceptionSpec ")"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ConstraintSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExceptionSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeWithConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SET Constraint OF Type
	// | SET SizeConstraint OF Type
	// | SEQUENCE Constraint OF Type
	// | SEQUENCE SizeConstraint OF Type
	// | SET Constraint OF NamedType
	// | SET SizeConstraint OF NamedType
	// | SEQUENCE Constraint OF NamedType
	// | SEQUENCE SizeConstraint OF NamedType
	PUSH_OP_CHOICE_8(ctx,
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SizeConstraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SizeConstraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		),
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		),
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SET),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SizeConstraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		),
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		),
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SEQUENCE),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SizeConstraint),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OF),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedType)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ConstrainedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Type Constraint | TypeWithConstraint
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeWithConstraint)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Type(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// BuiltinType | ReferencedType | ConstrainedType
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_BuiltinType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ConstrainedType)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier Type
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IdentifierList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier | IdentifierList "," identifier
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_identifier, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BitStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// bstring | hstring | "{" IdentifierList "}" | "{" "}" | CONTAINING Value
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_bstring),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_hstring),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_IdentifierList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CONTAINING),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLBitStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_BooleanValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// TRUE | FALSE
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TRUE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_FALSE)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLBooleanValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_Tuple(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" TableColumn "," TableRow "," Row "," Cell "}"
	PUSH_OP_SEQ_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Quadruple(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharsDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// cstring | Quadruple | Tuple | DefinedValue
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_cstring),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Quadruple),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Tuple),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharSyms(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// CharsDefn | CharSyms "," CharsDefn
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_CharsDefn, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharacterStringList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" CharSyms "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_CharSyms),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_RestrictedCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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

	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UnrestrictedCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SequenceValue
	PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_CharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// RestrictedCharacterStringValue | UnrestrictedCharacterStringValue
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_RestrictedCharacterStringValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UnrestrictedCharacterStringValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLCharacterStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_ChoiceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier ":" Value
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLChoiceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_EmbeddedPDVValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SequenceValue
	PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_EnumeratedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier
	PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLEnumeratedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_ExternalValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SequenceValue
	PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLExternalValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_InstanceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Value
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Value);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLInstanceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_IntegerValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SignedNumber | identifier
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SignedNumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLIntegerValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_ArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// integerUnicodeLabel | non-integerUnicodeLabel
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_integer_unicode_label),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_non_integer_unicode_label)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FirstArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "/" ArcIdentifier
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SLASH),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ArcIdentifier)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SubsequentArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "/" ArcIdentifier SubsequentArcIdentifier | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_SLASH),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ArcIdentifier),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_SubsequentArcIdentifier)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// """ FirstArcIdentifier SubsequentArcIdentifier """
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FirstArcIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SubsequentArcIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}


static bool __asn1_v2_parse_NullValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// NULL
	PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_NULL);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLNullValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}


static bool __asn1_v2_parse_OctetStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLOctetStringValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_NumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// number | DefinedValue
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_number),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NameAndNumberForm(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier "(" NumberForm ")"
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjIdComponents(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// NameForm | NumberForm | NameAndNumberForm | DefinedValue
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NameForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NameAndNumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjIdComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ObjIdComponents | ObjIdComponents ObjIdComponentsList
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_ObjIdComponents);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" ObjIdComponentsList "}" | "{" DefinedValue ObjIdComponentsList "}"
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjIdComponentsList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_4(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjIdComponentsList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLObjectIdentifierValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_NumericRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// realnumber | "-" realnumber | SequenceValue
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_HYPHEN),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_realnumber)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_realnumber),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SequenceValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_SpecialRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// PLUS-INFINITY | MINUS-INFINITY | NOT-A-NUMBER
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PLUS_INFINITY),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_MINUS_INFINITY),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_NOT_A_NUMBER)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_RealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// NumericRealValue | SpecialRealValue
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NumericRealValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SpecialRealValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLRealValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_FirstRelativeArcIdentifier(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ArcIdentifier
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ArcIdentifier);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// """ FirstRelativeArcIdentifier SubsequentArcIdentifier """
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FirstRelativeArcIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SubsequentArcIdentifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOUBLE_QUOTE)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLRelativeIRIValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_RelativeOIDComponents(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// NumberForm | NameAndNumberForm | DefinedValue
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_NameAndNumberForm),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeOIDComponentsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// RelativeOIDComponents | RelativeOIDComponents RelativeOIDComponentsList
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_RelativeOIDComponents);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RelativeOIDValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" RelativeOIDComponentsList "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_RelativeOIDComponentsList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLRelativeOIDValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_ComponentValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// NamedValue | ComponentValueList "," NamedValue
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_NamedValue, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SequenceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" ComponentValueList "}" | "{" "}"
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CHOICE_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentValueList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_CHOICE(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLSequenceValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_ValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Value | ValueList "," Value
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_Value, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedValueList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// NamedValue | ComponentValueList "," NamedValue
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_NamedValue, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SequenceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" ValueList "}" | "{" NamedValueList "}" | "{" "}"
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedValueList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLSequenceOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_SetValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" ComponentValueList "}" | "{" "}"
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CHOICE_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentValueList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_CHOICE(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLSetValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_SetOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" ValueList "}" | "{" NamedValueList "}" | "{" "}"
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_NamedValueList),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLSetOfValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_PrefixedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Value
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Value);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_XMLPrefixedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_TimeValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// tstring
	PUSH_OP_CALL(ctx, __asn1_v2_parse_tstring);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_XMLTimeValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

// This is legacy and included only for backward compatibility.
static bool __asn1_v2_parse_AnyValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Type Value
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_BuiltinValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// BitStringValue | BooleanValue | CharacterStringValue | ChoiceValue | EmbeddedPDVValue | EnumeratedValue |
	// ExternalValue | InstanceOfValue | IntegerValue | IRIValue | NullValue | ObjectIdentifierValue | OctetStringValue |
	// RealValue | RelativeIRIValue | RelativeOIDValue | SequenceValue | SequenceOfValue | SetValue | SetOfValue |
	// PrefixedValue | TimeValue | AnyValue
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CHOICE_10(ctx,
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
		PUSH_OP_CHOICE_10(ctx,
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
		PUSH_OP_CHOICE_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_PrefixedValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TimeValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_AnyValue)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ReferencedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefinedValue | ValueFromObject
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueFromObject)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_OpenTypeFieldVal(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Type ":" Value
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COLON),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FixedTypeFieldVal(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// BuiltinValue | ReferencedValue
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_BuiltinValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectClassFieldValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// OpenTypeFieldVal | FixedTypeFieldVal
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_OpenTypeFieldVal),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FixedTypeFieldVal)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Value(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// BuiltinValue | ReferencedValue | ObjectClassFieldValue
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_BuiltinValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClassFieldValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier Value
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_XMLValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// XMLBuiltinValue | XMLObjectClassFieldValue
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_XMLNamedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "<" & identifier ">" XMLValue "</" & identifier ">"
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_TypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// typereference "::=" Type
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// valuereference Type "::=" Value
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_XMLTypedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_XMLValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// valuereference "::=" XMLTypedValue
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_XMLTypedValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SingleValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Value
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Value);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Includes(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// INCLUDES | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INCLUDES),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}
static bool __asn1_v2_parse_ContainedSubtype(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Includes Type
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Includes),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UpperEndValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// UpperEndValue ::= Value | MAX
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_MAX)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_LowerEndValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// LowerEndValue ::= Value | MIN
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_MIN)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UpperEndpoint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// UpperEndpoint ::= UpperEndValue | "<" UpperEndValue
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UpperEndValue),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_ANGLE_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_UpperEndValue)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_LowerEndpoint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// LowerEndpoint ::= LowerEndValue | LowerEndValue "<"
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_LowerEndValue),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_ANGLE_BRKT)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_LowerEndValue)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// LowerEndpoint ".." UpperEndpoint
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_LowerEndpoint),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_RANGE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UpperEndpoint)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PermittedAlphabet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// FROM Constraint
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_FROM),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SizeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SIZE Constraint
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SIZE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Type
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Type);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PresenceConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// PRESENT | ABSENT | OPTIONAL | empty
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PRESENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ABSENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Constraint | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ComponentConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ValueConstraint PresenceConstraint
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueConstraint),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_PresenceConstraint)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_NamedConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// identifier ComponentConstraint
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_identifier),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ComponentConstraint)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// NamedConstraint | NamedConstraint "," TypeConstraints
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_NamedConstraint, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_MultipleTypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// FullSpecification | PartialSpecification
	// FullSpecification ::= "{" TypeConstraints "}"
	// PartialSpecification ::= "{" "..." "," TypeConstraints "}"
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeConstraints),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		),
		PUSH_OP_SEQ_5(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ELLIPSIS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeConstraints),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_InnerTypeConstraints(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// WITH COMPONENT SingleTypeConstraint
	// | WITH COMPONENTS MultipleTypeConstraints
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_WITH),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_COMPONENT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Constraint)
		),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_WITH),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_COMPONENTS),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_MultipleTypeConstraints)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PatternConstraint(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// PATTERN Value
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_PATTERN),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PropertyAndSettingPair(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// PropertyName "=" SettingName
	// PropertyName ::= psname
	// SettingName ::= psname
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_EQUAL),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_first_name)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PropertySettingsList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// PropertyAndSettingPair | PropertySettingsList PropertyAndSettingPair
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_PropertyAndSettingPair);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PropertySettings(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SETTINGS simplestring
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SETTINGS),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_cstring)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DurationRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ValueRange
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueRange);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TimePointRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ValueRange
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueRange);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RecurrenceRange(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ValueRange
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueRange);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SubtypeElements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CHOICE_10(ctx,
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
		PUSH_OP_CHOICE(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_TimePointRange),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_RecurrenceRange)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Elements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Exclusions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// EXCEPT Elements
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_EXCEPT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Elements)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IntersectionElements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Elements | Elems Exclusions
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Elements),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Exclusions)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Elements)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_IntersectionMark(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "^" | INTERSECTION
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_CARET),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_INTERSECTION)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Intersections(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// IntersectionElements | IElems IntersectionMark IntersectionElements
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_IntersectionElements),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Intersections),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_IntersectionMark),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_IntersectionElements)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UnionMark(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "|" | UNION
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_PIPE),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UNION)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Unions(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Intersections | UElems UnionMark Intersections
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Intersections),
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Unions),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_UnionMark),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Intersections)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ElementSetSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Unions | ALL Exclusions
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Unions),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ALL),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Exclusions)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ElementSetSpecs(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// RootElementSetSpec
	// | RootElementSetSpec "," "..."
	// | RootElementSetSpec "," "..." "," AdditionalElementSetSpec
	PUSH_OP_CHOICE_3(ctx,
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
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpec)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" ElementSetSpecs "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ElementSetSpecs),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueSetTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// typereference Type "::=" ValueSet
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSet)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_XMLFixedTypeFieldVal(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// XMLBuiltinValue
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_XMLOpenTypeFieldVal(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// XMLTypedValue | xmlhstring
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_XMLObjectClassFieldValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// XMLOpenTypeFieldVal | XMLFixedTypeFieldVal
	return __asn1_v2_parse_not_impl_fatal(__func__);
}

static bool __asn1_v2_parse_ReferencedObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefinedObject | ParameterizedObject | DefinedObjectSet | ParameterizedObjectSet
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectSet),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedObjectSet)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ReferencedObjects "." FieldName
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ReferencedObjects "." FieldName
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ReferencedObjects "." FieldName
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectFromObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ReferencedObjects "." FieldName
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSetFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ReferencedObjects "." FieldName
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ReferencedObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_InformationFromObjects(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ValueFromObject | ValueSetFromObjects | TypeFromObject | ObjectFromObject | ObjectSetFromObjects
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueFromObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSetFromObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeFromObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectFromObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSetFromObjects)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}


static bool __asn1_v2_parse_Literal(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// word | ","
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_upper_name),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_COMMA)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_RequiredToken(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Literal | PrimitiveFieldName
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Literal),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_PrimitiveFieldName)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_OptionalGroup(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "[" TokenOrGroupSpec empty + "]"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_SQUARE_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TokenOrGroupSpecs),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_SQUARE_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TokenOrGroupSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// RequiredToken | OptionalGroup
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_RequiredToken),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_OptionalGroup)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TokenOrGroupSpecs(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// TokenOrGroupSpec | TokenOrGroupSpec TokenOrGroupSpecs
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_TokenOrGroupSpec);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SyntaxList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" TokenOrGroupSpec empty + "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_TokenOrGroupSpecs),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalObjectClassReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// modulereference "." objectclassreference
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectclassreference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalObjectReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// modulereference "." objectreference
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectreference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ExternalObjectSetReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// modulereference "." objectsetreference
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_modulereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_DOT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetreference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_UsefulObjectClassReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// TYPE-IDENTIFIER | ABSTRACT-SYNTAX
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_TYPE_IDENTIFIER),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_ABSTRACT_SYNTAX)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedObjectClass(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ExternalObjectClassReference | objectclassreference | UsefulObjectClassReference
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalObjectClassReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectclassreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_UsefulObjectClassReference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ExternalObjectReference | objectreference
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalObjectReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectreference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ExternalObjectSetReference | objectsetreference
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalObjectSetReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetreference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_PrimitiveFieldName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// typefieldreference | valuefieldreference | valuesetfieldreference | objectfieldreference | objectsetfieldreference
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typefieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuefieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuesetfieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectfieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetfieldreference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FieldName(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// PrimitiveFieldName "." +
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_PrimitiveFieldName, __asn1_v2_parse_token_DOT);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSetOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// OPTIONAL | DEFAULT ObjectSet
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFAULT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSet)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSetFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// objectsetfieldreference DefinedObjectClass ObjectSetOptionalitySpec?
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetfieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSetOptionalitySpec))
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// OPTIONAL | DEFAULT Object
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFAULT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Object)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// objectfieldreference DefinedObjectClass ObjectOptionalitySpec?
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectfieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectOptionalitySpec))
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueSetOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// OPTIONAL | DEFAULT ValueSet
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFAULT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSet)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_VariableTypeValueSetFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// valuesetfieldreference FieldName ValueSetOptionalitySpec?
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuesetfieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSetOptionalitySpec))
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FixedTypeValueSetFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// valuesetfieldreference Type ValueSetOptionalitySpec?
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuesetfieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSetOptionalitySpec))
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ValueOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// OPTIONAL | DEFAULT Value
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFAULT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_VariableTypeValueFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// valuefieldreference FieldName ValueOptionalitySpec?
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuefieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldName),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueOptionalitySpec))
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FixedTypeValueFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// valuefieldreference Type UNIQUE? ValueOptionalitySpec?
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuefieldreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_UNIQUE)),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueOptionalitySpec))
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeOptionalitySpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// OPTIONAL | DEFAULT Type
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_OPTIONAL),
		PUSH_OP_SEQ_2(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_DEFAULT),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
		)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_TypeFieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// typefieldreference TypeOptionalitySpec?
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typefieldreference),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_TypeOptionalitySpec))
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FieldSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_WithSyntaxSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// WITH SYNTAX SyntaxList
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_WITH),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_SYNTAX),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SyntaxList)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FieldSpecList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// FieldSpec | FieldSpec "," FieldSpecList
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_FieldSpec, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectClassDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// CLASS "{" FieldSpec "," + "}" WithSyntaxSpec?
	PUSH_OP_SEQ_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_keyword_CLASS),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_FieldSpecList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_CALL(ctx, __asn1_v2_parse_WithSyntaxSpec))
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectClass(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefinedObjectClass | ObjectClassDefn | ParameterizedObjectClass
	PUSH_OP_CHOICE_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClassDefn),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedObjectClass)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectClassAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// objectclassreference "::=" ObjectClass
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectclassreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClass)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Setting(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Type | Value | ValueSet | Object | ObjectSet
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSet),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Object),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSet)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedSyntaxToken(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Literal | Setting
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Literal),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Setting)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedSyntaxTokenList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefinedSyntaxToken | DefinedSyntaxToken DefinedSyntaxTokenList
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_DefinedSyntaxToken);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefinedSyntax(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" DefinedSyntaxToken empty * "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_OPTIONAL(ctx, PUSH_OP_UNWRAP(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedSyntaxTokenList)
		)),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FieldSettingList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// FieldSetting | FieldSetting "," FieldSettingList
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_FieldSetting, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DefaultSyntax(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_FieldSetting(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// PrimitiveFieldName Setting
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_PrimitiveFieldName),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Setting)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectDefn(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefaultSyntax | DefinedSyntax
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefaultSyntax),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedSyntax)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Object(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefinedObject | ObjectDefn | ObjectFromObject | ParameterizedObject
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectDefn),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectFromObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedObject)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// objectreference DefinedObjectClass "::=" Object
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Assignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Object)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSetElements(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Object | DefinedObjectSet | ObjectSetFromObjects | ParameterizedObjectSet
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Object),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectSet),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSetFromObjects),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterizedObjectSet)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSetSpec(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" ObjectSetSpec "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSetSpec),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ObjectSetAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// objectsetreference DefinedObjectClass "::=" ObjectSet
	PUSH_OP_SEQ_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Assignment),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSet)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ActualParameter(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ActualParameters(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Parameter | Parameter "," Parameters
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_ActualParameter, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ActualParameterList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" ActualParameter "," + "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_UNWRAP(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameters)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedObject(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefinedObject ActualParameterList
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObject),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameterList)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedObjectSet(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefinedObjectSet ActualParameterList
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectSet),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameterList)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedObjectClass(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DefinedObjectClass ActualParameterList
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameterList)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedValueSetType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SimpleDefinedType ActualParameterList
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SimpleDefinedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameterList)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SimpleDefinedValue ActualParameterList
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SimpleDefinedValue),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameterList)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// SimpleDefinedType ActualParameterList
	PUSH_OP_SEQ_2(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_SimpleDefinedType),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ActualParameterList)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SimpleDefinedValue(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ExternalValueReference | valuereference
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalValueReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_SimpleDefinedType(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ExternalTypeReference | typereference
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ExternalTypeReference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DummyReference(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Reference
	PUSH_OP_CALL(ctx, __asn1_v2_parse_Reference);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_DummyGovernor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// DummyReference
	PUSH_OP_CALL(ctx, __asn1_v2_parse_DummyReference);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Governor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Type | DefinedObjectClass
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParamGovernor(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Governor | DummyGovernor
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Governor),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DummyGovernor)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Parameter(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Parameters(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Parameter | Parameter "," Parameters
	PUSH_OP_SEQ_OF_DELIMITED(ctx, __asn1_v2_parse_Parameter, __asn1_v2_parse_token_COMMA);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// "{" Parameter "," + "}"
	PUSH_OP_SEQ_3(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_L_CURLY_BRKT),
		PUSH_OP_UNWRAP(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Parameters)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_R_CURLY_BRKT)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedObjectSetAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// objectsetreference ParameterList DefinedObjectClass "::=" ObjectSet
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectsetreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectSet)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedObjectAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// objectreference ParameterList DefinedObjectClass "::=" Object
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_DefinedObjectClass),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Object)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedObjectClassAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// objectclassreference ParameterList "::=" ObjectClass
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_objectclassreference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ObjectClass)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedValueSetTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// typereference ParameterList Type "::=" ValueSet
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ValueSet)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedValueAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// valuereference ParameterList Type "::=" Value
	PUSH_OP_CHOICE_5(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_valuereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Value)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedTypeAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// typereference ParameterList "::=" Type
	PUSH_OP_CHOICE_4(ctx,
		PUSH_OP_CALL(ctx, __asn1_v2_parse_typereference),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_ParameterList),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_token_ASSIGNMENT),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_Type)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ParameterizedAssignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_Assignment(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_AssignmentList(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Assignment | AssignmentList Assignment
	PUSH_OP_SEQ_OF(ctx, __asn1_v2_parse_Assignment);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ModuleBody(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// Exports Imports AssignmentList | empty
	PUSH_OP_CHOICE(ctx,
		PUSH_OP_SEQ_3(ctx,
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Exports),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_Imports),
			PUSH_OP_CALL(ctx, __asn1_v2_parse_AssignmentList)
		),
		PUSH_OP_CALL(ctx, __asn1_v2_parse_empty)
	);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_ModuleDefinition(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
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
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

static bool __asn1_v2_parse_root(t_asn_v2_parser_ctx *ctx, t_asn_v2_token *token, t_asn_v2_ast **ast)
{
	// ModuleDefinition
	PUSH_OP_CALL(ctx, __asn1_v2_parse_ModuleDefinition);
	if (RUN_OP(ctx, token, ast)) {
		return true;
	}
	return false;
}

int asn1_v2_parse(const char *s, size_t slen, t_asn_v2_ast **ast)
{
	if (NULL == s) return (SSL_ERR);

	__asn1_v2_token_history_init(slen);

	t_asn_v2_parser_ctx ctx = {0};
	t_asn_v2_token token = {.s = s, .slen = slen,};

	if (__asn1_v2_parse_root(&ctx, &token, ast)) {
		if (!(!__asn1_v2_token_next(&token) && token.done)) {
			SSL_LOG(ERROR, "parsing is not complete");
			return (SSL_ERR);
		}
		return (SSL_OK);
	}
	return (SSL_ERR);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static char *__asn1_v2_ast_node_dumps(const t_node_v2 *node)
{
	if (NULL == node) return (ft_strdup("null"));
	t_asn_v2_ast *ast = container_of(node, t_asn_v2_ast, base);
	char *dumps = NULL;
	if (NULL != ast->token) {
		char *toks = __asn1_v2_token_dumps(*ast->token);
		ft_sprintf(&dumps, "kind=%s,parser=%s,token=%s", __asn1_v2_get_ast_kind_name(ast->kind), ast->parser, toks);
	}
	else {
		ft_sprintf(&dumps, "kind=%s,parser=%s", __asn1_v2_get_ast_kind_name(ast->kind), ast->parser);
	}
	return (dumps);
}

char *asn1_v2_ast_dumps(const t_asn_v2_ast *ast)
{
	return (ft_ntree_v2_dumps(&ast->base, __asn1_v2_ast_node_dumps));
}
