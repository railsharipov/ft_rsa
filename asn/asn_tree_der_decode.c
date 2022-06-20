#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

static const int	ASN_TAG	 		= (ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE);
static const int	ASN_PRIMITIVE	= (ASN_ENCODE_PRIMITIVE | ASN_TAG);
static const int	ASN_CONSTRUCT	= (ASN_ENCODE_CONSTRUCT | ASN_TAG);

static const struct {
	unsigned char	type;
	int		(*f_read)(t_iasn *, char **, size_t *);
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
		ft_htbl_add(
			htbl, T[idx].f_read, &(T[idx].type), sizeof(T[idx].type));
	}
	return (htbl);
}

static int	__decode_item(
	t_iasn *item, char **derenc, size_t *dersize, t_htbl *func_htable)
{
	int	(*f_read)(t_iasn *, void *, size_t *);

	SSL_CHECK((NULL != derenc) && (NULL != *derenc));
	SSL_CHECK(NULL != dersize);

	if (*dersize == 0)
	{
		return (UNSPECIFIED_ERROR);
	}
	if (NULL == (f_read = ft_htbl_get(func_htable, *derenc, 1)))
	{
		return (UNSPECIFIED_ERROR);
	}
	if (SSL_OK != f_read(item, derenc, dersize))
	{
		return (UNSPECIFIED_ERROR);
	}
	return (SSL_OK);
}

static	int __decode_recur(
	t_node *node, char *derenc, size_t dersize, t_htbl *func_htable)
{
	void	(*f_dec)(t_der *, void *, size_t);
	t_iasn	*item;
	char	*child_derenc;
	size_t	child_derenc_size;
	int		ret;

	if (NULL == node)
	{
		return (SSL_OK);
	}
	item = node->content;

	if (SSL_OK != __decode_item(item, &derenc, &dersize, func_htable))
	{
		return (SSL_ERROR(INVALID_DER_ENCODING));
	}
	if (ft_node_is_parent(node))
	{
		child_derenc = item->content;
		child_derenc_size = NBITS_TO_NBYTES(item->bitsize);

		ret = __decode_recur(
			node->nodes, child_derenc, child_derenc_size, func_htable);

		SSL_FREE(item->content);
		item->content = NULL;
		item->bitsize = 0;

		if (SSL_OK != ret)
		{
			return (SSL_ERROR(INVALID_DER_ENCODING));
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
