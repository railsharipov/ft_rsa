#include <common.h>
#include <base64.h>
#include <test.h>

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
	if (SSL_OK != __test_base64_setup()) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		TEST_FAIL();
	}

	return (
		__test_base64_encode()
		| __test_base64_decode()
		| __test_base64_check()
	);
}

static int	__test_base64_setup(void)
{
	if (SSL_OK != test_get_file_content(__binary_file_path, &__binary)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__base64_file_path, &__base64)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	if (__binary.size == 0 || __base64.size == 0) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}

	return (SSL_OK);
}

static int	__test_base64_encode(void)
{
	char	*output;
	size_t	outsize;
	int		ret_val;

	ret_val = base64_encode(__binary.content, __binary.size, &output, &outsize);

	TEST_ASSERT(SSL_OK == ret_val);
	TEST_ASSERT(NULL != output);
	TEST_ASSERT(outsize == __base64.size);
	TEST_ASSERT(!ft_memcmp(output, __base64.content, outsize));

	TEST_PASS();
}

static int	__test_base64_decode(void)
{
	char	*output;
	size_t	outsize;
	int		ret_val;

	ret_val = base64_decode(__base64.content, __base64.size, &output, &outsize);

	TEST_ASSERT(SSL_OK == ret_val);
	TEST_ASSERT(NULL != output);
	TEST_ASSERT(outsize == __binary.size);
	TEST_ASSERT(!ft_memcmp(output, __binary.content, outsize));

	TEST_PASS();
}

static int	__test_base64_check(void)
{
	const char	inval_chars[] = "#$%^&@";
	t_ostring	inval_b64;
	int			ret_val;

	inval_b64.size = __base64.size + sizeof(inval_chars);
	SSL_ALLOC(inval_b64.content, inval_b64.size);

	ft_memcpy(inval_b64.content, __base64.content, __base64.size);
	ft_memcpy(inval_b64.content, inval_chars, sizeof(inval_chars));

	TEST_ASSERT(SSL_OK == base64_check(__base64.content, __base64.size));
	TEST_ASSERT(SSL_OK != base64_check(inval_b64.content, inval_b64.size));

	SSL_FREE(inval_b64.content);

	TEST_PASS();
}
