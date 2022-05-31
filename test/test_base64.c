#include <ft_ssl.h>
#include <ssl_base64.h>
#include <ssl_test.h>

static const char	*__binary_file_path = "test/testfiles/file";
static const char	*__base64_file_path = "test/testfiles/base64_message.txt";

static t_ostring	__input;
static t_ostring	__ref;

static void	__test_base64_setup(void);
static void	__test_base64_encode(void);

void	test_base64(void)
{
	__test_base64_setup();
	__test_base64_encode();
}

static void	__test_base64_setup(void)
{
	test_get_testfile_content(__binary_file_path, &__input);
	test_get_testfile_content(__base64_file_path, &__ref);
}

static void	__test_base64_encode(void)
{
	char		*output;
	size_t		outsize;
	int			ret;

	ret = base64_encode(__input.content, __input.size, &output, &outsize);

	assert(SSL_OK == ret);
	assert(NULL != output);
	assert(outsize == __ref.size);
	assert(!ft_memcmp(output, __ref.content, outsize));

	TEST_PASS();
}
