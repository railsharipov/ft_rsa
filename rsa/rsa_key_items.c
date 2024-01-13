#include <ssl/ssl.h>
#include <ssl/error.h>
#include <ssl/asn.h>
#include <ssl/rsa.h>
#include <libft/htable.h>

static t_num	*__asn_int(t_htbl *htbl, const char *key);
static int		__private_key_items(t_htbl *asn_items, t_rsa **rsa_key);
static int		__public_key_items(t_htbl *asn_items, t_rsa **rsa_key);

int	rsa_key_items(t_node *asn_key, t_rsa **rsa_key)
{
	t_htbl	*asn_items;
	int		ret;

	if ((NULL == asn_key) || (NULL == rsa_key))
		return (RSA_ERROR(INVALID_INPUT));

	if (NULL == (asn_items = asn_tree_items(asn_key)))
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	if (!ft_strcmp(asn_key->key, "RSA_PRIVATE_KEY"))
		ret = __private_key_items(asn_items, rsa_key);

	else if (!ft_strcmp(asn_key->key, "PUBLIC_KEY"))
		ret = __public_key_items(asn_items, rsa_key);

	else
		ret = RSA_ERROR(INVALID_INPUT);

	asn_tree_items_del(asn_items);

	return (ret);
}

static int	__private_key_items(t_htbl *asn_items, t_rsa **rsa_key)
{
	t_rsa	*key;

	SSL_ALLOC(key, sizeof(t_rsa));

	key->tflag = RSA_PRIVKEY;
	key->version = __asn_int(asn_items, "version");
	key->modulus = __asn_int(asn_items, "modulus");
	key->pubexp = __asn_int(asn_items, "publicExponent");
	key->privexp = __asn_int(asn_items, "privateExponent");
	key->prime1 = __asn_int(asn_items, "prime1");
	key->prime2 = __asn_int(asn_items, "prime2");
	key->exponent1 = __asn_int(asn_items, "exponent1");
	key->exponent2 = __asn_int(asn_items, "exponent2");
	key->coeff = __asn_int(asn_items, "coefficient");
	key->keysize = bnum_lmbit(key->modulus);

	if ((NULL == key->version) || (NULL == key->modulus)
		|| (NULL == key->pubexp) || (NULL == key->privexp)
		|| (NULL == key->prime1) || (NULL == key->prime2)
		|| (NULL == key->exponent1) || (NULL == key->exponent2)
		|| (NULL == key->coeff))
	{
		rsa_key_items_del(key);
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	*rsa_key = key;

	return (SSL_OK);
}

static t_num	*__asn_int(t_htbl *htbl, const char *key)
{
	t_num	*num;
	t_iasn	*item;
	char	*content;
	size_t	size;

	if (NULL == item)
		return (NULL);

	item = (t_iasn *)ft_htbl_get(htbl, key);
	content = (char *)item->content;
	size = item->size;

	num = bnum_create();
	bnum_from_bytes_u(num, content, size);

	return (num);
}

static int	__public_key_items(t_htbl *asn_items, t_rsa **rsa_key)
{
	t_rsa	*key;

	SSL_ALLOC(key, sizeof(t_rsa));

	key->tflag = RSA_PUBKEY;
	key->modulus = __asn_int(asn_items, "modulus");
	key->pubexp = __asn_int(asn_items, "publicExponent");
	key->keysize = bnum_lmbit(key->modulus);

	if ((NULL == key->modulus) || (NULL == key->pubexp))
	{
		SSL_FREE(key);
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	*rsa_key = key;

	return (SSL_OK);
}
