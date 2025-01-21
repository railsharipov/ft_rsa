#include <common.h>
#include <test.h>

int	test_assert(int bool, const char *expr, const char *func, const char *file, int line)
{
	if (NULL == func) {
		return (SSL_ERR);
	}
	if (bool) {
#ifdef TEST_ENABLE_ASSERT_PASS_LOG
		ft_printf("%@%s, %s:%d: " TXT_GREEN("ASSERT PASS") " (%s)\n", func, file, line, expr);
#endif
		return (SSL_OK);
	} else {
		ft_printf("%@%s, %s:%d: ", func, file, line);

		if (errno) {
			perror(NULL);
		} else {
			ft_printf("%@" TXT_RED("ASSERT FAIL") " (%s)\n", expr);
		}
		return (SSL_ERR);
	}
}
