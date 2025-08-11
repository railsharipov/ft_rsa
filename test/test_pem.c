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

static int	__test_pem_decode_pkcs8_unencrypted(void);
static int	__test_pem_decode_pkcs1_encrypted_legacy(void);
static int	__test_pem_encode_pkcs8_unencrypted(void);
static int	__test_pem_encode_pkcs1_encrypted_legacy(void);

static const char	*__pkcs8_privateKeyInfo_pem_file_path = "test/files/keys/pkcs8-privateKeyInfo.pem";
static const char	*__pkcs8_privateKeyInfo_der_file_path = "test/files/keys/pkcs8-privateKeyInfo.der";

static const char	*__pkcs8_subjectPublicKeyInfo_pem_file_path = "test/files/keys/pkcs8-subjectPublicKeyInfo.pem";
static const char	*__pkcs8_subjectPublicKeyInfo_der_file_path = "test/files/keys/pkcs8-subjectPublicKeyInfo.der";

static const char	*__pkcs1_encrypted_rsaPrivateKey_pem_file_path = "test/files/keys/pkcs1-encrypted-rsaPrivateKey.pem";
static const char	*__pkcs8_encryptedPrivateKeyInfo_pem_file_path = "test/files/keys/pkcs8-encryptedPrivateKeyInfo.pem";

static const char	*__pkcs8_privateKeyInfo_schema_json_file_path = "resources/asn/schema-pkcs8-privateKeyInfo.json";

static t_ostring	__pkcs8_privateKeyInfo_der;
static t_ostring	__pkcs8_privateKeyInfo_pem;

static t_ostring	__pkcs8_subjectPublicKeyInfo_pem;
static t_ostring	__pkcs8_subjectPublicKeyInfo_der;

static t_ostring	__pkcs1_encrypted_rsaPrivateKey_pem;
static t_ostring	__pkcs8_encryptedPrivateKeyInfo_pem;

static t_ostring	__privateKeyInfo_schema_json_ostr;

static const char	*__password = "banana";

int	test_pem(void)
{
	int	ret;

	if (SSL_OK != __test_pem_setup()) {
		TEST_LOG(ERROR, "failed to setup test");
		TEST_FAIL();
	}

    ret = __test_pem_decode_pkcs8_unencrypted()
        | __test_pem_decode_pkcs1_encrypted_legacy()
        | __test_pem_encode_pkcs8_unencrypted()
        | __test_pem_encode_pkcs1_encrypted_legacy();

	__test_pem_cleanup();

	return (ret);
}

static int	__test_pem_setup(void)
{
    if (SSL_OK != test_read_file(__pkcs8_privateKeyInfo_pem_file_path, &__pkcs8_privateKeyInfo_pem)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
    if (SSL_OK != test_read_file(__pkcs8_privateKeyInfo_der_file_path, &__pkcs8_privateKeyInfo_der)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
    if (SSL_OK != test_read_file(__pkcs8_encryptedPrivateKeyInfo_pem_file_path, &__pkcs8_encryptedPrivateKeyInfo_pem)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
    if (SSL_OK != test_read_file(__pkcs8_subjectPublicKeyInfo_pem_file_path, &__pkcs8_subjectPublicKeyInfo_pem)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
    if (SSL_OK != test_read_file(__pkcs8_subjectPublicKeyInfo_der_file_path, &__pkcs8_subjectPublicKeyInfo_der)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_read_file(__pkcs8_privateKeyInfo_schema_json_file_path, &__privateKeyInfo_schema_json_ostr)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
    if (SSL_OK != test_read_file(__pkcs1_encrypted_rsaPrivateKey_pem_file_path, &__pkcs1_encrypted_rsaPrivateKey_pem)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}

	return (SSL_OK);
}

static void	__test_pem_cleanup(void)
{
    ft_ostr_clear(&__pkcs8_privateKeyInfo_pem);
    ft_ostr_clear(&__pkcs8_privateKeyInfo_der);
    ft_ostr_clear(&__pkcs8_subjectPublicKeyInfo_pem);
    ft_ostr_clear(&__pkcs8_subjectPublicKeyInfo_der);
	ft_ostr_clear(&__pkcs8_encryptedPrivateKeyInfo_pem);
}

static int	__test_pem_decode_pkcs8_unencrypted(void)
{
	t_ostring	data;
	t_pem		pem;
	int		ret;

	pem_init(&pem);
	ft_ostr_init(&data);

    ret = pem_decode(&pem, &__pkcs8_privateKeyInfo_pem, &data, NULL);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(data.content != NULL);
	TEST_ASSERT(data.size > 0);
    TEST_ASSERT(data.size == __pkcs8_privateKeyInfo_der.size);
    TEST_ASSERT(ft_memcmp(data.content, __pkcs8_privateKeyInfo_der.content, data.size) == 0);

	TEST_PASS();
}

static int	__test_pem_decode_pkcs1_encrypted_legacy(void)
{
	t_ostring	data;
	t_pem		pem;
	int			ret;

	pem_init(&pem);
	ft_ostr_init(&data);

    ret = pem_decode(&pem, &__pkcs1_encrypted_rsaPrivateKey_pem, &data, __password);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(pem.label != NULL);
	TEST_ASSERT(ft_streq(pem.label, "RSA PRIVATE KEY"));
	TEST_ASSERT(pem.proc == PEM_PROC_TYPE_ENCRYPTED);
	TEST_ASSERT(pem.cipher == PEM_CIPHER_DES_CBC);
	TEST_ASSERT(data.content != NULL);
	TEST_ASSERT(data.size > 0);

    // Wrap PKCS#1 privateKey DER encoding into PKCS#8 PrivateKeyInfo ASN.1 tree (legacy PEM carries PKCS#1)
	t_node	*schema_json, *asn_tree, *asn_node;
	t_iasn  *asn_item;
	char	*schema_json_str;
	t_ostring	der;

	schema_json_str = ft_ostr_to_cstr(&__privateKeyInfo_schema_json_ostr, 0, __privateKeyInfo_schema_json_ostr.size);
	ret = json_parse(schema_json_str, &schema_json);
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
    TEST_ASSERT(der.size == __pkcs8_privateKeyInfo_der.size);
    TEST_ASSERT(ft_memcmp(der.content, __pkcs8_privateKeyInfo_der.content, der.size) == 0);

	TEST_PASS();
}

static int	__test_pem_encode_pkcs8_unencrypted(void)
{
	t_ostring	enc;
	t_pem		*pem;
	int			ret;

	pem = pem_create("PRIVATE KEY", NULL, PEM_PROC_TYPE_NONE, PEM_CIPHER_NONE);
    ret = pem_encode(pem, &__pkcs8_privateKeyInfo_der, &enc, NULL);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(enc.content != NULL);
	TEST_ASSERT(enc.size > 0);
    TEST_ASSERT(enc.size == __pkcs8_privateKeyInfo_pem.size);
    TEST_ASSERT(ft_streq(ft_ostr_to_cstr(&enc, 0, enc.size), ft_ostr_to_cstr(&__pkcs8_privateKeyInfo_pem, 0, __pkcs8_privateKeyInfo_pem.size)));

	TEST_PASS();
}

static int	__test_pem_encode_pkcs1_encrypted_legacy(void)
{
	t_ostring	enc;
	t_pem		*pem;
	const char	*salthex = "F122D9DEFE7F91F6";
	uint8_t		salt[8];
	int			ret, idx;

	// // Wrap privateKey DER encoding into privateKeyInfo ASN.1 tree
	// t_node	*schema_json, *asn_tree, *asn_node;
	// t_iasn  *asn_item;
	// char	*schema_json_str;
	// t_ostring	der;

	// schema_json_str = ft_ostr_to_cstr(&__privateKeyInfo_schema_json_ostr, 0, __privateKeyInfo_schema_json_ostr.size);
	// ret = json_parse(schema_json_str, &schema_json);
	// TEST_ASSERT(ret == SSL_OK);
	// TEST_ASSERT(schema_json != NULL);

	// asn_tree = asn_tree_create(schema_json);
	// TEST_ASSERT(asn_tree != NULL);

	// ret = asn_tree_query(".[2]", asn_tree, &asn_node);
	// TEST_ASSERT(ret == SSL_OK);
	// TEST_ASSERT(asn_node != NULL);
	// asn_item = asn_node->content;
	// TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_OCTET_STRING);
	// asn_item->content = enc.content;
	// asn_item->size = enc.size;

	// // Encode privateKeyInfo ASN.1 tree into DER encoding
	// ret = der_encode(asn_tree, &der);
	// TEST_ASSERT(ret == SSL_OK);
	// TEST_ASSERT(der.content != NULL);
	// TEST_ASSERT(der.size > 0);

	ft_hex_to_bytes(salt, salthex, ft_strlen(salthex));
    pem = pem_create("RSA PRIVATE KEY", salt, PEM_PROC_TYPE_ENCRYPTED, PEM_CIPHER_DES_CBC);
	TEST_ASSERT(pem->has_salt == 1);

    ret = pem_encode(pem, &__pkcs8_privateKeyInfo_der, &enc, __password);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(enc.content != NULL);
	TEST_ASSERT(enc.size > 0);

	idx = textutil_findf((char *)enc.content, enc.size, "-----BEGIN RSA PRIVATE KEY-----");
	TEST_ASSERT(idx == 0);
	idx = textutil_findf((char *)enc.content, enc.size, "-----END RSA PRIVATE KEY-----");
	TEST_ASSERT(idx == enc.size - 30);
	idx = textutil_findf((char *)enc.content, enc.size, "Proc-Type: 4,ENCRYPTED");
	TEST_ASSERT(idx == 32);
	idx = textutil_findf((char *)enc.content, enc.size, "DEK-Info: DES-CBC,%s", salthex);
	TEST_ASSERT(idx == 55);

	t_ostring	dec;
	t_pem		dec_pem;

	pem_init(&dec_pem);
    ret = pem_decode(&dec_pem, &__pkcs1_encrypted_rsaPrivateKey_pem, &dec, __password);
	TEST_ASSERT(ret == SSL_OK);

	pem_init(&dec_pem);
	ret = pem_decode(&dec_pem, &enc, &dec, __password);
	TEST_ASSERT(ret == SSL_OK);

	TEST_PASS();
}
