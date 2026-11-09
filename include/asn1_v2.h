#ifndef ASN_V2_H
#define ASN_V2_H

#include <common.h>
#include <libft.h>
#include <libft_v2.h>
#include <json_v2.h>
#include <bnum.h>

#define ASN_V2_OID_TREE_FILE_PATH "resources/asn1/oid-tree.json"
#define ASN_V2_OID_MAX_NARCS 16
#define ASN_V2_OID_MAX_ARC_NUMBER SIZE_MAX

typedef enum e_asn_v2_oid_arc_kind {
	ASN_V2_OID_ARC_KIND_NUMERIC = 0,
	ASN_V2_OID_ARC_KIND_NAMED,
	ASN_V2_OID_ARC_KIND_NAMED_AND_NUMERIC,
} t_asn_v2_oid_arc_kind;

typedef struct s_asn_v2_oid_arc {
	t_asn_v2_oid_arc_kind kind;
	union {
		char *name;
		size_t number;
	} as;
} t_asn_v2_oid_arc;

typedef struct s_asn_v2_oid {
	size_t arc_nums[ASN_V2_OID_MAX_NARCS];
	size_t narcs;
} t_asn_v2_oid;

t_asn_v2_oid_arc	*asn1_v2_oid_arc_create(void);
void	asn1_v2_oid_arc_copy(const t_asn_v2_oid_arc *src, t_asn_v2_oid_arc *dst);
void	asn1_v2_oid_arc_clear(t_asn_v2_oid_arc *arc);
void	asn1_v2_oid_arc_delete(t_asn_v2_oid_arc *arc);

int		asn1_v2_oid_tree_create(const char *filename, t_node_v2 **tree);
void	asn1_v2_oid_tree_delete(t_node_v2 *tree);

const char	*asn1_v2_get_oid_name(t_node_v2 *tree, const t_asn_v2_oid *oid);
int		asn1_v2_get_oid_by_name(t_node_v2 *tree, const char *name, t_asn_v2_oid *oid);
int 	asn1_v2_get_arc_number_by_name(t_node_v2 *tree, const char *name, size_t *number);

void	asn1_v2_oid_clear(t_asn_v2_oid *oid);
void	asn1_v2_oid_delete(t_asn_v2_oid *oid);
char	*asn1_v2_oid_dumps(const t_asn_v2_oid *oid);

/****************************************************************************/

typedef enum e_asn_v2_tag_mode {
	ASN_V2_TAG_MODE_DEFAULT = 0,
	ASN_V2_TAG_MODE_EXPLICIT,
	ASN_V2_TAG_MODE_IMPLICIT,
	ASN_V2_TAG_MODE_AUTOMATIC,
} t_asn_v2_tag_mode;

typedef enum e_asn_v2_tag_class {
	ASN_V2_TAG_CLASS_UNIVERSAL = 0,
	ASN_V2_TAG_CLASS_APPLICATION,
	ASN_V2_TAG_CLASS_CONTEXT_SPECIFIC,
	ASN_V2_TAG_CLASS_PRIVATE,
} t_asn_v2_tag_class;

typedef enum t_asn_v2_universal_tag_number {
    ASN_V2_TAG_NUMBER_EOC                = 0,
    ASN_V2_TAG_NUMBER_BOOLEAN            = 1,
    ASN_V2_TAG_NUMBER_INTEGER            = 2,
    ASN_V2_TAG_NUMBER_BIT_STRING         = 3,
    ASN_V2_TAG_NUMBER_OCTET_STRING       = 4,
    ASN_V2_TAG_NUMBER_NULL               = 5,
    ASN_V2_TAG_NUMBER_OBJECT_ID          = 6,
    ASN_V2_TAG_NUMBER_OBJECT_DESCR       = 7,
    ASN_V2_TAG_NUMBER_EXTERNAL           = 8,
    ASN_V2_TAG_NUMBER_REAL               = 9,
    ASN_V2_TAG_NUMBER_ENUMERATED         = 10,
    ASN_V2_TAG_NUMBER_EMBEDDED_PDV       = 11,
    ASN_V2_TAG_NUMBER_UTF8_STRING        = 12,
    ASN_V2_TAG_NUMBER_RELATIVE_OBJECT_ID = 13,
    ASN_V2_TAG_NUMBER_TIME               = 14,
    ASN_V2_TAG_NUMBER_RESERVED_15        = 15,

    ASN_V2_TAG_NUMBER_SEQUENCE           = 16,
    ASN_V2_TAG_NUMBER_SEQUENCE_OF        = 16,
    ASN_V2_TAG_NUMBER_SET                = 17,
    ASN_V2_TAG_NUMBER_SET_OF             = 17,

    ASN_V2_TAG_NUMBER_NUMERIC_STRING     = 18,
    ASN_V2_TAG_NUMBER_PRINTABLE_STRING   = 19,
    ASN_V2_TAG_NUMBER_TELETEX_STRING     = 20,
    ASN_V2_TAG_NUMBER_VIDEOTEX_STRING    = 21,
    ASN_V2_TAG_NUMBER_IA5_STRING         = 22,
    ASN_V2_TAG_NUMBER_UTC_TIME           = 23,
    ASN_V2_TAG_NUMBER_GENERALIZED_TIME   = 24,
    ASN_V2_TAG_NUMBER_GRAPHIC_STRING     = 25,
    ASN_V2_TAG_NUMBER_VISIBLE_STRING     = 26,
    ASN_V2_TAG_NUMBER_GENERAL_STRING     = 27,
    ASN_V2_TAG_NUMBER_UNIVERSAL_STRING   = 28,
    ASN_V2_TAG_NUMBER_CHARACTER_STRING   = 29,
    ASN_V2_TAG_NUMBER_BMP_STRING         = 30,
    ASN_V2_TAG_NUMBER_DATE               = 31,
    ASN_V2_TAG_NUMBER_TIME_OF_DAY        = 32,
    ASN_V2_TAG_NUMBER_DATE_TIME          = 33,
    ASN_V2_TAG_NUMBER_DURATION           = 34,
    ASN_V2_TAG_NUMBER_OID_IRI            = 35,
    ASN_V2_TAG_NUMBER_RELATIVE_OID_IRI   = 36
} t_asn_v2_universal_tag_number;

typedef struct s_asn_v2_tag {
	t_asn_v2_tag_mode 	mode;
	t_asn_v2_tag_class	class;
	uint32_t 	number;
} t_asn_v2_tag;

// TODO: better support for constraints.
// typedef enum e_asn_v2_constraint_kind {
// 	ASN_V2_CONSTRAINT_KIND_VALUE = 0,
// 	ASN_V2_CONSTRAINT_KIND_RANGE,
// 	ASN_V2_CONSTRAINT_KIND_SIZE,
// 	ASN_V2_CONSTRAINT_KIND_UNION,
// 	ASN_V2_CONSTRAINT_KIND_INTERSECTION,
// 	ASN_V2_CONSTRAINT_KIND_EXCEPT,
// 	ASN_V2_CONSTRAINT_KIND_ALL_EXCEPT,
// 	ASN_V2_CONSTRAINT_KIND_FROM,
// 	ASN_V2_CONSTRAINT_KIND_WITH_COMPONENT,
// 	ASN_V2_CONSTRAINT_KIND_WITH_COMPONENTS,
// 	ASN_V2_CONSTRAINT_KIND_ABSENT,
// 	ASN_V2_CONSTRAINT_KIND_PRESENT,
// 	ASN_V2_CONSTRAINT_KIND_OPTIONAL,
// 	ASN_V2_CONSTRAINT_KIND_CONTAINING,
// 	ASN_V2_CONSTRAINT_KIND_ENCODED_BY,
// 	ASN_V2_CONSTRAINT_KIND_CONTAINING_AND_ENCODED_BY,
// 	ASN_V2_CONSTRAINT_KIND_CONSTRAINED_BY,
// } t_asn_v2_constraint_kind;

typedef enum e_asn_v2_constraint_kind {
	ASN_V2_CONSTRAINT_KIND_RANGE = 0,
	ASN_V2_CONSTRAINT_KIND_SIZE,
} t_asn_v2_constraint_kind;

typedef struct s_asn_v2_constraint {
	t_asn_v2_constraint_kind kind;
	union {
		struct { t_num *min; t_num *max; } range;
		struct { size_t min; size_t max; } size;
	} as;
} t_asn_v2_constraint;

typedef struct s_asn_v2_enum {
	char *name;
	t_num number;
} t_asn_v2_enum;

typedef enum e_asn_v2_type_kind {
	ASN_V2_TYPE_KIND_NULL				 = 0,
	ASN_V2_TYPE_KIND_BOOLEAN			 = 1,
	ASN_V2_TYPE_KIND_INTEGER			 = 2,
	ASN_V2_TYPE_KIND_ENUMERATED			 = 3,
	ASN_V2_TYPE_KIND_BIT_STRING			 = 4,
	ASN_V2_TYPE_KIND_OCTET_STRING		 = 5,
	ASN_V2_TYPE_KIND_IA5_STRING			 = 6,
	ASN_V2_TYPE_KIND_UTF8_STRING		 = 7,
	ASN_V2_TYPE_KIND_NUMERIC_STRING		 = 8,
	ASN_V2_TYPE_KIND_PRINTABLE_STRING	 = 9,
	ASN_V2_TYPE_KIND_VISIBLE_STRING		 = 10,
	ASN_V2_TYPE_KIND_UTC_TIME			 = 11,
	ASN_V2_TYPE_KIND_OBJECT_ID			 = 12,
	ASN_V2_TYPE_KIND_OBJECT_DESCR		 = 13,
	ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID	 = 14,
	ASN_V2_TYPE_KIND_SEQUENCE			 = 15,
	ASN_V2_TYPE_KIND_SEQUENCE_OF		 = 16,
	ASN_V2_TYPE_KIND_SET				 = 17,
	ASN_V2_TYPE_KIND_SET_OF				 = 18,
	ASN_V2_TYPE_KIND_ANY				 = 19,
	ASN_V2_TYPE_KIND_CHOICE				 = 20,
	ASN_V2_TYPE_KIND_GENERALIZED_TIME	 = 21,
} t_asn_v2_type_kind;

typedef struct s_asn_v2_value {
	t_asn_v2_type_kind	kind;
	union {
		t_ostring	ostring;
		t_num		number;
		bool		boolean;
		char		*cstring;
		t_asn_v2_enum enumerated;
		struct { t_list arcs; } object_id;
		struct { char *id; struct s_asn_v2_value *value; } choice;
		struct { t_ostring octets; } any;
		struct { t_list	elements; } composite;
	} as;
} t_asn_v2_value;

// typedef enum e_asn_v2_type_kind {
// 	ASN_V2_TYPE_KIND_BUILTIN = 0,
// 	ASN_V2_TYPE_KIND_NAMED,
// 	ASN_V2_TYPE_KIND_REFERENCED,
// 	ASN_V2_TYPE_KIND_COMPONENTS_OF,
// } t_asn_v2_type_kind;
//
// typedef struct s_asn_v2_type {
// 	t_asn_v2_type_kind kind;
// 	char *id;
// 	t_list tags;
// 	t_list constraints;
// 	t_asn_v2_value *default_value;
// 	char *defined_by;
// 	bool optional;
// 	union {
// 		struct { t_asn_v2_builtin_type_kind kind } builtin;
// 		struct { char *module; char *name } referenced;
// 		struct { struct s_asn_v2_type *type } components_of;
// 		struct { t_list elements; } composite;
// 		struct { struct s_asn_v2_type *element_type; } collection;
// 		struct { t_list items; bool extendable; } enumeration;
// 	} as;
// } t_asn_v2_type;

typedef struct s_asn_v2_type {
	t_asn_v2_type_kind kind;
	t_list tags;
	t_list constraints;
	union {
		struct { t_list elements; } composite;
		struct { struct s_asn_v2_type *element_type; } collection;
		struct { t_list items; bool extendable; } enumeration;
	} as;
} t_asn_v2_type;

typedef struct s_asn_v2_component {
	char			*id;
	t_asn_v2_type	*type;
	t_asn_v2_value	*default_value;
	char			*defined_by;
	bool 			optional;
} t_asn_v2_component;

typedef struct s_asn_v2_symbol_table {
	struct s_asn_v2_symbol_table *parent;
	t_htbl_v2 symbols;
} t_asn_v2_symbol_table;

typedef struct s_asn_v2_module {
	t_asn_v2_tag_mode tag_mode;
	t_asn_v2_symbol_table symbol_table;
	char *name;
} t_asn_v2_module;

t_asn_v2_tag *asn1_v2_tag_create(void);
void asn1_v2_tag_clear(t_asn_v2_tag *asn1_tag);
void asn1_v2_tag_delete(t_asn_v2_tag *asn1_tag);
void asn1_v2_tag_copy(const t_asn_v2_tag *src, t_asn_v2_tag *dst);

t_asn_v2_component *asn1_v2_component_create(void);
void asn1_v2_component_clear(t_asn_v2_component *asn1_component);
void asn1_v2_component_delete(t_asn_v2_component *asn1_component);
void asn1_v2_component_copy(const t_asn_v2_component *src, t_asn_v2_component *dst);

t_asn_v2_enum *asn1_v2_enum_create(void);
void asn1_v2_enum_clear(t_asn_v2_enum *asn1_type);
void asn1_v2_enum_delete(t_asn_v2_enum *asn1_type);
void asn1_v2_enum_copy(const t_asn_v2_enum *src, t_asn_v2_enum *dst);

t_asn_v2_value *asn1_v2_value_create(void);
void asn1_v2_value_clear(t_asn_v2_value *asn1_value);
void asn1_v2_value_delete(t_asn_v2_value *asn1_value);
void asn1_v2_value_copy(const t_asn_v2_value *src, t_asn_v2_value *dst);

t_asn_v2_type *asn1_v2_type_create(void);
void asn1_v2_type_clear(t_asn_v2_type *asn1_type);
void asn1_v2_type_delete(t_asn_v2_type *asn1_type);
void asn1_v2_type_copy(const t_asn_v2_type *src, t_asn_v2_type *dst);
bool asn1_v2_type_eq(const t_asn_v2_type *a, const t_asn_v2_type *b);

t_asn_v2_constraint	*asn1_v2_constraint_create(void);
void asn1_v2_constraint_clear(t_asn_v2_constraint *asn1_constraint);
void asn1_v2_constraint_delete(t_asn_v2_constraint *asn1_constraint);
void asn1_v2_constraint_copy(const t_asn_v2_constraint *src, t_asn_v2_constraint *dst);

t_asn_v2_module *asn1_v2_module_create(void);
void asn1_v2_module_clear(t_asn_v2_module *asn1_module);
void asn1_v2_module_delete(t_asn_v2_module *asn1_module);
void asn1_v2_module_copy(const t_asn_v2_module *src, t_asn_v2_module *dst);

t_asn_v2_type *asn1_v2_module_get_type(const t_asn_v2_module *asn1_module, const char *name);
t_asn_v2_value *asn1_v2_module_get_value(const t_asn_v2_module *asn1_module, const char *name);

char *asn1_v2_module_dumps(const t_asn_v2_module *asn1_module);
char *asn1_v2_module_dumpb(const t_asn_v2_module *asn1_module, char *buf, size_t size);
char *asn1_v2_module_pretty_dumps(const t_asn_v2_module *asn1_module, bool colored);

char *asn1_v2_tag_dumps(const t_asn_v2_tag *asn1_tag);
char *asn1_v2_constraint_dumps(const t_asn_v2_constraint *asn1_constraint);
char *asn1_v2_value_dumps(const t_asn_v2_value *asn1_value);
char *asn1_v2_type_dumps(const t_asn_v2_type *asn1_type);
char *asn1_v2_component_dumps(const t_asn_v2_component *asn1_component);

char *asn1_v2_tag_dumpb(const t_asn_v2_tag *asn1_tag, char *buf, size_t size);
char *asn1_v2_value_dumpb(const t_asn_v2_value *asn1_value, char *buf, size_t size);
char *asn1_v2_type_dumpb(const t_asn_v2_type *asn1_type, char *buf, size_t size);
char *asn1_v2_component_dumpb(const t_asn_v2_component *asn1_component, char *buf, size_t size);

const char *asn1_v2_get_tag_class_name(t_asn_v2_tag_class tag_class);
const char *asn1_v2_get_tag_mode_name(t_asn_v2_tag_mode tag_mode);
const char *asn1_v2_get_type_name(t_asn_v2_type_kind type);

t_asn_v2_constraint_kind asn1_v2_get_constraint_type_by_name(const char *name);
t_asn_v2_type_kind asn1_v2_get_type_by_name(const char *name);
t_asn_v2_tag_mode asn1_v2_get_tag_mode_by_name(const char *name);
t_asn_v2_tag_class asn1_v2_get_tag_class_by_name(const char *name);

/****************************************************************************/

typedef enum e_asn_v2_symbol_state {
    ASN_V2_SYMBOL_STATE_UNRESOLVED = 0,
    ASN_V2_SYMBOL_STATE_RESOLVING,
    ASN_V2_SYMBOL_STATE_RESOLVED,
    ASN_V2_SYMBOL_STATE_ERROR
} t_asn_v2_symbol_state;

typedef enum e_asn_v2_symbol_kind {
	ASN_V2_SYMBOL_KIND_TYPE_REF = 0,
	ASN_V2_SYMBOL_KIND_VALUE_REF,
} t_asn_v2_symbol_kind;

typedef struct s_asn_v2_symbol {
	t_asn_v2_symbol_kind kind;
	t_asn_v2_symbol_state state;
	const void *source;
	union {
		struct { t_asn_v2_type *type; } type_ref;
		struct { t_asn_v2_type *type; t_asn_v2_value *value; } value_ref;
	} resolved;
} t_asn_v2_symbol;

t_asn_v2_symbol *asn1_v2_symbol_create(void);
void asn1_v2_symbol_clear(t_asn_v2_symbol *asn1_symbol);
void asn1_v2_symbol_delete(t_asn_v2_symbol *asn1_symbol);
void asn1_v2_symbol_copy(const t_asn_v2_symbol *src, t_asn_v2_symbol *dst);

int	asn1_v2_symbol_register_type(t_asn_v2_symbol_table *symbol_table, const char *symbol_name, const t_json_v2 *source);
int	asn1_v2_symbol_register_value(t_asn_v2_symbol_table *symbol_table, const char *symbol_name, const t_json_v2 *source);

/****************************************************************************/

int	asn1_v2_schema_validate(const t_json_v2 *jschema);
int	asn1_v2_schema_parse(t_asn_v2_module **asn1_module, const t_json_v2 *jschema);

/****************************************************************************/

typedef enum e_asn_v2_token_kind {
	ASN_V2_TOKEN_KIND_EMPTY = 0,
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

typedef struct s_asn_v2_token {
	t_asn_v2_token_kind kind;
	const char *s;
	size_t slen;
	size_t start;
	size_t end;
	bool done;
} t_asn_v2_token;

typedef enum e_asn_v2_ast_kind {
	ASN_V2_DATA_KIND_NULL = 0,
	ASN_V2_DATA_KIND_EMPTY,
	ASN_V2_DATA_KIND_LEAF,
	ASN_V2_DATA_KIND_LIST,
	ASN_V2_DATA_KIND_WRAP,
} t_asn_v2_ast_kind;

typedef struct s_asn_v2_ast {
	t_node_v2 base;
	t_asn_v2_ast_kind kind;
	t_asn_v2_token *token;
	const char *parser;
} t_asn_v2_ast;

int	asn1_v2_parse(const char *s, size_t slen, t_asn_v2_ast **ast);
char *asn1_v2_ast_dumps(const t_asn_v2_ast *ast);

/****************************************************************************/

typedef struct s_der_v2_tag {
	t_asn_v2_tag_class	class;
	uint32_t 	number;
	bool		constructed;
} t_der_v2_tag;

typedef struct s_der_v2_value {
	t_asn_v2_type_kind	kind;
	struct { t_der_v2_tag *items; size_t count; } explicit_tags;
	struct { t_der_v2_tag *items; size_t count; } implicit_tags;
	union {
		t_ostring	ostring;
		char		*cstring;
		t_num		number;
		bool		boolean;
		t_asn_v2_oid object_id;
		t_asn_v2_enum enumerated;
		struct { struct s_der_v2_value *items; size_t count; } composite;
		struct { char *id; struct s_der_v2_value *value; } choice;
		struct { t_ostring octets; } any;
	} as;
} t_der_v2_value;

typedef struct s_der_v2_type {
	t_asn_v2_type_kind kind;
	t_list tags;
	t_list constraints;
	union {
		struct { t_list elements; } composite;
		struct { struct s_der_v2_type *element_type; } collection;
		struct { t_list items; bool extendable; } enumeration;
	} as;
} t_der_v2_type;

typedef struct s_der_v2_component {
    char *id;
	t_der_v2_type *type;
	t_der_v2_value *default_value;
	const struct s_der_v2_component *defined_by;
	bool optional;
} t_der_v2_component;

t_der_v2_tag *der_v2_tag_create(void);
void der_v2_tag_delete(t_der_v2_tag *der_tag);
void der_v2_tag_copy(const t_der_v2_tag *src, t_der_v2_tag *dst);
bool der_v2_tag_eq(const t_der_v2_tag *a, const t_der_v2_tag *b);
t_der_v2_tag der_v2_get_tag_universal(t_asn_v2_type_kind type);

t_der_v2_value *der_v2_value_create(void);
void der_v2_value_copy(t_der_v2_value *src, t_der_v2_value *dst);
void der_v2_value_clear(t_der_v2_value *der_value);
void der_v2_value_delete(t_der_v2_value *der_value);

t_der_v2_type *der_v2_type_create(void);
void der_v2_type_delete(t_der_v2_type *der_type);

t_der_v2_component *der_v2_component_create(void);
void der_v2_component_delete(t_der_v2_component *der_component);
t_der_v2_component *der_v2_component_find_by_id(t_list components, const char *name);

char *der_v2_type_dumps(const t_der_v2_type *der_type);
char *der_v2_type_dumpb(const t_der_v2_type *der_type, char *buf, size_t size);
char *der_v2_type_pretty_dumps(const t_der_v2_type *der_type, bool colored);

char *der_v2_value_dumps(const t_der_v2_value *der_value);
char *der_v2_value_pretty_dumps(const t_der_v2_value *der_value, bool colored);

int	der_v2_value_compile(t_der_v2_value **der_value, const t_asn_v2_value *asn1_value);
int	der_v2_type_compile(t_der_v2_type **der_type, const t_asn_v2_type *asn1_type);
int der_v2_component_compile(t_der_v2_component **der_component, const t_asn_v2_component *asn1_component);

int der_v2_decode(const uint8_t *encoded, size_t encsize, const t_der_v2_type *der_type, t_der_v2_value **der_value);
int der_v2_generic_decode(const uint8_t *encoded, size_t encsize, t_der_v2_value **der_value);

int der_v2_generic_encode(uint8_t **encoded, size_t *encsize, const t_der_v2_value *der_value);
int der_v2_encode(uint8_t **encoded, size_t *encsize, const t_der_v2_type *der_type, const t_der_v2_value *der_value);

#endif
