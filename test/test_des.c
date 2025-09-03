#include <common.h>
#include <des.h>
#include <test.h>
#include <rand.h>
#include <io.h>
#include <libft/bytes.h>

static int	__test_des_setup(void);
static void	__test_des_cleanup(void);

static int	__test_des_ecb_encrypt(void);
static int	__test_des_ecb_decrypt(void);
static int	__test_des_cbc_encrypt(void);
static int	__test_des_cbc_decrypt(void);

static const char	*__small_text_file_path = "test/files/text/small.txt";
// static const char	*__large_text_file_path = "test/files/text/large.txt";

static const char	*__des_ecb_small_cipher_file_path = "test/files/des/des-ecb-small-cipher";
// static const char	*__des_ecb_large_cipher_file_path = "test/files/des/des-ecb-large-cipher";
static const char	*__des_cbc_small_cipher_file_path = "test/files/des/des-cbc-small-cipher";
// static const char	*__des_cbc_large_cipher_file_path = "test/files/des/des-cbc-large-cipher";

static t_ostring	__small_text;
static t_ostring	__des_ecb_small_cipher;
// static t_ostring	__des_ecb_large_cipher;
static t_ostring	__des_cbc_small_cipher;

static const char	*__keyhex = "F122D9DEFE7F91F6";
static const char	*__ivhex = "6F19F7EFED9D221F";

static uint8_t	__key[8];
static uint8_t	__iv[8];

int	test_des(void)
{
	int ret;

	if (SSL_OK != __test_des_setup()) {
		TEST_LOG(ERROR, "failed to setup test");
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
	if (SSL_OK != test_read_file(__small_text_file_path, &__small_text)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_read_file(__des_ecb_small_cipher_file_path, &__des_ecb_small_cipher)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_read_file(__des_cbc_small_cipher_file_path, &__des_cbc_small_cipher)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	ft_hex_to_bytes(__key, __keyhex, 16);
	ft_hex_to_bytes(__iv, __ivhex, 16);

	return (SSL_OK);
}

static void	__test_des_cleanup(void)
{
	return ;
}

static int	__test_des_ecb_encrypt(void)
{
	t_des		des;
	t_iodes		in, out;
	t_ostring	cipher;
	int			ret;

	des_init(&des, __key, NULL, DES_CRYPT_ECB, DES_MODE_ENCRYPT);
	ft_ostr_init(&cipher);
	
	if (SSL_OK != io_fopen(&in, IO_READ|IO_FILE, __small_text_file_path)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != io_osbuf(&out, IO_WRITE, &cipher)) {
		TEST_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}
	ret = des_update(&des, &in, &out);
	io_fclose(&in);
	TEST_ASSERT(ret == SSL_OK);

	ret = des_final(&des, &out);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(cipher.size == __des_ecb_small_cipher.size);
	TEST_ASSERT(ft_memcmp(cipher.content, __des_ecb_small_cipher.content, cipher.size) == 0);

	TEST_PASS();
}

static int	__test_des_ecb_decrypt(void)
{
	t_des		des;
	t_iodes		in, out;
	t_ostring	mes;
	int			ret;

	ft_ostr_init(&mes);
	des_init(&des, __key, NULL, DES_CRYPT_ECB, DES_MODE_DECRYPT);
	if (SSL_OK != io_fopen(&in, IO_READ|IO_FILE, __des_ecb_small_cipher_file_path)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != io_osbuf(&out, IO_WRITE, &mes)) {
		TEST_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}
	ret = des_update(&des, &in, &out);
	io_fclose(&in);
	TEST_ASSERT(ret == SSL_OK);

	ret = des_final(&des, &out);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(mes.size == __small_text.size);
	TEST_ASSERT(ft_memcmp(mes.content, __small_text.content, mes.size) == 0);

	TEST_PASS();
}

static int	__test_des_cbc_encrypt(void)
{
	t_des		des;
	t_iodes		in, out;
	t_ostring	cipher;
	int			ret;

	ft_ostr_init(&cipher);
	des_init(&des, __key, __iv, DES_CRYPT_CBC, DES_MODE_ENCRYPT);
	if (SSL_OK != io_fopen(&in, IO_READ|IO_FILE, __small_text_file_path)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != io_osbuf(&out, IO_WRITE, &cipher)) {
		TEST_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}
	ret = des_update(&des, &in, &out);
	io_fclose(&in);
	TEST_ASSERT(ret == SSL_OK);

	ret = des_final(&des, &out);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(cipher.size == __des_cbc_small_cipher.size);
	TEST_ASSERT(ft_memcmp(cipher.content, __des_cbc_small_cipher.content, cipher.size) == 0);

	TEST_PASS();
}

static int	__test_des_cbc_decrypt(void)
{
	t_des		des;
	t_iodes		in, out;
	t_ostring	mes;
	int			ret;

	ft_ostr_init(&mes);
	des_init(&des, __key, __iv, DES_CRYPT_CBC, DES_MODE_DECRYPT);
	if (SSL_OK != io_fopen(&in, IO_READ|IO_FILE, __des_cbc_small_cipher_file_path)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != io_osbuf(&out, IO_WRITE, &mes)) {
		TEST_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}
	ret = des_update(&des, &in, &out);
	io_fclose(&in);
	TEST_ASSERT(ret == SSL_OK);

	ret = des_final(&des, &out);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(mes.size == __small_text.size);
	TEST_ASSERT(ft_memcmp(mes.content, __small_text.content, mes.size) == 0);

	TEST_PASS();
}