#include <common.h>
#include <logger.h>
#include <des.h>
#include "test.h"
#include <rand.h>
#include <io.h>
#include <file.h>
#include <libft.h>

static int	__test_des_setup(void);
static void	__test_des_cleanup(void);

static int	__test_des_ecb_encrypt(void);
static int	__test_des_ecb_decrypt(void);
static int	__test_des_cbc_encrypt(void);
static int	__test_des_cbc_decrypt(void);

static ssize_t	__helper_des_update(t_des_ctx *des, const t_ostring *in, t_ostring *out);
static ssize_t	__helper_des_final(t_des_ctx *des, t_ostring *out);

static const char	*__small_text_file_path = "tests/files/text/small.txt";
static const char	*__large_text_file_path = "tests/files/text/large.txt";

static const char	*__des_ecb_small_cipher_file_path = "tests/files/des/des-ecb-small-cipher";
static const char	*__des_ecb_large_cipher_file_path = "tests/files/des/des-ecb-large-cipher";
static const char	*__des_cbc_small_cipher_file_path = "tests/files/des/des-cbc-small-cipher";
static const char	*__des_cbc_large_cipher_file_path = "tests/files/des/des-cbc-large-cipher";

static t_ostring	*__small_text;
static t_ostring	*__large_text;
static t_ostring	*__des_ecb_small_cipher;
static t_ostring	*__des_cbc_small_cipher;
static t_ostring	*__des_ecb_large_cipher;
static t_ostring	*__des_cbc_large_cipher;

static const char	*__keyhex = "F122D9DEFE7F91F6";
static const char	*__ivhex = "6F19F7EFED9D221F";

static uint8_t	__key[8];
static uint8_t	__iv[8];

int	test_des(void)
{
	int ret;

	if (SSL_OK != __test_des_setup()) {
		TEST_LOG(ERROR, TEST_SETUP_ERROR);
		TEST_FAIL();
	}

	ret = __test_des_ecb_encrypt()
		| __test_des_ecb_decrypt()
		| __test_des_cbc_encrypt()
		| __test_des_cbc_decrypt();

	__test_des_cleanup();

	return (ret);
}

static int	__test_des_setup(void)
{
	__small_text = ft_ostr_new();
	__large_text = ft_ostr_new();
	__des_ecb_small_cipher = ft_ostr_new();
	__des_cbc_small_cipher = ft_ostr_new();
	__des_ecb_large_cipher = ft_ostr_new();
	__des_cbc_large_cipher = ft_ostr_new();

	if (SSL_OK != file_read_all(__small_text_file_path, __small_text)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__des_ecb_small_cipher_file_path, __des_ecb_small_cipher)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__des_cbc_small_cipher_file_path, __des_cbc_small_cipher)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__large_text_file_path, __large_text)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__des_ecb_large_cipher_file_path, __des_ecb_large_cipher)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__des_cbc_large_cipher_file_path, __des_cbc_large_cipher)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	ft_hex_to_bytes(__key, __keyhex, 16);
	ft_hex_to_bytes(__iv, __ivhex, 16);

	return (SSL_OK);
}

static void	__test_des_cleanup(void)
{
	ft_ostr_del(__small_text);
	ft_ostr_del(__large_text);
	ft_ostr_del(__des_ecb_small_cipher);
	ft_ostr_del(__des_cbc_small_cipher);
	ft_ostr_del(__des_ecb_large_cipher);
	ft_ostr_del(__des_cbc_large_cipher);
}

static int	__test_des_ecb_encrypt(void)
{
	t_des_ctx		des;
	t_ostring	cipher;
	ssize_t		nbytes;

	// Small text file.
	des_init(&des, __key, NULL, DES_CRYPT_ECB, DES_MODE_ENCRYPT);
	ft_ostr_init(&cipher);

	nbytes = __helper_des_update(&des, __small_text, &cipher);
	TEST_ASSERT(nbytes > 0);

	nbytes = __helper_des_final(&des, &cipher);
	TEST_ASSERT(nbytes > 0);
	TEST_ASSERT(cipher.size == __des_ecb_small_cipher->size);
	TEST_ASSERT(ft_memcmp(cipher.content, __des_ecb_small_cipher->content, cipher.size) == 0);

	// Large text file.
	des_init(&des, __key, NULL, DES_CRYPT_ECB, DES_MODE_ENCRYPT);
	ft_ostr_init(&cipher);

	nbytes = __helper_des_update(&des, __large_text, &cipher);
	TEST_ASSERT(nbytes > 0);

	nbytes = __helper_des_final(&des, &cipher);
	TEST_ASSERT(nbytes > 0);
	TEST_ASSERT(cipher.size == __des_ecb_large_cipher->size);
	TEST_ASSERT(ft_memcmp(cipher.content, __des_ecb_large_cipher->content, cipher.size) == 0);
	ft_ostr_clear(&cipher);

	TEST_PASS();
}

static int	__test_des_ecb_decrypt(void)
{
	t_des_ctx		des;
	t_ostring	mes;
	ssize_t		nbytes;

	// Small cipher file.
	des_init(&des, __key, NULL, DES_CRYPT_ECB, DES_MODE_DECRYPT);
	ft_ostr_init(&mes);

	nbytes = __helper_des_update(&des, __des_ecb_small_cipher, &mes);
	TEST_ASSERT(nbytes > 0);

	nbytes = __helper_des_final(&des, &mes);
	TEST_ASSERT(nbytes >= 0);
	TEST_ASSERT(mes.size == __small_text->size);
	TEST_ASSERT(ft_memcmp(mes.content, __small_text->content, mes.size) == 0);

	// Large cipher file.
	des_init(&des, __key, NULL, DES_CRYPT_ECB, DES_MODE_DECRYPT);
	ft_ostr_init(&mes);

	nbytes = __helper_des_update(&des, __des_ecb_large_cipher, &mes);
	TEST_ASSERT(nbytes > 0);

	nbytes = __helper_des_final(&des, &mes);
	TEST_ASSERT(nbytes >= 0);
	TEST_ASSERT(mes.size == __large_text->size);
	TEST_ASSERT(ft_memcmp(mes.content, __large_text->content, mes.size) == 0);
	ft_ostr_clear(&mes);

	TEST_PASS();
}

static int	__test_des_cbc_encrypt(void)
{
	t_des_ctx		des;
	t_ostring	cipher;
	ssize_t		nbytes;

	// Small text file.
	des_init(&des, __key, __iv, DES_CRYPT_CBC, DES_MODE_ENCRYPT);
	ft_ostr_init(&cipher);

	nbytes = __helper_des_update(&des, __small_text, &cipher);
	TEST_ASSERT(nbytes > 0);

	nbytes = __helper_des_final(&des, &cipher);
	TEST_ASSERT(nbytes > 0);
	TEST_ASSERT(cipher.size == __des_cbc_small_cipher->size);
	TEST_ASSERT(ft_memcmp(cipher.content, __des_cbc_small_cipher->content, cipher.size) == 0);

	// Large text file.
	des_init(&des, __key, __iv, DES_CRYPT_CBC, DES_MODE_ENCRYPT);
	ft_ostr_init(&cipher);

	nbytes = __helper_des_update(&des, __large_text, &cipher);
	TEST_ASSERT(nbytes > 0);

	nbytes = __helper_des_final(&des, &cipher);
	TEST_ASSERT(nbytes > 0);
	TEST_ASSERT(cipher.size == __des_cbc_large_cipher->size);
	TEST_ASSERT(ft_memcmp(cipher.content, __des_cbc_large_cipher->content, cipher.size) == 0);
	ft_ostr_clear(&cipher);

	TEST_PASS();
}

static int	__test_des_cbc_decrypt(void)
{
	t_des_ctx		des;
	t_ostring	mes;
	ssize_t		nbytes;

	// Small cipher file.
	des_init(&des, __key, __iv, DES_CRYPT_CBC, DES_MODE_DECRYPT);
	ft_ostr_init(&mes);

	nbytes = __helper_des_update(&des, __des_cbc_small_cipher, &mes);
	TEST_ASSERT(nbytes > 0);

	nbytes = __helper_des_final(&des, &mes);
	TEST_ASSERT(nbytes >= 0);
	TEST_ASSERT(mes.size == __small_text->size);
	TEST_ASSERT(ft_memcmp(mes.content, __small_text->content, mes.size) == 0);

	// Large cipher file.
	des_init(&des, __key, __iv, DES_CRYPT_CBC, DES_MODE_DECRYPT);
	ft_ostr_init(&mes);

	nbytes = __helper_des_update(&des, __des_cbc_large_cipher, &mes);
	TEST_ASSERT(nbytes > 0);

	nbytes = __helper_des_final(&des, &mes);
	TEST_ASSERT(nbytes >= 0);

	TEST_ASSERT(mes.size == __large_text->size);
	TEST_ASSERT(ft_memcmp(mes.content, __large_text->content, mes.size) == 0);
	ft_ostr_clear(&mes);

	TEST_PASS();
}


static ssize_t	__helper_des_update(t_des_ctx *des, const t_ostring *in, t_ostring *out)
{
	char	*buf;
	ssize_t wbytes;

	SSL_ALLOC(buf, in->size);
	wbytes = des_update(des, (char *)in->content, buf, in->size);
	if (wbytes < 0) {
		SSL_FREE(buf);
		return (-1);
	}
	ft_ostr_append(out, buf, wbytes);
	SSL_FREE(buf);

	return (wbytes);
}

static ssize_t	__helper_des_final(t_des_ctx *des, t_ostring *out)
{
	char	buf[DES_BLOCK_SIZE];
	ssize_t wbytes;

	wbytes = des_final(des, (char *)buf, sizeof(buf));
	if (wbytes < 0) {
		return (-1);
	}
	ft_ostr_append(out, buf, wbytes);

	return (wbytes);
}
