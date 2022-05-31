#include <ft_ssl.h>
#include <ssl_test.h>

int	comm_test(const char **opt, const char *name)
{
	(void)opt;
	(void)name;

	test_base64();

	return (SSL_OK);
}
