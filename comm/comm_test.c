#include <ft_ssl.h>
#include <ssl_test.h>

typedef int	(*FUNC_TEST)(void);

static const FUNC_TEST	__TESTS[] = {
	test_bnum,
	test_base64,
	test_der,
};

static const int	__NUM_TESTS = sizeof(__TESTS)/sizeof(FUNC_TEST);

int	comm_test(const char **opt, const char *name)
{
	FUNC_TEST	f_test;
	int			ret[__NUM_TESTS];
	int			idx;

	(void)opt;
	(void)name;

	ft_bzero(ret, sizeof(ret));
	idx = 0;

	while (idx < __NUM_TESTS)
	{
		TEST_INFO(idx);

		f_test = __TESTS[idx];
		ret[idx] = f_test();

		if (IS_ERROR(ret[idx]))
			return SSL_ERROR(UNSPECIFIED_ERROR);

		idx++;
	}

	TEST_SUMMARY(ret, __NUM_TESTS);

	return (SSL_OK);
}
