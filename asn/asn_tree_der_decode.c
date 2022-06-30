#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

static t_htbl	*__init_func_htable(void);
static int		__decode_der_encoding(t_der *, t_node *);
static int		__decode_recur(t_node *, unsigned char *, size_t , t_htbl *);
static int		__decode_item(t_iasn *, unsigned char **, size_t *, t_htbl *);

static const int	ASN_TAG	 		= (ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE);
static const int	ASN_PRIMITIVE	= (ASN_ENCODE_PRIMITIVE | ASN_TAG);
static const int	ASN_CONSTRUCT	= (ASN_ENCODE_CONSTRUCT | ASN_TAG);

static const struct {
	unsigned char	type;
	int		(*f_read)(t_iasn *, unsigned char *, size_t);
} T[] = {
	{	(ASN_CONSTRUCT | ASN_TAG_SEQUENCE),		der_read_sequence_octets	},
	{	(ASN_PRIMITIVE | ASN_TAG_BOOLEAN),		der_read_bool_octets		},
	{	(ASN_PRIMITIVE | ASN_TAG_OBJECT_ID),	der_read_oid_octets			},
	{	(ASN_PRIMITIVE | ASN_TAG_INT),			der_read_int_octets			},
	{	(ASN_PRIMITIVE | ASN_TAG_OCTET_STRING),	der_read_ostring_octets		},
	{	(ASN_PRIMITIVE | ASN_TAG_BIT_STRING),	der_read_bitstring_octets	},
	{	(ASN_PRIMITIVE | ASN_TAG_NULL),			der_read_null_octets		}
};

int	asn_tree_der_decode(t_der *der, const char *map, t_node **tree)
{
	int	ret;

	if (NULL == der || NULL == map || NULL == tree)
		return (ASN_ERROR(INVALID_INPUT));

	*tree = asn_tree(map);

	ret = __decode_der_encoding(der, *tree);

	if (ret == SSL_OK)
		return (SSL_OK);

	asn_tree_del(*tree);
	*tree = NULL;

	return (ASN_ERROR(INVALID_DER_ENCODING));
}

static t_htbl	*__init_func_htable(void)
{
	t_htbl	*htbl;

	if (NULL == (htbl = ft_htbl_init(sizeof(T)/sizeof(*T))))
		return (NULL);

	for (int idx = 0; idx < sizeof(T)/sizeof(*T); idx++)
		ft_htbl_add(
			htbl, T[idx].f_read, &(T[idx].type), sizeof(T[idx].type));

	return (htbl);
}

static int	__decode_der_encoding(t_der *der, t_node *tree)
{
	t_htbl			*func_htable;
	unsigned char	*derenc;
	size_t			dersize;
	int				ret;

	func_htable = __init_func_htable();

	derenc = (unsigned char *)der->content;
	dersize = der->size;

	ret = __decode_recur(tree, derenc, dersize, func_htable);

	ft_htbl_del(func_htable);

	return (ret);
}

static	int __decode_recur(
	t_node *node, unsigned char *derenc, size_t dersize, t_htbl *func_htable)
{
	t_iasn			*item;
	unsigned char	*child_derenc;
	size_t			child_derenc_size;
	int				ret;

	if (NULL == node)
		return (SSL_OK);

	item = node->content;

	if (SSL_OK != __decode_item(item, &derenc, &dersize, func_htable))
		return (SSL_ERROR(INVALID_DER_ENCODING));

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
			return (SSL_ERROR(INVALID_DER_ENCODING));
	}
	return (__decode_recur(node->next, derenc, dersize, func_htable));
}

static int	__decode_item(
	t_iasn *item, unsigned char **derenc, size_t *dersize, t_htbl *func_htable)
{
	int			(*f_read)(t_iasn *, unsigned char *, size_t);
	uint8_t		tag;
	size_t		readsize;
	size_t		lensize;
	size_t		len;

	readsize = 0;

	if (NULL == item || NULL == derenc || NULL == dersize)
		return (ASN_ERROR(INVALID_INPUT));

	if (*dersize == 0)
		return (UNSPECIFIED_ERROR);

	if (SSL_OK != der_read_id_tag(&tag, *derenc, *dersize))
		return (UNSPECIFIED_ERROR);

	if (NULL == (f_read = ft_htbl_get(func_htable, &tag, sizeof(tag))))
		return (UNSPECIFIED_ERROR);

	readsize++;

	if (SSL_OK != der_read_len_size(&lensize, *derenc, *dersize))
		return (UNSPECIFIED_ERROR);

	readsize++;

	if (SSL_OK != der_read_len_octets(&len, *derenc, lensize))
		return (UNSPECIFIED_ERROR);

	readsize += lensize;

	if (SSL_OK != f_read(item, *derenc, len))
		return (UNSPECIFIED_ERROR);

	readsize += len;

	*derenc += readsize;
	*dersize -= readsize;

	return (SSL_OK);
}
