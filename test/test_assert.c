#include <ssl/ssl.h>
#include <test/test.h>

int	test_assert(int bool, const char *func, const char *file, int line)
{
	if (bool)
		return (SSL_OK);

	if (NULL == func || NULL == file)
		return (SSL_FAIL);

	ft_printf("%@%s, %s:%d: ", func, file, line);

	if (errno)
		perror(NULL);
	else
		ft_printf("%@" TXT_RED("ASSERT FAIL\n"));

	return (SSL_FAIL);
}
