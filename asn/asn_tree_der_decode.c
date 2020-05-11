#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_der.h>

static const int	ASN_TAG	 		= (ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE);
static const int	ASN_PRIMITIVE	= (ASN_ENCODE_PRIMITIVE | ASN_TAG);
static const int	ASN_CONSTRUCT	= (ASN_ENCODE_CONSTRUCT | ASN_TAG);

static const struct {
	uint8_t	type;
	int		(*f_read)(t_iasn *, char **, int *);
} T[] = {
	{	(ASN_CONSTRUCT | ASN_TAG_SEQUENCE),		der_read_sequence	},
	{	(ASN_PRIMITIVE | ASN_TAG_BOOLEAN),		der_read_bool		},
	{	(ASN_PRIMITIVE | ASN_TAG_OBJECT_ID),	der_read_oid		},
	{	(ASN_PRIMITIVE | ASN_TAG_INT),			der_read_int		},
	{	(ASN_PRIMITIVE | ASN_TAG_OCTET_STRING),	der_read_ostring	},
	{	(ASN_PRIMITIVE | ASN_TAG_BIT_STRING),	der_read_bitstring	},
	{	(ASN_PRIMITIVE | ASN_TAG_NULL),			der_read_null		}
};

static t_htbl	*__init_func_htable(void)
{
	t_htbl	*htbl;

	if (NULL == (htbl = ft_htbl_init(sizeof(T)/sizeof(*T))))
	{
		return (NULL);
	}
	for (int idx = 0; idx < sizeof(T)/sizeof(*T); idx++)
	{
		ft_htbl_raw_add(
			htbl, &(T[idx].type), sizeof(T[idx].type), T[idx].f_read);
	}
	return (htbl);
}

static int	__decode_item(
	t_iasn *item, char **derenc, int *dersize, t_htbl *func_htable)
{
	int	(*f_read)(t_iasn *, void *, int *);

	SSL_CHECK((NULL != derenc) && (NULL != *derenc));
	SSL_CHECK(NULL != dersize);

	if (*dersize <= 0)
	{
		return (SSL_ERROR("invalid der encoding: bad size"));
	}
	if (NULL == (f_read = ft_htbl_raw_get(func_htable, *derenc, 1)))
	{
		return (SSL_ERROR("invalid der encoding: bad asn tag"));
	}
	if (SSL_OK != f_read(item, derenc, dersize))
	{
		return (SSL_ERROR("invalid der encoding"));
	}
	return (SSL_OK);
}

static	int __decode_recur(
	t_node *node, char *derenc, int dersize, t_htbl *func_htable)
{
	void	(*f_dec)(t_der *, void *, int);
	t_iasn	*item;
	char	*content;
	int		consize;
	int		ret;

	if (NULL == node)
	{
		return (SSL_OK);
	}
	item = node->content;

	if (SSL_OK != __decode_item(item, &derenc, &dersize, func_htable))
	{
		return (SSL_ERROR("der decode error"));
	}
	if (ft_node_is_parent(node))
	{
		ret = __decode_recur(
			node->nodes, item->content, item->size, func_htable);

		SSL_FREE(item->content);
		item->content = NULL;
		item->size = 0;

		if (SSL_OK != ret)
		{
			return (SSL_ERR);
		}
	}
	return (__decode_recur(node->next, derenc, dersize, func_htable));
}

int	asn_tree_der_decode(t_der *der, const char *map, t_node **tree)
{
	t_htbl	*func_htable;
	int		ret;

	SSL_CHECK(NULL != tree);
	SSL_CHECK(NULL != der);
	SSL_CHECK(NULL != map);

	ret = SSL_OK;

	*tree = asn_tree(map);
	func_htable = __init_func_htable();

	if (SSL_OK != (ret = __decode_recur(
		*tree, der->content, der->size, func_htable)))
	{
		asn_tree_del(*tree);
		*tree = NULL;
	}
	ft_htbl_del(func_htable);

	return (ret);
}
