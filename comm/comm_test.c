#include <ft_ssl.h>
#include <ssl_test.h>

int	comm_test(const char **opt, const char *name)
{
	FUNC_TEST	f_test;
	int			ret[NUM_TESTS];
	int			idx;

	(void)opt;
	(void)name;

	ft_bzero(ret, sizeof(ret));
	idx = 0;

	while (idx < NUM_TESTS)
	{
		TEST_INFO(idx);

		f_test = TESTS[idx];
		ret[idx] = f_test();

		if (IS_ERROR(ret[idx]))
			return SSL_ERROR(UNSPECIFIED_ERROR);

		idx++;
	}

	TEST_SUMMARY(ret, NUM_TESTS);

	return (SSL_OK);
}
