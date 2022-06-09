#include <ft_ssl.h>
#include <ssl_test.h>

int	test_assert(
	int bool, const char *expr, const char *func, const char *file)
{
	if (bool)
		return (SSL_OK);

	if (NULL == func || NULL == file)
		return (SSL_FAIL);

	ft_printf("%@%s, %s: ", func, file);

	if (errno)
		perror(NULL);
	else
		ft_printf("%@" TXT_RED("ASSERT FAIL") " " TXT_YELL("(%s)\n"), expr);

	return (SSL_FAIL);
}
