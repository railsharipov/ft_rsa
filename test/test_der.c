#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <ssl_rand.h>
#include <ssl_test.h>
#include <ssl_io.h>

static int	__test_der_setup(void);
static void	__test_der_cleanup(void);

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
// static int	__test_der_read_octets(void);
// static int	__test_der_read_tag(void);
// static int	__test_der_read_len_size(void);
// static int	__test_der_read_len_octets(void);
// static int	__test_der_read_ostring_octets(void);
// static int	__test_der_read_bitstring_octets(void);
// static int	__test_der_read_bool_octets(void);
// static int	__test_der_read_int_octets(void);
// static int	__test_der_read_null_octets(void);
// static int	__test_der_read_oid_octets(void);
// static int	__test_der_read_sequence_octets(void);

static const size_t		__consize = 256;
static char				*__content;

int	test_der(void)
{
	int	res;

	if (SSL_OK != __test_der_setup())
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	res = SSL_OK;

	// res = __test_der_append_simple_id_tag();
	// res |= __test_der_append_complex_id_tag();
	// res |= __test_der_append_len_long_form();
	// res |= __test_der_append_len_short_form();
	// res |= __test_der_append_content();
	// res |= __test_der_append_ostring();
	// res |= __test_der_append_bitstring();
	// res |= __test_der_append_bool();
	// res |= __test_der_append_int();
	// res |= __test_der_append_null();
	// res |= __test_der_append_oid();
	// res |= __test_der_append_sequence();
	// res |= __test_der_read_octets();
	// res |= __test_der_read_tag();
	// res |= __test_der_read_len_size();
	// res |= __test_der_read_len_octets();
	// res |= __test_der_read_ostring_octets();
	// res |= __test_der_read_bitstring_octets();
	// res |= __test_der_read_bool_octets();
	// res |= __test_der_read_int_octets();
	// res |= __test_der_read_null_octets();
	// res |= __test_der_read_oid_octets();
	// res |= __test_der_read_sequence_octets();

	__test_der_cleanup();

	return (res);
}

static int	__test_der_setup(void)
{
	uint64_t	seed;

	if (SSL_OK != rand_useed(&seed, sizeof(seed)))
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	SSL_ALLOC(__content, __consize);

	if (SSL_OK != rand_bytes(seed, __content, __consize))
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static void	__test_der_cleanup(void)
{
	SSL_FREE(__content);
}

// static int	__test_der_append_simple_id_tag(void)
// {
// 	t_der		*der;
// 	uint32_t	tag_num;
// 	int			flags;
// 	int			ret;
// 	int			pass;
//
// 	flags = ASN_ENCODE_PRIMITIVE | ASN_TAGNUM_UNIVERSAL;
//
// 	tag_num = 10;
// 	der = der_init();
// 	ret = der_append_tag(der, flags, tag_num);
//
// 	pass = TEST_ASSERT(SSL_OK == ret);
// 	pass |= TEST_ASSERT(der->size == 1);
// 	pass |= TEST_ASSERT(der->content != NULL);
// 	pass |= TEST_ASSERT(der->content[0] == (flags | tag_num));
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_append_complex_id_tag(void)
// {
// 	t_der		*der;
// 	uint32_t	tag_num = 0x28d;
// 	uint32_t	tag_num_encoded = 0x850d;
// 	int			flags;
// 	int			ret;
// 	int			pass;
//
// 	flags = ASN_ENCODE_PRIMITIVE | ASN_TAGNUM_UNIVERSAL;
//
// 	der = der_init();
// 	ret = der_append_tag(der, flags, tag_num);
//
// 	pass = TEST_ASSERT(SSL_OK == ret);
// 	pass |= TEST_ASSERT(der->size == 3);
// 	pass |= TEST_ASSERT(der->content != NULL);
// 	pass |= TEST_ASSERT(der->content[0] == (flags | ASN_TAGNUM_COMPLEX));
// 	pass |= TEST_ASSERT(ft_memcmp(der->content+1, &tag_num_encoded, 2));
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_append_len_long_form(void)
// {
// 	t_der	*der;
// 	size_t	len;
// 	int		ret;
// 	int		pass;
// 	int		len_size;
//
// 	len = 12354;
// 	len_size = NBITS_TO_NBYTES(util_lmbit(len, 8 * sizeof(len)));
//
// 	der = der_init();
// 	ret = der_append_len(der, len);
//
// 	pass = TEST_ASSERT(SSL_OK == ret);
// 	pass |= TEST_ASSERT(NULL != der->content);
// 	pass |= TEST_ASSERT(der->size > 1);
// 	pass |= TEST_ASSERT(ASN_LEN_LONG == (0x80 & der->content[0]));
// 	pass |= TEST_ASSERT(len_size == (der->content[0] & 0x7F));
// 	pass |= TEST_ASSERT(len_size == der->size - 1);
// 	pass |= TEST_ASSERT(
// 		len == util_bytes_to_uint64((char *)der->content + 1, der->size - 1));
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_append_len_short_form(void)
// {
// 	t_der	*der;
// 	size_t	len;
// 	int		ret;
// 	int		pass;
// 	int		len_size;
//
// 	len = 124;
//
// 	der = der_init();
// 	ret = der_append_len(der, len);
//
// 	pass = TEST_ASSERT(SSL_OK == ret);
// 	pass |= TEST_ASSERT(NULL != der->content);
// 	pass |= TEST_ASSERT(der->size == 1);
// 	pass |= TEST_ASSERT(ASN_LEN_SHORT == (0x80 & der->content[0]));
// 	pass |= TEST_ASSERT(len == (0x7F & der->content[0]));
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_append_content(void)
// {
// 	t_der	*der;
// 	int		pass;
//
// 	der = der_init();
// 	der_append_content(der, __content, __consize);
//
// 	pass = TEST_ASSERT(ft_memcmp(der->content, __content, __consize) == 0);
// 	pass |= TEST_ASSERT(der->size == __consize);
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_append_ostring(void)
// {
// 	t_der	*der;
// 	uint8_t	tag;
// 	int		pass;
//
// 	der = der_init();
// 	der_append_ostring(der, __content, __consize * CHAR_BIT);
//
// 	pass = TEST_ASSERT(der->content != NULL);
//
// 	tag = (uint8_t)der->content[0];
//
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_UNIVERSAL, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_SIMPLE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_ENCODE_PRIMITIVE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_OCTET_STRING, tag));
// 	pass |= TEST_ASSERT(der->size == __consize + 2);
//
// 	if (der->size >= __consize + 2)
// 		pass |= TEST_ASSERT(ft_memcmp(der->content + 2, __content, __consize) == 0);
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_append_bitstring(void)
// {
// 	t_der	*der;
// 	uint8_t	tag;
// 	int		pass;
//
// 	der = der_init();
// 	der_append_bitstring(der, __content, __consize * CHAR_BIT - 2);
//
// 	pass = TEST_ASSERT(der->content != NULL);
//
// 	tag = (uint8_t)der->content[0];
//
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_UNIVERSAL, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_SIMPLE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_ENCODE_PRIMITIVE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_BIT_STRING, tag));
// 	pass |= TEST_ASSERT(der->size == __consize + 3);
//
// 	if (der->size >= __consize + 3)
// 	{
// 		pass |= TEST_ASSERT(der->content[2] == 2);
// 		pass |= TEST_ASSERT(ft_memcmp(der->content + 3, __content, __consize-1) == 0);
// 		pass |= TEST_ASSERT(
// 			der->content[der->size-1] == (uint8_t)(__content[__consize-1] & 0xFC));
// 	}
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_append_bool(void)
// {
// 	t_der	*der;
// 	uint8_t	tag;
// 	uint8_t	bool_val;
// 	int		pass;
//
// 	der = der_init();
// 	bool_val = 1;
// 	der_append_bool(der, &bool_val, CHAR_BIT);
//
// 	pass = TEST_ASSERT(der->content != NULL);
//
// 	tag = (uint8_t)der->content[0];
//
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_UNIVERSAL, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_SIMPLE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_ENCODE_PRIMITIVE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_BOOLEAN, tag));
// 	pass |= TEST_ASSERT(der->size == 3);
//
// 	if (der->size >= 3)
// 		pass |= TEST_ASSERT(ft_memcmp(der->content + 2, &bool_val, 1) == 0);
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_append_int(void)
// {
// 	t_der	*der;
// 	uint8_t	tag;
// 	int		pass;
//
// 	der = der_init();
// 	der_append_int(der, __content, __consize * CHAR_BIT);
//
// 	pass = TEST_ASSERT(der->content != NULL);
//
// 	tag = (uint8_t)der->content[0];
//
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_UNIVERSAL, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_SIMPLE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_ENCODE_PRIMITIVE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_INT, tag));
// 	pass |= TEST_ASSERT(der->size == __consize + 2);
//
// 	if (der->size >= __consize + 2)
// 		pass |= TEST_ASSERT(ft_memcmp(der->content + 2, __content, __consize) == 0);
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_append_null(void)
// {
// 	t_der	*der;
// 	uint8_t	tag;
// 	int		pass;
//
// 	der = der_init();
// 	der_append_null(der, NULL, 0);
//
// 	pass = TEST_ASSERT(der->content != NULL);
//
// 	tag = (uint8_t)der->content[0];
//
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_UNIVERSAL, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_SIMPLE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_ENCODE_PRIMITIVE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_BOOLEAN, tag));
// 	pass |= TEST_ASSERT(der->size == 2);
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_append_oid(void)
// {
// 	t_der	*der;
// 	char	*content = "rsaEncryption";
// 	size_t	consize = ft_strlen(content);
// 	uint8_t	ref_content[] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01};
// 	uint8_t	tag;
// 	int		pass;
//
// 	der = der_init();
// 	der_append_oid(der, content, consize * CHAR_BIT);
//
// 	pass = TEST_ASSERT(der->content != NULL);
//
// 	tag = (uint8_t)der->content[0];
//
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_UNIVERSAL, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_SIMPLE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_ENCODE_PRIMITIVE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_OBJECT_ID, tag));
// 	pass |= TEST_ASSERT(der->size == sizeof(ref_content) + 2);
//
// 	if (der->size >= sizeof(ref_content) + 2)
// 		pass |= TEST_ASSERT(
// 			ft_memcmp(der->content + 2, ref_content, sizeof(ref_content)) == 0);
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_append_sequence(void)
// {
// 	t_der	*der;
// 	uint8_t	tag;
// 	int		pass;
//
// 	der = der_init();
// 	der_append_sequence(der, __content, __consize * CHAR_BIT);
//
// 	pass = TEST_ASSERT(der->content != NULL);
//
// 	tag = (uint8_t)der->content[0];
//
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_UNIVERSAL, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_SIMPLE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_SEQUENCE, tag));
// 	pass |= TEST_ASSERT(der->size == __consize + 2);
//
// 	if (der->size >= __consize + 2)
// 		pass |= TEST_ASSERT(ft_memcmp(der->content + 2, __content, __consize) == 0);
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
// 
// static int	__test_der_read_octets(void)
// {
// 	uint8_t	buf[__consize];
// 	int		pass;
// 	ssize_t	rbytes;
// 	t_iodes	iodes;
//
// 	io_init(&iodes, IO_INPUT | IO_OSBUF, __content, __consize);
//
// 	rbytes = der_read_octets(buf, __consize, &io);
//
// 	pass = TEST_ASSERT(rbytes == __consize);
// 	pass |= TEST_ASSERT(ft_memcmp(buf, __content, __consize) == 0);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }


// static int	__test_der_read_tag(void)
// {
// 	t_der	*der;
// 	uint8_t	tag;
// 	int		pass;
//
// 	der = der_init();
// 	der_append_ostring(der, __content, __consize * CHAR_BIT);
// 	der_read_tag(&tag, der->content, der->size);
//
// 	pass = TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_UNIVERSAL, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_SIMPLE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_ENCODE_PRIMITIVE, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_OCTET_STRING, tag));
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_read_tagnum_octets(void)
// {
// 	t_der		*der;
// 	uint8_t		tag;
// 	uint32_t	tag_num_ref;
// 	uint32_t	tag_num_encoded_ref;
// 	uint32_t	tag_num;
// 	size_t		tag_num_size;
// 	int			pass;
//
// 	tag_num_ref = 20u;
//
// 	der = der_init();
// 	der_append_tag(der, ASN_TAGNUM_UNIVERSAL, tag_num_ref);
// 	der_read_tag(&tag, der->content, der->size);
// 	der_read_tagnum_octets(&tag_num, &tag_num_size, der->content, der->size);
//
// 	pass = TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_UNIVERSAL, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_SIMPLE, tag));
// 	pass |= TEST_ASSERT(tag_num_size == 1);
// 	pass |= TEST_ASSERT(tag_num == tag_num_ref);
//
// 	der_del(der);
//
// 	tag_num_ref = 0x28d;
// 	tag_num_encoded_ref = 0x850d;
//
// 	der = der_init();
// 	der_append_tag(der, ASN_TAGNUM_UNIVERSAL, tag_num_ref);
// 	der_read_tag(&tag, der->content, der->size);
// 	der_read_tagnum_octets(&tag_num, &tag_num_size, der->content, der->size);
//
// 	pass = TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_UNIVERSAL, tag));
// 	pass |= TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_COMPLEX, tag));
// 	pass |= TEST_ASSERT(tag_num_size == 2);
// 	pass |= TEST_ASSERT(ft_memcmp(der->content+1, &tag_num_encoded_ref, 2));
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_read_len_size(void)
// {
// 	t_der		*der;
// 	size_t		lensize;
// 	int			pass;
//
// 	der = der_init();
// 	der_append_len(der, __consize);
// 	der_read_len_size(&lensize, der->content, der->size);
//
// 	pass = TEST_ASSERT(lensize = __consize);
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_read_len_octets(void)
// {
// 	t_der		*der;
// 	size_t		len;
// 	size_t		read_size;
// 	int			pass;
//
// 	der = der_init();
// 	der_append_len(der, 60u);
// 	der_read_len_octets(&len, &read_size, der->content, der->size);
//
// 	pass = TEST_ASSERT(len == 60u);
//
// 	der_del(der);
//
// 	der = der_init();
// 	der_append_len(der, 653u);
// 	der_read_len_octets(&len, &read_size, der->content, der->size);
//
// 	pass = TEST_ASSERT(len == 653u);
//
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_read_ostring_octets(void)
// {
// 	t_iasn		*item;
// 	t_der		*der;
// 	uint8_t		tag;
// 	size_t		len;
// 	size_t		read_size;
// 	int			pass;
//
// 	der = der_init();
// 	item = asn_item_init();
// 	der_append_ostring(der, __content, CHAR_BIT*__consize);
// 	der_read_tag(&tag, der->content, der->size);
// 	der_read_len_octets(&len, &read_size, der->content+1, der->size-1);
// 	der_read_ostring_octets(item, der->content+read_size+1, der->size-read_size-1);
//
//
// 	pass = TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_OCTET_STRING, tag));
// 	pass |= TEST_ASSERT(item->content != NULL);
//
// 	if (NULL != item->content)
// 	{
// 		pass |= TEST_ASSERT(item->bitsize == CHAR_BIT*__consize);
// 		pass |= TEST_ASSERT(ft_memcmp(item->content, __content, __consize) == 0);
// 	}
//
// 	asn_item_del(item);
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_read_bitstring_octets(void)
// {
// 	t_iasn		*item;
// 	t_der		*der;
// 	uint8_t		tag;
// 	size_t		len;
// 	size_t		read_size;
// 	int			pass;
//
// 	der = der_init();
// 	item = asn_item_init();
// 	der_append_bitstring(der, __content, CHAR_BIT*__consize-2);
// 	der_read_tag(&tag, der->content, der->size);
// 	der_read_len_octets(&len, &read_size, der->content+1, der->size-1);
// 	der_read_bitstring_octets(item, der->content+read_size+1, der->size-read_size-1);
//
//
// 	pass = TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_BIT_STRING, tag));
// 	pass |= TEST_ASSERT(item->content != NULL);
//
// 	if (NULL != item->content)
// 	{
// 		pass |= TEST_ASSERT(item->bitsize == CHAR_BIT*__consize-2);
// 		pass |= TEST_ASSERT(ft_memcmp(item->content, __content, __consize-1) == 0);
// 		pass |= TEST_ASSERT(
// 			((uint8_t *)item->content)[NBITS_TO_NBYTES(item->bitsize)-1] == (__content[__consize-1] & 0xFC));
// 	}
//
// 	asn_item_del(item);
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_read_bool_octets(void)
// {
// 	t_iasn		*item;
// 	t_der		*der;
// 	uint8_t		tag;
// 	size_t		len;
// 	size_t		read_size;
// 	char		*content = "some_content";
// 	size_t		consize = ft_strlen(content);
// 	int			pass;
//
// 	der = der_init();
// 	item = asn_item_init();
// 	der_append_bool(der, content, consize);
// 	der_read_tag(&tag, der->content, der->size);
// 	der_read_len_octets(&len, &read_size, der->content+1, der->size-1);
// 	der_read_bool_octets(item, der->content+read_size+1, der->size-read_size-1);
//
// 	pass = TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_BOOLEAN, tag));
// 	pass |= TEST_ASSERT(item->content != NULL);
//
// 	if (NULL != item->content)
// 	{
// 		pass |= TEST_ASSERT(item->bitsize == CHAR_BIT);
// 		pass |= TEST_ASSERT(*((uint8_t *)item->content) == 1);
// 	}
//
// 	asn_item_del(item);
// 	der_del(der);
//
// 	der = der_init();
// 	item = asn_item_init();
// 	der_append_bool(der, NULL, 0);
// 	der_read_len_octets(&len, &read_size, der->content+1, der->size-1);
// 	der_read_bool_octets(item, der->content+read_size+1, der->size-read_size-1);
//
// 	pass |= TEST_ASSERT(item->content != NULL);
//
// 	if (NULL != item->content)
// 		pass |= TEST_ASSERT(*((uint8_t *)item->content) == 0);
//
// 	asn_item_del(item);
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_read_int_octets(void)
// {
// 	t_iasn		*item;
// 	t_der		*der;
// 	uint8_t		tag;
// 	size_t		len;
// 	size_t		read_size;
// 	int			pass;
//
// 	der = der_init();
// 	item = asn_item_init();
// 	der_append_int(der, __content, CHAR_BIT*__consize);
// 	der_read_tag(&tag, der->content, der->size);
// 	der_read_len_octets(&len, &read_size, der->content+1, der->size-1);
// 	der_read_int_octets(item, der->content+read_size+1, der->size-read_size-1);
//
//
// 	pass = TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_INT, tag));
// 	pass |= TEST_ASSERT(item->content != NULL);
//
// 	if (item->content != NULL)
// 	{
// 		pass |= TEST_ASSERT(item->bitsize == CHAR_BIT*__consize);
// 		pass |= TEST_ASSERT(ft_memcmp(item->content, __content, __consize) == 0);
// 	}
//
// 	asn_item_del(item);
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_read_null_octets(void)
// {
// 	t_iasn		*item;
// 	t_der		*der;
// 	char		content[8] = {0};
// 	uint8_t		tag;
// 	size_t		len;
// 	size_t		read_size;
// 	int			pass;
//
// 	der = der_init();
// 	item = asn_item_init();
// 	der_append_null(der, content, sizeof(content));
// 	der_read_tag(&tag, der->content, der->size);
// 	der_read_len_octets(&len, &read_size, der->content+1, der->size-1);
// 	der_read_null_octets(item, der->content+read_size+1, der->size-read_size-1);
//
//
// 	pass = TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_NULL, tag));
// 	pass |= TEST_ASSERT(item->content == NULL);
// 	pass |= TEST_ASSERT(item->bitsize == 0);
//
// 	asn_item_del(item);
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_read_oid_octets(void)
// {
// 	t_iasn		*item;
// 	t_der		*der;
// 	uint8_t		tag;
// 	size_t		len;
// 	size_t		read_size;
// 	char		*content = "rsaEncryption";
// 	size_t		consize = ft_strlen(content);
// 	int			pass;
//
// 	der = der_init();
// 	item = asn_item_init();
// 	der_append_oid(der, content, CHAR_BIT*consize);
// 	der_read_tag(&tag, der->content, der->size);
// 	der_read_len_octets(&len, &read_size, der->content+1, der->size-1);
// 	der_read_oid_octets(item, der->content+read_size+1, der->size-read_size-1);
//
//
// 	pass = TEST_ASSERT(SSL_FLAG(ASN_TAGNUM_OBJECT_ID, tag));
// 	pass |= TEST_ASSERT(item->content != NULL);
//
// 	if (NULL != item->content)
// 	{
// 		pass |= TEST_ASSERT(item->bitsize == CHAR_BIT*consize);
// 		pass |= TEST_ASSERT(ft_memcmp(item->content, content, consize) == 0);
// 	}
//
// 	asn_item_del(item);
// 	der_del(der);
//
// 	if (SSL_OK == pass)
// 		return (TEST_PASS());
//
// 	return (TEST_FAIL());
// }
//
// static int	__test_der_read_sequence_octets(void)
// {
// 	return (TEST_PASS());
// }
