#include <common.h>
#include <des.h>
#include <test.h>
#include <rand.h>
#include <libft/bytes.h>

static int	__test_des_setup(void);
static void	__test_des_cleanup(void);

static int	__test_des_encrypt(void);
static int	__test_des_decrypt(void);

static const char	*__small_text_file_path = "test/files/des/small_text.txt";
// static const char	*__large_text_file_path = "test/files/des/large_text.txt";

static const char	*__des_ecb_kdf_small_cipher_file_path = "test/files/des/des-ecb-kdf-small-cipher";
// static const char	*__des_ecb_kdf_large_cipher_file_path = "test/files/des/des-ecb-kdf-large-cipher";
// static const char	*__des_cbc_kdf_small_cipher_file_path = "test/files/des/des-cbc-kdf-small-cipher";
// static const char	*__des_cbc_kdf_large_cipher_file_path = "test/files/des/des-cbc-kdf-large-cipher";

static t_ostring	__small_text;
static t_ostring	__des_ecb_kdf_small_cipher;

static uint8_t		__salt[8];
static const char	*__salthex = "F122D9DEFE7F91F6";
static const char	*__pass = "banana";

int	test_des(void)
{
	int ret;

	if (SSL_OK != __test_des_setup()) {
		TEST_LOG(ERROR, "failed to setup test");
		TEST_FAIL();
	}

	ret = __test_des_encrypt()
		| __test_des_decrypt();

	__test_des_cleanup();

	return (ret);
}

static int	__test_des_setup(void)
{
	if (SSL_OK != test_read_file(__small_text_file_path, &__small_text)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_read_file(__des_ecb_kdf_small_cipher_file_path, &__des_ecb_kdf_small_cipher)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	ft_hex_to_bytes(__salt, __salthex, 16);

	return (SSL_OK);
}

static void	__test_des_cleanup(void)
{
	return ;
}

static int	__test_des_encrypt(void)
{
	uint8_t key[8];
	t_ostring cipher;
	int ret;

	ret = rand_openssl_kdf(key, __salt, NULL, __pass);
	if (SSL_OK != ret) {
		TEST_LOG(ERROR, "failed to generate key");
		return (SSL_ERR);
	}

	ft_ostr_init(&cipher);
	ret = des_ecb_encrypt(key, &__small_text, &cipher);

	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(cipher.size == __des_ecb_kdf_small_cipher.size);
	TEST_ASSERT(ft_memcmp(cipher.content, __des_ecb_kdf_small_cipher.content, cipher.size) == 0);

    TEST_PASS();
}

static int	__test_des_decrypt(void)
{
    TEST_PASS();
}