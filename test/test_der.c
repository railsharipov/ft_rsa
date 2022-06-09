#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <ssl_test.h>

static int	__test_der_setup(void);
static int	__test_der_append_simple_id_tag(void);
static int	__test_der_append_complex_id_tag(void);
static int	__test_der_encode(void);
static int	__test_der_decode(void);

int	test_der(void)
{
	int	res;

	if (SSL_OK != __test_der_setup())
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	res = __test_der_append_simple_id_tag();
	res |= __test_der_append_complex_id_tag();
	res |= __test_der_encode();
	res |= __test_der_encode();

	return (res);
}

static int	__test_der_setup(void)
{
	return (SSL_OK);
}

static int	__test_der_append_simple_id_tag(void)
{
	t_der		*der;
	uint32_t	tag_num;
	int			flags;
	int			ret;
	int			pass;

	flags = ASN_ENCODE_PRIMITIVE | ASN_TAG_UNIVERSAL;

	tag_num = 10;
	der = der_init();
	ret = der_append_id_tag(der, flags, tag_num);

	pass = TEST_ASSERT(SSL_OK == ret);
	pass |= TEST_ASSERT(der->size == 1);
	pass |= TEST_ASSERT(der->content != NULL);

	if (NULL != der->content)
		pass |= TEST_ASSERT(((char *)der->content)[0] == (flags | tag_num));

	der_del(der);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_der_append_complex_id_tag(void)
{
	t_der		*der;
	uint32_t	tag_num;
	int			flags;
	int			ret;
	int			pass;

	flags = ASN_ENCODE_PRIMITIVE | ASN_TAG_UNIVERSAL;

	tag_num = 60;
	der = der_init();
	ret = der_append_id_tag(der, flags, tag_num);

	pass = TEST_ASSERT(SSL_OK == ret);
	pass |= TEST_ASSERT(der->size > 1);
	pass |= TEST_ASSERT(der->content != NULL);

	if (NULL != der->content)
		pass |= TEST_ASSERT(
			((char *)der->content)[0] == (flags | ASN_TAG_COMPLEX));

	der_del(der);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_der_encode(void)
{
	return (TEST_PASS());
}

static int	__test_der_decode(void)
{
	return (TEST_PASS());
}
