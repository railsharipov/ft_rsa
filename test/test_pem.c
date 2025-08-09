#include <common.h>
#include <asn.h>
#include <pem.h>
#include <der.h>
#include <bnum.h>
#include <rand.h>
#include <test.h>
#include <io.h>
#include <textutil.h>
#include <base64.h>
#include <json.h>
#include <libft/bytes.h>

static int	__test_pem_setup(void);
static void	__test_pem_cleanup(void);

static int	__test_pem_decode_unencrypted(void);
static int	__test_pem_decode_encrypted(void);
static int	__test_pem_encode_unencrypted(void);
static int	__test_pem_encode_encrypted(void);

static const char	*__privateKeyInfo_pem_file_path = "test/testfiles/keys/test-privateKeyInfo.pem";
static const char	*__privateKeyInfo_der_file_path = "test/testfiles/keys/test-privateKeyInfo.der";

static const char	*__subjectPublicKeyInfo_pem_file_path = "test/testfiles/keys/test-subjectPublicKeyInfo.pem";
static const char	*__subjectPublicKeyInfo_der_file_path = "test/testfiles/keys/test-subjectPublicKeyInfo.der";

static const char	*__encryptedPrivateKeyInfo_pem_file_path = "test/testfiles/keys/test-encryptedPrivateKeyInfo.pem";
static const char	*__encryptedPrivateKeyInfo_der_file_path = "test/testfiles/keys/test-encryptedPrivateKeyInfo.der";

static const char	*__privateKeyInfo_with_enc_headers_pem_file_path = "test/testfiles/keys/test-privateKeyInfo-with-enc-headers.pem";
static const char	*__privateKeyInfo_with_enc_headers_der_file_path = "test/testfiles/keys/test-privateKeyInfo-with-enc-headers.der";

static const char	*__privateKeyInfo_schema_json_file_path = "resources/asn/schema-privateKeyInfo.json";

static t_ostring	__privateKeyInfo_der;
static t_ostring	__privateKeyInfo_pem;

static t_ostring	__subjectPublicKeyInfo_pem;
static t_ostring	__subjectPublicKeyInfo_der;

static t_ostring	__privateKeyInfo_with_enc_headers_pem;
static t_ostring	__privateKeyInfo_with_enc_headers_der;

static t_ostring	__encryptedPrivateKeyInfo_pem;
static t_ostring	__encryptedPrivateKeyInfo_der;

static t_ostring	__privateKeyInfo_schema_json_ostr;

static const char	*__password = "banana";

int	test_pem(void)
{
	int	ret;

	if (SSL_OK != __test_pem_setup()) {
		TEST_LOG(ERROR, "failed to setup test");
		TEST_FAIL();
	}

	ret = __test_pem_decode_unencrypted()
		| __test_pem_decode_encrypted()
		| __test_pem_encode_unencrypted()
		| __test_pem_encode_encrypted();

	__test_pem_cleanup();

	return (ret);
}

static int	__test_pem_setup(void)
{
	if (SSL_OK != test_get_file_content(__privateKeyInfo_pem_file_path, &__privateKeyInfo_pem)) {
		TEST_LOG(ERROR, "failed to get file content");
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__privateKeyInfo_der_file_path, &__privateKeyInfo_der)) {
		TEST_LOG(ERROR, "failed to get file content");
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__privateKeyInfo_with_enc_headers_pem_file_path, &__privateKeyInfo_with_enc_headers_pem)) {
		TEST_LOG(ERROR, "failed to get file content");
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__privateKeyInfo_with_enc_headers_der_file_path, &__privateKeyInfo_with_enc_headers_der)) {
		TEST_LOG(ERROR, "failed to get file content");
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__subjectPublicKeyInfo_pem_file_path, &__subjectPublicKeyInfo_pem)) {
		TEST_LOG(ERROR, "failed to get file content");
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__subjectPublicKeyInfo_der_file_path, &__subjectPublicKeyInfo_der)) {
		TEST_LOG(ERROR, "failed to get file content");
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__encryptedPrivateKeyInfo_pem_file_path, &__encryptedPrivateKeyInfo_pem)) {
		TEST_LOG(ERROR, "failed to get file content");
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__encryptedPrivateKeyInfo_der_file_path, &__encryptedPrivateKeyInfo_der)) {
		TEST_LOG(ERROR, "failed to get file content");
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__privateKeyInfo_schema_json_file_path, &__privateKeyInfo_schema_json_ostr)) {
		TEST_LOG(ERROR, "failed to get file content");
		return (SSL_ERR);
	}

	return (SSL_OK);
}

static void	__test_pem_cleanup(void)
{
	ft_ostr_clear(&__privateKeyInfo_pem);
	ft_ostr_clear(&__privateKeyInfo_der);
	ft_ostr_clear(&__privateKeyInfo_with_enc_headers_pem);
	ft_ostr_clear(&__privateKeyInfo_with_enc_headers_der);
	ft_ostr_clear(&__subjectPublicKeyInfo_pem);
	ft_ostr_clear(&__subjectPublicKeyInfo_der);
	ft_ostr_clear(&__encryptedPrivateKeyInfo_pem);
	ft_ostr_clear(&__encryptedPrivateKeyInfo_der);
}

static int	__test_pem_decode_unencrypted(void)
{
	t_ostring	data;
	t_pem		pem;
	int		ret;

	pem_init(&pem);
	ft_ostr_init(&data);

	ret = pem_decode(&pem, &__privateKeyInfo_pem, &data, NULL);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(data.content != NULL);
	TEST_ASSERT(data.size > 0);
	TEST_ASSERT(data.size == __privateKeyInfo_der.size);
	TEST_ASSERT(ft_memcmp(data.content, __privateKeyInfo_der.content, data.size) == 0);

	TEST_PASS();
}

static int	__test_pem_decode_encrypted(void)
{
	t_ostring	data;
	t_pem		pem;
	int			ret;

	pem_init(&pem);
	ft_ostr_init(&data);

	ret = pem_decode(&pem, &__privateKeyInfo_with_enc_headers_pem, &data, __password);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(pem.label != NULL);
	TEST_ASSERT(ft_streq(pem.label, "RSA PRIVATE KEY"));
	TEST_ASSERT(pem.proc == PEM_PROC_TYPE_ENCRYPTED);
	TEST_ASSERT(pem.cipher == PEM_CIPHER_DES_CBC);
	TEST_ASSERT(data.content != NULL);
	TEST_ASSERT(data.size > 0);

	// Wrap privateKey DER encoding into privateKeyInfo ASN.1 tree
	t_node	*schema_json, *asn_tree, *asn_node;
	t_iasn  *asn_item;
	t_ostring	der;

	ret = json_parse(ft_ostr_to_cstr(&__privateKeyInfo_schema_json_ostr, 0, __privateKeyInfo_schema_json_ostr.size), &schema_json);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(schema_json != NULL);

	asn_tree = asn_tree_create(schema_json);
	TEST_ASSERT(asn_tree != NULL);

	ret = asn_tree_query(".[2]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_OCTET_STRING);
	asn_item->content = data.content;
	asn_item->size = data.size;

	// Encode privateKeyInfo ASN.1 tree into DER encoding
	ret = der_encode(asn_tree, &der);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(der.content != NULL);
	TEST_ASSERT(der.size > 0);

	// Compare DER encoding with reference DER encoding from OpenSSL
	TEST_ASSERT(der.size == __privateKeyInfo_with_enc_headers_der.size);
	TEST_ASSERT(ft_memcmp(der.content, __privateKeyInfo_with_enc_headers_der.content, der.size) == 0);

	TEST_PASS();
}

static int	__test_pem_encode_unencrypted(void)
{
	t_ostring	enc;
	t_pem		*pem;
	int			ret;

	pem = pem_create("PRIVATE KEY", PEM_PROC_TYPE_NONE, PEM_CIPHER_NONE);
	ret = pem_encode(pem, &__privateKeyInfo_der, &enc, NULL);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(enc.content != NULL);
	TEST_ASSERT(enc.size > 0);
	TEST_ASSERT(enc.size == __privateKeyInfo_pem.size);
	TEST_ASSERT(ft_streq(ft_ostr_to_cstr(&enc, 0, enc.size), ft_ostr_to_cstr(&__privateKeyInfo_pem, 0, __privateKeyInfo_pem.size)));

	TEST_PASS();
}

static int	__test_pem_encode_encrypted(void)
{
	t_ostring	enc;
	t_pem		*pem;
	int			ret, idx;

	pem = pem_create("PRIVATE KEY", PEM_PROC_TYPE_ENCRYPTED, PEM_CIPHER_DES_CBC);
	ret = pem_encode(pem, &__privateKeyInfo_with_enc_headers_der, &enc, __password);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(enc.content != NULL);
	TEST_ASSERT(enc.size > 0);

	idx = textutil_findf((char *)enc.content, enc.size, "-----BEGIN PRIVATE KEY-----");
	TEST_ASSERT(idx == 0);
	idx = textutil_findf((char *)enc.content, enc.size, "-----END PRIVATE KEY-----");
	TEST_ASSERT(idx == enc.size - 26);
	idx = textutil_findf((char *)enc.content, enc.size, "Proc-Type: 4,ENCRYPTED");
	TEST_ASSERT(idx == 28);
	idx = textutil_findf((char *)enc.content, enc.size, "DEK-Info: DES-CBC,");
	TEST_ASSERT(idx == 51);

	TEST_PASS();
}
