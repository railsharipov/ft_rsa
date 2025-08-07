#include <common.h>
#include <asn.h>
#include <pem.h>
#include <der.h>
#include <bnum.h>
#include <rand.h>
#include <test.h>
#include <io.h>

static int	__test_pem_setup(void);
static void	__test_pem_cleanup(void);

static int	__test_der_decode_unencrypted(void);
static int	__test_der_decode_encrypted(void);

static const char	*__privateKeyInfo_pem_file_path = "test/testfiles/keys/test-privateKeyInfo.pem";
static const char	*__subjectPublicKeyInfo_pem_file_path = "test/testfiles/keys/test-subjectPublicKeyInfo.pem";

static t_ostring	__privateKeyInfo_pem;
static t_ostring	__subjectPublicKeyInfo_pem;

int	test_pem(void)
{
	if (SSL_OK != __test_pem_setup()) {
		TEST_LOG(ERROR, "failed to setup test");
		TEST_FAIL();
	}

	__test_pem_cleanup();

	return (__test_der_decode_unencrypted()
		| __test_der_decode_encrypted());
}

static int	__test_pem_setup(void)
{
	if (SSL_OK != test_get_file_content(__privateKeyInfo_pem_file_path, &__privateKeyInfo_pem)) {
		TEST_LOG(ERROR, "failed to get file content");
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__subjectPublicKeyInfo_pem_file_path, &__subjectPublicKeyInfo_pem)) {
		TEST_LOG(ERROR, "failed to get file content");
		return (SSL_ERR);
	}

	return (SSL_OK);
}

static void	__test_pem_cleanup(void)
{
	return ;
}

static int	__test_der_decode_unencrypted(void)
{
	t_node	*asn_tree;
	t_iodes	iodes;
	t_ostring	data;
	t_pem	*pem;
	int		ret;

	pem = pem_create("PRIVATE KEY", PEM_PROC_TYPE_NONE, PEM_CIPHER_NONE);
	ft_ostr_init(&data);

	ret = pem_decode(pem, &__privateKeyInfo_pem, &data, NULL);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(data.content != NULL);
	TEST_ASSERT(data.size > 0);

	if (SSL_OK != (ret = io_osbuf(&iodes, IO_READ, &data))) {
		TEST_LOG(ERROR, "failed to setup iodess");
		TEST_FAIL();
	}
	ret = der_decode(&asn_tree, &iodes);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_tree != NULL);

	TEST_PASS();
}

static int	__test_der_decode_encrypted(void)
{
	TEST_PASS();
}