#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <ssl_io.h>

static void	__init_func_htable(void);
static void	__del_func_htable(void);
static int	__encode_recursive(t_node *, t_iodes *);
static int	__encode_primitive(t_node *, t_iodes *);
static int	__encode_construct(t_node *, t_iodes *);
static int	__encode_item(t_iasn *, t_iodes *);

static const struct {
	uint32_t	tagnum;
	int			(*f_enc)(t_ostring *, void *, size_t);
} T[] = {
	{	ASN_TAGNUM_SEQUENCE,		der_encode_sequence		},
	{	ASN_TAGNUM_BOOLEAN,			der_encode_bool			},
	{	ASN_TAGNUM_INT,				der_encode_int			},
	{	ASN_TAGNUM_OCTET_STRING,	der_encode_ostring		},
	{	ASN_TAGNUM_BIT_STRING,		der_encode_bitstring	},
	{	ASN_TAGNUM_OBJECT_ID,		der_encode_oid			},
	{	ASN_TAGNUM_NULL,			der_encode_null			}
};

static t_htbl	*__func_htable;


int	der_encode(t_node *tree, t_iodes *iodes)
{
	int		ret;

	if (NULL == tree || NULL == iodes)
		return (ASN_ERROR(INVALID_INPUT));

	__init_func_htable();

	ret = __encode_recursive(tree, iodes);

	__del_func_htable();

	if (SSL_OK != ret)
		return (SSL_ERROR(INVALID_ASN_TREE));

	return (SSL_OK);
}

static int	__encode_recursive(t_node *node, t_iodes *iodes)
{
	if (NULL == node)
		return (SSL_OK);

	if (ft_node_is_parent(node))
	{
		if (SSL_OK != __encode_construct(node->content, iodes))
			return (UNSPECIFIED_ERROR);
	}
	else
	{
		if (SSL_OK != __encode_primitive(node->content, iodes))
			return (UNSPECIFIED_ERROR);
	}

	return (__encode_recursive(node->next, iodes));
}

static int	__encode_primitive(t_node *node, t_iodes *iodes)
{
	t_iasn	*item;

	item = node->content;

	if (NULL == item)
		return (UNSPECIFIED_ERROR);

	item->tag |= ASN_ENCODE_PRIMITIVE;

	if (SSL_OK != __encode_item(item, iodes))
		return (UNSPECIFIED_ERROR);

	return (SSL_OK);
}

static int	__encode_construct(t_node *node, t_iodes *iodes)
{
	t_iasn		*item;
	t_iodes		temp_iodes;
	t_ostring	osbuf;
	int			ret;

	item = node->content;

	if (NULL == item)
		return (UNSPECIFIED_ERROR);

	io_init(&temp_iodes, IO_WRITE|IO_OSBUF, &osbuf);

	ret = __encode_recursive(node->nodes, &temp_iodes);

	if (SSL_OK == ret)
	{
		item->tag |= ASN_ENCODE_CONSTRUCT;
		item->content = osbuf.content;
		item->size = osbuf.size;

		ret = __encode_item(item, iodes);
	}

	util_ostr_clean(&osbuf);
	io_close(&temp_iodes);

	return (ret);
}

static int	__encode_item(t_iasn *item, t_iodes *iodes)
{
	int			(*f_enc)(t_ostring *, void *, size_t);
	t_ostring	osbuf;
	uint8_t		tag;
	uint32_t	tagnum;
	ssize_t		wbytes;

	tag = item->tag;
	tagnum = item->tagnum;

	if (NULL == (f_enc = ft_htbl_get(__func_htable, &tagnum, sizeof(tagnum))))
		return (DER_ERROR(INVALID_ASN_TYPE_TAG));

	if (SSL_OK != der_write_tag(tag, tagnum, iodes))
		return (DER_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != f_enc(&osbuf, item->content, item->size))
		return (DER_ERROR(UNSPECIFIED_ERROR));

	wbytes = der_write_octets(osbuf.content, osbuf.size, iodes);

	util_ostr_clean(&osbuf);

	if (wbytes < 0)
		return (DER_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static void	__init_func_htable(void)
{
	t_htbl	*htbl;
	int		idx;

	htbl = ft_htbl_init(sizeof(T)/sizeof(*T));

	for (idx = 0; idx < sizeof(T)/sizeof(*T); idx++)
		ft_htbl_add(
			htbl, T[idx].f_enc, &(T[idx].tagnum), sizeof(T[idx].tagnum));

	__func_htable = htbl;
}

static void	__del_func_htable(void)
{
	ft_htbl_del(__func_htable);
}
