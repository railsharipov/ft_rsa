#include <ssl/ssl.h>
#include <ssl/base64.h>
#include <test/test.h>

static const char	*__binary_file_path = "test/testfiles/file";
static const char	*__base64_file_path = "test/testfiles/base64_message.txt";

static t_ostring	__binary;
static t_ostring	__base64;

static int	__test_base64_setup(void);
static int	__test_base64_encode(void);
static int	__test_base64_decode(void);
static int	__test_base64_check(void);

int	test_base64(void)
{
	int	res;

	if (SSL_OK != __test_base64_setup())
		return (TEST_ERROR(UNSPECIFIED_ERROR));

	res = __test_base64_encode();
	res |= __test_base64_decode();
	res |= __test_base64_check();

	return (res);
}

static int	__test_base64_setup(void)
{
	if (SSL_OK != test_get_file_content(__binary_file_path, &__binary))
		return (TEST_ERROR(UNSPECIFIED_ERROR));
	if (SSL_OK != test_get_file_content(__base64_file_path, &__base64))
		return (TEST_ERROR(UNSPECIFIED_ERROR));
	if (__binary.size == 0 || __base64.size == 0)
		return (TEST_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__test_base64_encode(void)
{
	char	*output;
	size_t	outsize;
	int		ret_val;
	int		pass;

	ret_val = base64_encode(__binary.content, __binary.size, &output, &outsize);

	pass = TEST_ASSERT(SSL_OK == ret_val);
	pass |= TEST_ASSERT(NULL != output);
	pass |= TEST_ASSERT(outsize == __base64.size);
	pass |= TEST_ASSERT(!ft_memcmp(output, __base64.content, outsize));

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_base64_decode(void)
{
	char	*output;
	size_t	outsize;
	int		ret_val;
	int		pass;

	ret_val = base64_decode(__base64.content, __base64.size, &output, &outsize);

	pass = TEST_ASSERT(SSL_OK == ret_val);
	pass |= TEST_ASSERT(NULL != output);
	pass |= TEST_ASSERT(outsize == __binary.size);
	pass |= TEST_ASSERT(!ft_memcmp(output, __binary.content, outsize));

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_base64_check(void)
{
	const char	inval_chars[] = "#$%^&@";
	t_ostring	inval_b64;
	int			ret_val;
	int			pass;

	inval_b64.size = __base64.size + sizeof(inval_chars);
	SSL_ALLOC(inval_b64.content, inval_b64.size);

	ft_memcpy(inval_b64.content, __base64.content, __base64.size);
	ft_memcpy(inval_b64.content, inval_chars, sizeof(inval_chars));

	pass = TEST_ASSERT(SSL_OK == base64_check(__base64.content, __base64.size));
	pass |= TEST_ASSERT(SSL_OK != base64_check(inval_b64.content, inval_b64.size));

	SSL_FREE(inval_b64.content);

	if (SSL_TRUE != pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}
