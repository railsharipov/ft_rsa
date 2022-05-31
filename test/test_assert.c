#include <ft_ssl.h>
#include <ssl_test.h>

void	test_assert(
	int bool, const char *expr, const char *func, const char *file)
{
	if (bool)
		return ;

	ft_printf("%@%s, %s: ", func, file);

	if (errno)
		perror(NULL);
	else
		ft_printf("%@assertion failed (%s)\n", expr);

	exit(0);
}
