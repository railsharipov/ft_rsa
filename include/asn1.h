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

typedef struct		s_iasn
{
	uint8_t			tag;
	uint32_t		tagnum;
	char			*description;
	void			*content;
	size_t			size;
}					t_iasn;

struct s_node	*asn1_tree(const char *);
t_node			*asn1_node_create_from_schema(t_node *schema_json);
int				asn1_tree_query(const char *s, t_node *asn1_tree, t_node **ret_asn1_node);
char			*asn1_tree_dumps(struct s_node *);
size_t			asn1_tree_dumpb(struct s_node *, char *, size_t);
void			asn1_tree_del(struct s_node *);
void			*asn1_tree_get(t_node *, const char *);
t_htbl			*asn1_tree_items(struct s_node *);
void			asn1_tree_items_del(t_htbl *);

t_node			*asn1_node_create(t_iasn *asn1_item);
void			asn1_node_clear(t_node *asn1_node);
void			asn1_node_del(t_node *asn1_node);

t_iasn			*asn1_item_create(void);
void			asn1_item_clear(t_iasn *);
void			asn1_item_del(t_iasn *);
void			asn1_item_init(t_iasn *);
t_iasn			*asn1_item_dup(t_iasn *);
int				asn1_item_set_type(t_iasn *, char *type_key);
char			*asn1_item_get_type_name(t_iasn *);
char			*asn1_item_dumps(t_iasn *);
char			*asn1_item_value_dumps(t_iasn *);

void			asn1_print(struct s_node *);
int				asn1_transform(struct s_node *, struct s_node *);
char			*asn1_oid_tree_get_name(const char *);
char			*asn1_oid_tree_get_oid(const char *);

#endif
