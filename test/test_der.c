#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <ssl_test.h>

static int	__test_der_setup(void);
static int	__test_der_append_simple_id_tag(void);
static int	__test_der_append_complex_id_tag(void);
static int	__test_der_append_len_long_form(void);
static int	__test_der_append_len_short_form(void);
static int	__test_der_append_content(void);
static int	__test_der_append_ostring(void);
static int	__test_der_append_bitstring(void);
static int	__test_der_append_bool(void);
static int	__test_der_append_int(void);
static int	__test_der_append_null(void);
static int	__test_der_append_oid(void);
static int	__test_der_append_sequence(void);
static int	__test_der_encode(void);
static int	__test_der_decode(void);

int	test_der(void)
{
	int	res;

	if (SSL_OK != __test_der_setup())
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	res = __test_der_append_simple_id_tag();
	res |= __test_der_append_complex_id_tag();
	res |= __test_der_append_len_long_form();
	res |= __test_der_append_len_short_form();
	res |= __test_der_append_content();
	res |= __test_der_append_ostring();
	res |= __test_der_append_bitstring();
	res |= __test_der_append_bool();
	res |= __test_der_append_int();
	res |= __test_der_append_null();
	res |= __test_der_append_oid();
	res |= __test_der_append_sequence();

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

static int	__test_der_append_len_long_form(void)
{
	t_der	*der;
	size_t	len;
	int		ret;
	int		pass;
	int		len_size;

	len = 12354;
	len_size = NBITS_TO_NBYTES(util_lmbit(len, 8 * sizeof(len)));

	der = der_init();
	ret = der_append_len(der, len);

	pass = TEST_ASSERT(SSL_OK == ret);
	pass |= TEST_ASSERT(NULL != der->content);
	pass |= TEST_ASSERT(der->size > 1);
	pass |= TEST_ASSERT(ASN_LEN_LONG == (0x80 & der->content[0]));
	pass |= TEST_ASSERT(len_size == (der->content[0] & 0x7F));
	pass |= TEST_ASSERT(len_size == der->size - 1);
	pass |= TEST_ASSERT(
		len == util_bytes_to_uint64(der->content + 1, der->size - 1));

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_der_append_len_short_form(void)
{
	t_der	*der;
	size_t	len;
	int		ret;
	int		pass;
	int		len_size;

	len = 124;

	der = der_init();
	ret = der_append_len(der, len);

	pass = TEST_ASSERT(SSL_OK == ret);
	pass |= TEST_ASSERT(NULL != der->content);
	pass |= TEST_ASSERT(der->size == 1);
	pass |= TEST_ASSERT(ASN_LEN_SHORT == (0x80 & der->content[0]));
	pass |= TEST_ASSERT(len == (0x7F & der->content[0]));

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_der_append_content(void)
{
	t_der	*der;
	char	*content = "known_content";
	size_t	consize = ft_strlen(content);
	int		pass;

	der = der_init();
	der_append_content(der, content, consize);

	pass = TEST_ASSERT(ft_memcmp(der->content, content, consize) == 0);
	pass |= TEST_ASSERT(der->size == consize);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_der_append_ostring(void)
{
	t_der	*der;
	char	*content = "known_content";
	size_t	consize = ft_strlen(content);
	uint8_t	tag;
	int		pass;

	der = der_init();
	der_append_ostring(der, content, consize * CHAR_BIT);

	pass = TEST_ASSERT(der->content != NULL);

	tag = (uint8_t)der->content[0];

	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_UNIVERSAL, tag));
	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_SIMPLE, tag));
	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_OCTET_STRING, tag));
	pass |= TEST_ASSERT(der->size == consize + 2);

	if (der->size >= consize + 2)
		pass |= TEST_ASSERT(ft_memcmp(der->content + 2, content, consize) == 0);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_der_append_bitstring(void)
{
	t_der	*der;
	char	*content = "known_content";
	size_t	consize = ft_strlen(content);
	uint8_t	tag;
	int		pass;

	der = der_init();
	der_append_bitstring(der, content, consize * CHAR_BIT - 2);

	pass = TEST_ASSERT(der->content != NULL);

	tag = (uint8_t)der->content[0];

	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_UNIVERSAL, tag));
	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_SIMPLE, tag));
	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_BIT_STRING, tag));
	pass |= TEST_ASSERT(der->size == consize + 3);

	if (der->size >= consize + 3)
	{
		pass |= TEST_ASSERT(der->content[2] == 2);
		pass |= TEST_ASSERT(ft_memcmp(der->content + 3, content, consize-1) == 0);
		pass |= TEST_ASSERT(der->content[der->size-1] == (content[consize-1] & 0xFC));
	}

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_der_append_bool(void)
{
	t_der	*der;
	uint8_t	tag;
	uint8_t	bool_val;
	int		pass;

	der = der_init();
	bool_val = 1;
	der_append_bool(der, &bool_val, CHAR_BIT);

	pass = TEST_ASSERT(der->content != NULL);

	tag = (uint8_t)der->content[0];

	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_UNIVERSAL, tag));
	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_SIMPLE, tag));
	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_BOOLEAN, tag));
	pass |= TEST_ASSERT(der->size == 3);

	if (der->size >= 3)
		pass |= TEST_ASSERT(ft_memcmp(der->content + 2, &bool_val, 1) == 0);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_der_append_int(void)
{
	t_der	*der;
	char	*content = "known_content";
	size_t	consize = ft_strlen(content);
	uint8_t	tag;
	int		pass;

	der = der_init();
	der_append_int(der, content, consize * CHAR_BIT);

	pass = TEST_ASSERT(der->content != NULL);

	tag = (uint8_t)der->content[0];

	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_UNIVERSAL, tag));
	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_SIMPLE, tag));
	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_INT, tag));
	pass |= TEST_ASSERT(der->size == consize + 2);

	if (der->size >= consize + 2)
		pass |= TEST_ASSERT(ft_memcmp(der->content + 2, content, consize) == 0);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_der_append_null(void)
{
	t_der	*der;
	uint8_t	tag;
	int		pass;

	der = der_init();
	der_append_null(der, NULL, 0);

	pass = TEST_ASSERT(der->content != NULL);

	tag = (uint8_t)der->content[0];

	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_UNIVERSAL, tag));
	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_SIMPLE, tag));
	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_BOOLEAN, tag));
	pass |= TEST_ASSERT(der->size == 2);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_der_append_oid(void)
{
	t_der	*der;
	char	*content = "rsaEncryption";
	uint8_t	ref_content[] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01};
	size_t	consize = ft_strlen(content);
	uint8_t	tag;
	int		pass;

	der = der_init();
	der_append_oid(der, content, consize * CHAR_BIT);

	pass = TEST_ASSERT(der->content != NULL);

	tag = (uint8_t)der->content[0];

	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_UNIVERSAL, tag));
	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_SIMPLE, tag));
	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAG_OBJECT_ID, tag));
	pass |= TEST_ASSERT(der->size == sizeof(ref_content) + 2);

	if (der->size >= sizeof(ref_content) + 2)
		pass |= TEST_ASSERT(
			ft_memcmp(der->content + 2, ref_content, sizeof(ref_content)) == 0);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_der_append_sequence(void)
{
	return (TEST_PASS());
}


static int	__test_der_encode(void)
{
	return (TEST_PASS());
}

static int	__test_der_decode(void)
{
	return (TEST_PASS());
}
