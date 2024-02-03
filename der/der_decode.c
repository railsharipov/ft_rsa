#include <ssl/ssl.h>
#include <util/io.h>
#include <ssl/asn.h>
#include <ssl/der.h>
#include <libft/node.h>
#include <libft/htable.h>

static void		__init_func_htable(void);
static void		__del_func_htable(void);
static t_node	*__create_asn_tree(t_iodes *);
static ssize_t	__read_tag(t_iasn *item, t_iodes *iodes);
static ssize_t	__read_content_octets(t_iasn *item, t_iodes *iodes);
static ssize_t	__read_octets_indef(t_iasn *item, t_iodes *iodes);
static ssize_t	__read_octets(t_iasn *item, size_t size, t_iodes *iodes);
static int		__is_construct(t_iasn *item);

static t_node	*__create_construct_node(t_iasn *);
static t_node	*__create_primitive_node(t_iasn *);

static const struct {
	unsigned char	type;
	int		(*f_read)(t_ostring *, unsigned char *, size_t);
} T[] = {
	{	ASN_TAGNUM_SEQUENCE,		der_decode_sequence		},
	{	ASN_TAGNUM_BOOLEAN,			der_decode_bool			},
	{	ASN_TAGNUM_OBJECT_ID,		der_decode_oid			},
	{	ASN_TAGNUM_INT,				der_decode_int			},
	{	ASN_TAGNUM_OCTET_STRING,	der_decode_ostring		},
	{	ASN_TAGNUM_BIT_STRING,		der_decode_bitstring	},
	{	ASN_TAGNUM_NULL,			der_decode_null			}
};

static t_htbl	*__func_htable;
static int		__err;

int	der_decode(t_node **tree, t_iodes *iodes)
{
	if (NULL == tree || NULL == iodes)
		return (DER_ERROR(INVALID_INPUT_ERROR));

	__err = SSL_OK;
	__init_func_htable();

	*tree = __create_asn_tree(iodes);

	__del_func_htable();

	if (__err)
		asn_tree_del(*tree);

	return (__err);
}

static t_node	*__create_asn_tree(t_iodes *iodes)
{
	t_node		*node;
	t_iasn		item;
	ssize_t		rbytes;

	ft_bzero(&item, sizeof(t_iasn));

	// if no tag octets, then end of encoding
	if ((rbytes = __read_tag(&item, iodes)) == 0)
		return (NULL);

	if (rbytes < 0)
		goto error;

	if (__read_content_octets(&item, iodes) < 0)
		goto error;

	if (__is_construct(&item))
		node = __create_construct_node(&item);
	else
		node = __create_primitive_node(&item);

	asn_item_clean(&item);

	if (NULL == node)
		goto error;

	node->next = __create_asn_tree(iodes);
	return (node);

error:
	DER_ERROR("invalid der encoding");
	return (NULL);
}

static ssize_t	__read_tag(t_iasn *item, t_iodes *iodes)
{
	return (der_read_tag(&item->tag, &item->tagnum, iodes));
}

static ssize_t	__read_content_octets(t_iasn *item, t_iodes *iodes)
{
	ssize_t	rbytes;
	ssize_t	tbytes;
	size_t	len;
	uint8_t	lenform;

	if ((rbytes = der_read_len(&len, &lenform, iodes)) <= 0)
		return (-1);

	tbytes += rbytes;

	if (ASN_LEN_LONG == lenform && len == 0) // length is not known (indefinite form)
		rbytes = __read_octets_indef(item, iodes);
	else
		rbytes = __read_octets(item, len, iodes);

	if (rbytes < 0)
		return (-1);

	tbytes += rbytes;

	return (tbytes);
}

static ssize_t	__read_octets_indef(t_iasn *item, t_iodes *iodes)
{
	ssize_t	rbytes;
	char	*content;
	size_t	size;

	rbytes = der_read_octets_indef(&content, &size, iodes);

	if (rbytes < 0)
		return (-1);

	item->content = content;
	item->size = size;

	return (rbytes);
}

static ssize_t	__read_octets(t_iasn *item, size_t size, t_iodes *iodes)
{
	ssize_t	rbytes;
	char	*content;

	SSL_ALLOC(content, size);

	rbytes = der_read_octets(content, size, iodes);

	if (rbytes < 0)
	{
		SSL_FREE(content);
		return (-1);
	}

	item->content = content;
	item->size = size;

	return (rbytes);
}

static int	__is_construct(t_iasn *item)
{
	return (SSL_FLAG(ASN_ENCODE_CONSTRUCT, item->tag));
}

static t_node	*__create_construct_node(t_iasn *item)
{
	t_iodes		temp_iodes;
	t_ostring	osbuf;
	t_node		*child_nodes;
	t_node		*node;

	ft_ostr_set_content(&osbuf, item->content, item->size);

	if (SSL_OK != io_init(&temp_iodes, IO_READ|IO_OSBUF, &osbuf))
		return (NULL);

	if (NULL == (child_nodes = __create_asn_tree(&temp_iodes)))
		return (NULL);

	node = ft_node_create();
	node->nodes = child_nodes;

	return (node);
}

static t_node	*__create_primitive_node(t_iasn *item)
{
	t_node	*node;

	node = ft_node_create();
	node->content = asn_item_dup(item);
	node->size = sizeof(t_iasn);

	return (node);
}

static void	__init_func_htable(void)
{
	t_htbl	*htbl;

	htbl = ft_htbl_init(sizeof(T)/sizeof(*T));

	for (int idx = 0; idx < sizeof(T)/sizeof(*T); idx++) {
		ft_htbl_add_rawkey(htbl, T[idx].f_read, &(T[idx].type), sizeof(T[idx].type));
	}
	__func_htable = htbl;
}

static void	__del_func_htable(void)
{
	ft_htbl_del(__func_htable);
}
