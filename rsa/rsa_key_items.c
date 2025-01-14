#include <ssl.h>
#include <asn.h>
#include <rsa.h>
#include <libft/htable.h>

static t_num	*__asn_int(t_htbl *htbl, const char *key);
static int		__private_key_items(t_htbl *asn_items, t_rsa **rsa_key);
static int		__public_key_items(t_htbl *asn_items, t_rsa **rsa_key);

int	rsa_key_items(t_node *asn_key, t_rsa **rsa_key)
{
	t_htbl	*asn_items;
	int		ret;

	if (NULL == asn_key || NULL == rsa_key) {
		return (RSA_LOG(ERROR, INVALID_INPUT_ERROR));
	}
	if (NULL == (asn_items = asn_tree_items(asn_key))) {
		return (RSA_LOG(ERROR, "failed to asn decode rsa key"));
	}

	if (!ft_strcmp(asn_key->key, "sequence:RSA_PRIVATE_KEY")) {
		ret = __private_key_items(asn_items, rsa_key);
	} else if (!ft_strcmp(asn_key->key, "sequence:PUBLIC_KEY")) {
		ret = __public_key_items(asn_items, rsa_key);
	} else {
		ret = RSA_LOG(ERROR, "invalid asn key: %s", asn_key->key);
	}
	asn_tree_items_del(asn_items);

	return (ret);
}

static int	__private_key_items(t_htbl *asn_items, t_rsa **rsa_key)
{
	t_rsa	*key;
	int		ret;

	SSL_ALLOC(key, sizeof(t_rsa));

	key->tflag = RSA_PRIVKEY;
	key->version = __asn_int(asn_items, "int:version");
	key->modulus = __asn_int(asn_items, "int:modulus");
	key->pubexp = __asn_int(asn_items, "int:publicExponent");
	key->privexp = __asn_int(asn_items, "int:privateExponent");
	key->prime1 = __asn_int(asn_items, "int:prime1");
	key->prime2 = __asn_int(asn_items, "int:prime2");
	key->exponent1 = __asn_int(asn_items, "int:exponent1");
	key->exponent2 = __asn_int(asn_items, "int:exponent2");
	key->coeff = __asn_int(asn_items, "int:coefficient");

	if (NULL == key->version) {
		RSA_LOG(ERROR, "invalid rsa key: version is missing");
		ret = SSL_ERR;
	}
	if (NULL == key->modulus) {
		RSA_LOG(ERROR, "invalid rsa key: modulus is missing");
		ret = SSL_ERR;
	}
	if (NULL == key->pubexp) {
		RSA_LOG(ERROR, "invalid rsa key: public exponent is missing");
		ret = SSL_ERR;
	}
	if (NULL == key->privexp) {
		RSA_LOG(ERROR, "invalid rsa key: private exponent is missing");
		ret = SSL_ERR;
	}
	if (NULL == key->prime1) {
		RSA_LOG(ERROR, "invalid rsa key: prime1 is missing");
		ret = SSL_ERR;
	}
	if (NULL == key->prime2) {
		RSA_LOG(ERROR, "invalid rsa key: prime2 is missing");
		ret = SSL_ERR;
	}
	if (NULL == key->exponent1) {
		RSA_LOG(ERROR, "invalid rsa key: exponent1 is missing");
		ret = SSL_ERR;
	}
	if (NULL == key->exponent2) {
		RSA_LOG(ERROR, "invalid rsa key: exponent2 is missing");
		ret = SSL_ERR;
	}
	if (NULL == key->coeff) {
		RSA_LOG(ERROR, "invalid rsa key: coefficient is missing");
		ret = SSL_ERR;
	}

	if (SSL_OK != ret) {
		rsa_key_items_del(key);
		return (ret);
	}

	key->keysize = bnum_lmbit(key->modulus);
	*rsa_key = key;

	return (SSL_OK);
}

static t_num	*__asn_int(t_htbl *htbl, const char *key)
{
	t_num	*num;
	t_iasn	*item;
	char	*content;
	size_t	size;

	item = (t_iasn *)ft_htbl_get(htbl, key);

	if (NULL == item) {
		return (NULL);
	}
	content = (char *)item->content;
	size = item->size;
	// TODO: find why content is always NULL
	printf("%p\n", content);

	num = bnum_create();
	bnum_from_bytes_u(num, content, size);

	return (num);
}

static int	__public_key_items(t_htbl *asn_items, t_rsa **rsa_key)
{
	t_rsa	*key;

	SSL_ALLOC(key, sizeof(t_rsa));

	key->tflag = RSA_PUBKEY;
	key->modulus = __asn_int(asn_items, "int:modulus");
	key->pubexp = __asn_int(asn_items, "int:publicExponent");
	key->keysize = bnum_lmbit(key->modulus);

	if ((NULL == key->modulus) || (NULL == key->pubexp)) {
		SSL_FREE(key);
		return (RSA_LOG(ERROR, UNSPECIFIED_ERROR));
	}
	*rsa_key = key;

	return (SSL_OK);
}
