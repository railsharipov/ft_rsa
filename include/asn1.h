#ifndef ASN_1_H
# define ASN_1_H

# include <common.h>
# include <libft.h>
# include <libft_v2.h>
# include <json_v2.h>
# include <bnum.h>

// ASN.1 - Abstract Syntax Notation One
// Refer to X.208, X.209, X.608

// IDENTIFIER OCTET

# define ASN_TAGNUM_UNIVERSAL		0x0
# define ASN_TAGNUM_APPLICATION		0x40
# define ASN_TAGNUM_CONTEXT			0x80
# define ASN_TAGNUM_PRIVATE			0xC0

# define ASN_ENCODE_PRIMITIVE		0x0
# define ASN_ENCODE_CONSTRUCT		0x20

# define ASN_TAGNUM_SIMPLE			0x0
# define ASN_TAGNUM_COMPLEX			0x1F

# define ASN_TAGNUM_BOOLEAN			0x1
# define ASN_TAGNUM_INT				0x2
# define ASN_TAGNUM_BIT_STRING		0x3
# define ASN_TAGNUM_OCTET_STRING	0x4
# define ASN_TAGNUM_NULL			0x5
# define ASN_TAGNUM_OBJECT_ID		0x6
# define ASN_TAGNUM_OBJECT_DESCR	0x7
# define ASN_TAGNUM_SEQUENCE		0x10

# define ASN_TAGNUM_SUBSEQ			0x80
# define ASN_TAGNUM_SUBSEQ_LAST		0x0

// LENGTH OCTET

# define ASN_LEN_SHORT				0x0
# define ASN_LEN_LONG				0x80

# define ASN_TYPE_NAME_SEQUENCE		"sequence"
# define ASN_TYPE_NAME_OSTRING		"ostring"
# define ASN_TYPE_NAME_BITSTRING	"bitstring"
# define ASN_TYPE_NAME_OBJECT_ID	"objectId"
# define ASN_TYPE_NAME_OBJECT_DESC	"objectDesc"
# define ASN_TYPE_NAME_INT			"int"
# define ASN_TYPE_NAME_NULL			"null"
# define ASN_TYPE_NAME_BOOL			"bool"
# define ASN_TYPE_NAME_UNKNOWN		"unknown"

typedef struct s_iasn {
	uint8_t		tag;
	uint32_t	tagnum;
	char		*description;
	void		*content;
	size_t		size;
} t_iasn;

// DEPRECATED ////////////////////////////////////////////
t_node	*asn1_tree(const char *);
void	asn1_tree_del(struct s_node *);
void	*asn1_tree_get(t_node *, const char *);
t_htbl	*asn1_tree_items(struct s_node *);
void	asn1_tree_items_del(t_htbl *);
//////////////////////////////////////////////////////////

t_node	*asn1_node_create(t_iasn *asn1_item);
t_node	*asn1_node_create_from_schema(t_node *schema_json);
char	*asn1_node_dumps(t_node *asn1_node);
size_t	asn1_node_dumpb(t_node *asn1_node, char *buf, size_t bufsize);
void	asn1_node_clear(t_node *asn1_node);
void	asn1_node_del(t_node *asn1_node);

int		asn1_query(const char *s, t_node *asn1_node, t_node **ret_asn1_node);

t_iasn	*asn1_item_create(void);
void	asn1_item_clear(t_iasn *asn1_item);
void	asn1_item_del(t_iasn *asn1_item);
void	asn1_item_init(t_iasn *asn1_item);
t_iasn	*asn1_item_dup(t_iasn *asn1_item);
int		asn1_item_set_type(t_iasn *asn1_item, char *type_name);
char	*asn1_item_get_type_name(t_iasn *asn1_item);
char	*asn1_item_dumps(t_iasn *asn1_item);
char	*asn1_item_value_dumps(t_iasn *asn1_item);

char	*asn1_oid_get_name(const char *oid);
char	*asn1_oid_get_oid(const char *name);

//////////////
// ASN.1 v2 //
//////////////

typedef enum e_asn_v2_tag_mode {
	ASN_V2_TAG_MODE_EXPLICIT,
	ASN_V2_TAG_MODE_IMPLICIT,
	ASN_V2_TAG_MODE_AUTOMATIC,
} t_asn_v2_tag_mode;

typedef enum e_asn_v2_tag_class {
	ASN_V2_TAG_CLASS_UNIVERSAL,
	ASN_V2_TAG_CLASS_APPLICATION,
	ASN_V2_TAG_CLASS_CONTEXT,
	ASN_V2_TAG_CLASS_PRIVATE,
} t_asn_v2_tag_class;

typedef enum e_asn_v2_type_kind {
	ASN_V2_TYPE_KIND_NULL,
	ASN_V2_TYPE_KIND_BOOLEAN,
	ASN_V2_TYPE_KIND_INT,
	ASN_V2_TYPE_KIND_BIT_STRING,
	ASN_V2_TYPE_KIND_OCTET_STRING,
	ASN_V2_TYPE_KIND_IA5_STRING,
	ASN_V2_TYPE_KIND_UTF8_STRING,
	ASN_V2_TYPE_KIND_PRINTABLE_STRING,
	ASN_V2_TYPE_KIND_OBJECT_ID,
	ASN_V2_TYPE_KIND_OBJECT_DESCR,
	ASN_V2_TYPE_KIND_SEQUENCE,
	ASN_V2_TYPE_KIND_SEQUENCE_OF,
	ASN_V2_TYPE_KIND_SET,
	ASN_V2_TYPE_KIND_SET_OF,
	ASN_V2_TYPE_KIND_CHOICE,
	ASN_V2_TYPE_KIND_TAGGED,
} t_asn_v2_type_kind;

typedef enum t_asn_v2_universal_tag_number {
    ASN_V2_TAG_NUMBER_BOOLEAN          = 1,
    ASN_V2_TAG_NUMBER_INTEGER          = 2,
    ASN_V2_TAG_NUMBER_BIT_STRING       = 3,
    ASN_V2_TAG_NUMBER_OCTET_STRING     = 4,
    ASN_V2_TAG_NUMBER_NULL             = 5,
    ASN_V2_TAG_NUMBER_OBJECT_ID        = 6,
    ASN_V2_TAG_NUMBER_OBJECT_DESCR     = 7,

    ASN_V2_TAG_NUMBER_UTF8_STRING      = 12,
    ASN_V2_TAG_NUMBER_PRINTABLE_STRING = 19,
    ASN_V2_TAG_NUMBER_IA5_STRING       = 22,

    ASN_V2_TAG_NUMBER_SEQUENCE         = 16,
    ASN_V2_TAG_NUMBER_SEQUENCE_OF      = 16,
    ASN_V2_TAG_NUMBER_SET              = 17,
    ASN_V2_TAG_NUMBER_SET_OF           = 17,
} t_asn_v2_universal_tag_number;

typedef enum s_asn_v2_value_type {
	ASN_V2_VALUE_TYPE_NULL,
	ASN_V2_VALUE_TYPE_BOOLEAN,
	ASN_V2_VALUE_TYPE_NUMBER,
	ASN_V2_VALUE_TYPE_CSTRING,
	ASN_V2_VALUE_TYPE_BSTRING,
	ASN_V2_VALUE_TYPE_OSTRING,
	ASN_V2_VALUE_TYPE_LIST,
	ASN_V2_VALUE_TYPE_CHOICE,
} t_asn_v2_value_type;

typedef struct s_asn_v2_value {
	t_asn_v2_value_type	type;
	union {
		t_ostring	ostring;
		t_num		number;
		t_list		list;
		bool		boolean;
		char		*cstr;
		struct {
		    char *id;
			struct s_asn_v2_value *value;
		} choice;
	} as;
} t_asn_v2_value;

typedef struct s_asn_v2_tag {
	t_asn_v2_tag_class	class;
	uint32_t 	number;
	bool 		constructed;
} t_asn_v2_tag;

typedef enum e_asn_v2_constraint_type {
	ASN_V2_CONSTRAINT_TYPE_RANGE,
	ASN_V2_CONSTRAINT_TYPE_SIZE,
} t_asn_v2_constraint_type;

typedef struct s_asn_v2_constraint {
	t_asn_v2_constraint_type type;
	union {
		struct { t_num *min; t_num *max; } range;
		struct { size_t min; size_t max; } size;
	} as;
} t_asn_v2_constraint;

typedef struct s_asn_v2_type {
	t_asn_v2_type_kind kind;
	t_list constraints;
	bool is_ref;
	union {
		struct { t_list elements; } composite;
		struct { struct s_asn_v2_type *element_type; } collection;
		struct {
			struct s_asn_v2_type *base_type;
			t_asn_v2_tag_mode tag_mode;
			t_asn_v2_tag tag;
		} tagged;
	} as;
} t_asn_v2_type;

typedef struct s_asn_v2_component {
	char			*id;
	t_asn_v2_type	*type;
	t_asn_v2_value	*default_value;
	bool 			optional;
} t_asn_v2_component;

typedef struct s_asn_v2_module {
	t_asn_v2_tag_mode	tag_mode;
	t_htbl_v2			types;
} t_asn_v2_module;

typedef struct s_der_v2_type {
	t_asn_v2_type_kind kind;
	t_list constraints;
	union {
		struct { t_list elements; } composite;
		struct { struct s_der_v2_type *element_type; } collection;
		struct {
			struct s_der_v2_type *base_type;
			t_asn_v2_tag_mode tag_mode;
			t_asn_v2_tag tag;
		} tagged;
	} as;
} t_der_v2_type;

typedef struct s_der_v2_component {
    char *id;
	t_der_v2_type *type;
	t_asn_v2_value *default_value;
	bool optional;
} t_der_v2_component;

int	asn1_v2_schema_validate(t_json_v2 *jschema);
int	asn1_v2_schema_parse(t_asn_v2_module **asn1_module, t_json_v2 *jschema);

char *asn1_v2_module_dumps(const t_asn_v2_module *asn1_module);
char *asn1_v2_module_dumpb(const t_asn_v2_module *asn1_module, char *buf, size_t size);

const char	*asn1_v2_get_tag_class_name(t_asn_v2_tag_class tag_class);
const char	*asn1_v2_get_tag_mode_name(t_asn_v2_tag_mode tag_mode);
const char	*asn1_v2_get_type_name(t_asn_v2_type_kind type);

int	asn1_v2_module_compile_automatic_tags(t_asn_v2_module **asn1_module_compiled, const t_asn_v2_module *asn1_module);

int	asn1_v2_type_compile(t_der_v2_type **der_type, const t_asn_v2_type *asn1_type);

char *der_v2_type_dumps(const t_der_v2_type *der_type);
char *der_v2_type_dumpb(const t_der_v2_type *der_type, char *buf, size_t size);

#endif
