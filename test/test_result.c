#include <ft_ssl.h>
#include <ssl_test.h>

int	test_result(
	int res, int verbose, const char *func_name, const char *file_name)
{
	if (verbose)
	{
		if (SSL_OK == res)
			ft_printf("%@%s, %s: " TXT_GREEN("TEST OK\n"), file_name, func_name);
		else
			ft_printf("%@%s, %s: " TXT_RED("TEST FAIL\n"), file_name, func_name);
	}

	return (res);
}
