#include <ft_ssl.h>
#include <ssl_test.h>

int	test_result(
	int res, int verbose, const char *func_name, const char *file_name)
{
	if (verbose)
	{
		if (SSL_OK == res)
			ft_printf("%@%s: " TXT_GREEN("TEST OK\n"), func_name);
		else
			ft_printf("%@%s: " TXT_RED("TEST FAIL\n"), func_name);
	}
	(void)file_name;

	return (res);
}
