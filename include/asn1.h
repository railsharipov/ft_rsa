#ifndef ASN_1_H
# define ASN_1_H

# include <common.h>
#include <libft.h>

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
} t_asn_v2_tag_mode;

typedef enum e_asn_v2_tag_class {
	ASN_V2_TAG_CLASS_UNIVERSAL,
	ASN_V2_TAG_CLASS_APPLICATION,
	ASN_V2_TAG_CLASS_CONTEXT,
	ASN_V2_TAG_CLASS_PRIVATE,
} t_asn_v2_tag_class;

typedef enum e_asn_v2_type_kind {
	ASN_V2_TYPE_KIND_BOOLEAN,
	ASN_V2_TYPE_KIND_INT,
	ASN_V2_TYPE_KIND_BIT_STRING,
	ASN_V2_TYPE_KIND_OCTET_STRING,
	ASN_V2_TYPE_KIND_IA5_STRING,
	ASN_V2_TYPE_KIND_UTF8_STRING,
	ASN_V2_TYPE_KIND_PRINTABLE_STRING,
	ASN_V2_TYPE_KIND_NULL,
	ASN_V2_TYPE_KIND_OBJECT_ID,
	ASN_V2_TYPE_KIND_OBJECT_DESCR,
	ASN_V2_TYPE_KIND_SEQUENCE,
	ASN_V2_TYPE_KIND_SEQUENCE_OF,
	ASN_V2_TYPE_KIND_SET,
	ASN_V2_TYPE_KIND_SET_OF,
	ASN_V2_TYPE_KIND_CHOICE,
	ASN_V2_TYPE_KIND_TAGGED,
	ASN_V2_TYPE_KIND_REF,
} t_asn_v2_type_kind;

typedef struct s_asn_v2_value {
	void	*content;
	size_t	size;
} t_asn_v2_value;

typedef struct s_asn_v2_tag {
	t_asn_v2_tag_mode	mode;
	t_asn_v2_tag_class	class;
	uint32_t			number;
	uint8_t				complex;
} t_asn_v2_tag;

typedef struct s_asn_v2_type {
	t_asn_v2_type_kind		kind;
	t_asn_v2_tag			tag;
	struct s_asn_v2_type	*base_type;
} t_asn_v2_type;

typedef struct s_asn_v2_typedef {
	t_asn_v2_type_kind		kind;
	struct s_asn_v2_typedef	*base_typedef;
	t_asn_v2_tag			tag;
	t_asn_v2_value			*default_value;
	uint8_t 				optional;
	struct s_asn_v2_typedef	*element_typedef;
	size_t					element_count;
	void 					*elements;
} t_asn_v2_typedef;

typedef struct s_asn_v2_schema {
	t_asn_v2_tag_mode	tag_mode;
	t_asn_v2_typedef	*typedefs;
} t_asn_v2_schema;

#endif
