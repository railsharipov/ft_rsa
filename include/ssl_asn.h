#ifndef ASN_SPECIFIC_H
# define ASN_SPECIFIC_H

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

enum	e_asn_lenform
{
	ASN_LEN_DEFINITE = 0,
	ASN_LEN_INDEFINITE
};

# define ASN_ERROR(ERROR)	SSL_ERROR(ADD_ERROR_CTX(ERROR, ASN_ERROR_CTX))

enum  e_asn_error
{
	INVALID_ASN_TREE = 1,
};

typedef struct	s_iasn
{
	uint8_t		tag;
	uint32_t	tagnum;
	char		*description;
	void		*content;
	size_t		size;
}				t_iasn;

struct s_der;
struct s_node;

struct s_node	*asn_tree(const char *);
int				asn_tree_del(struct s_node *);
void			*asn_tree_fetch(t_node *, const char *);
t_htbl			*asn_tree_items(struct s_node *);
void			asn_tree_items_del(t_htbl *);
int 			asn_tree_der_encode(struct s_node *, struct s_der **);
int				asn_tree_der_decode(
					struct s_der *, const char *, struct s_node **);

t_iasn			*asn_item_init(void);
void			asn_item_del(t_iasn *);
void			asn_item_clean(t_iasn *);
t_iasn			*asn_item_dup(t_iasn *);
int				asn_item_set_type(t_iasn *, char *type_key);

void			asn_print(struct s_node *);
int				asn_transform(struct s_node *, struct s_node *);
char			*asn_oid_tree_get_name(const char *);
char			*asn_oid_tree_get_oid(const char *);

#endif
