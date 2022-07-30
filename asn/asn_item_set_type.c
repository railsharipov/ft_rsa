#include <ft_ssl.h>
#include <ssl_asn.h>
#include <bnum.h>

static t_htbl	*__init_htable(void);

struct s_type_ctx
{
	char	*key;
	uint8_t	tagnum;
};

static const struct s_type_ctx	T[] = {
	{	"sequence",		ASN_TAGNUM_OCTET_STRING,	},
	{	"ostring",		ASN_TAGNUM_OCTET_STRING,	},
	{	"bitstring",	ASN_TAGNUM_BIT_STRING,		},
	{	"objectId",		ASN_TAGNUM_OCTET_STRING,	},
	{	"int",			ASN_TAGNUM_INT,			},
	{	"null",			ASN_TAGNUM_NULL,			},
	{	"bool",			ASN_TAGNUM_OCTET_STRING,	},
};

int	asn_item_set_type(t_iasn *item, char *type_key)
{
	struct s_type_ctx	*type_ctx;
	t_htbl				*htable;
	int					ret;

	htable = __init_htable();

	if (NULL != (type_ctx = ft_htbl_get(htable, type_key)))
	{
		item->tagnum = type_ctx->tagnum;
		item->tag = ASN_TAGNUM_UNIVERSAL;

		ret = SSL_OK;
	}
	else
	{
		ret = ASN_ERROR(INVALID_INPUT);
	}

	ft_htbl_del(htable);

	return (ret);
}

static t_htbl	*__init_htable(void)
{
	t_htbl	*htbl;

	htbl = ft_htbl_init(sizeof(T)/sizeof(*T));

	for (int idx = 0; idx < sizeof(T)/sizeof(*T); idx++)
		ft_htbl_add(htbl, (void *)T+idx, T[idx].key);

	return (htbl);
}
