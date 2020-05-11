#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_rsa.h>

static inline t_num	*__asn_int(t_htbl *htbl, const char *key)
{
	return (((t_iasn *)ft_htbl_get(htbl, key))->content);
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
	key->keysize = lmbit_num(key->modulus);

	if ((NULL == key->version) || (NULL == key->modulus)
		|| (NULL == key->pubexp) || (NULL == key->privexp)
		|| (NULL == key->prime1) || (NULL == key->prime2)
		|| (NULL == key->exponent1) || (NULL == key->exponent2)
		|| (NULL == key->coeff))
	{
		SSL_FREE(key);
		return (SSL_ERR);
	}
	*rsa_key = key;

	return (SSL_OK);
}

static int	__public_key_items(t_htbl *asn_items, t_rsa **rsa_key)
{
	t_rsa	*key;

	SSL_ALLOC(key, sizeof(t_rsa));

	key->tflag = RSA_PUBKEY;
	key->modulus = __asn_int(asn_items, "modulus");
	key->pubexp = __asn_int(asn_items, "publicExponent");
	key->keysize = lmbit_num(key->modulus);

	if ((NULL == key->modulus) || (NULL == key->pubexp))
	{
		SSL_FREE(key);
		return (SSL_ERR);
	}
	*rsa_key = key;

	return (SSL_OK);
}

int	rsa_key_items(t_node *asn_key, t_rsa **rsa_key)
{
	t_htbl	*asn_items;
	int		ret;

	if ((NULL == asn_key) || (NULL == rsa_key))
	{
		return (SSL_ERROR("invalid input"));
	}
	if (NULL == (asn_items = asn_tree_items(asn_key)))
	{
		return (SSL_ERR);
	}
	if (!ft_strcmp(asn_key->key, "RSA_PRIVATE_KEY"))
	{
		ret = __private_key_items(asn_items, rsa_key);
	}
	else if (!ft_strcmp(asn_key->key, "PUBLIC_KEY"))
	{
		ret = __public_key_items(asn_items, rsa_key);
	}
	else
	{
		ret = SSL_ERROR("invalid rsa key input");
	}
	asn_tree_items_del(asn_items);

	return (ret);
}
