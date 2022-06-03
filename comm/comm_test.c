#include <ft_ssl.h>
#include <ssl_test.h>

int	comm_test(const char **opt, const char *name)
{
	int	ret[NUMBER_OF_TEST_MODULES];
	int	idx;

	(void)opt;
	(void)name;

	ft_bzero(ret, sizeof(ret));
	idx = 0;

	ret[idx++] = test_base64();

	return (SSL_OK);
}
