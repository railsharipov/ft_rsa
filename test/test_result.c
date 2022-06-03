#include <ft_ssl.h>
#include <ssl_test.h>

int	test_result(
	int pass, int verbose, const char *func_name, const char *file_name)
{
	if (verbose)
	{
		if (pass)
			ft_printf("%@%s, %s: " TXT_GREEN("TEST OK\n"), func_name, file_name);
		else
			ft_printf("%@%s, %s: " TXT_RED("TEST FAIL\n"), func_name, file_name);
	}

	return (pass);
}
