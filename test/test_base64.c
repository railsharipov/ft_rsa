#include <ft_ssl.h>
#include <ssl_base64.h>
#include <ssl_test.h>

static const char	*__binary_file_path = "test/testfiles/file";
static const char	*__base64_file_path = "test/testfiles/base64_message.txt";

static t_ostring	__input;
static t_ostring	__ref;

static int	__test_base64_setup(void);
static int	__test_base64_encode(void);

int	test_base64(void)
{
	int	res;

	if (SSL_OK != __test_base64_setup())
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	res = __test_base64_encode();


	return (SSL_OK);
}

static int	__test_base64_setup(void)
{
	if (SSL_OK != test_get_testfile_content(__binary_file_path, &__input))
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	if (SSL_OK != test_get_testfile_content(__base64_file_path, &__ref))
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__test_base64_encode(void)
{
	char		*output;
	size_t		outsize;
	int			ret_val;
	int			pass;

	ret_val = base64_encode(__input.content, __input.size, &output, &outsize);

	pass = TEST_ASSERT(SSL_OK == ret_val);
	pass &= TEST_ASSERT(NULL != output);
	pass &= TEST_ASSERT(outsize == __ref.size);
	pass &= TEST_ASSERT(!ft_memcmp(output, __ref.content, outsize));

	if (SSL_TRUE == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}
