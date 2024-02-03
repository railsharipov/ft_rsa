#include <ssl/ssl.h>
#include <test/test.h>

int	comm_test(const char **opt, const char *name)
{
	FUNC_TEST	f_test;
	int			ret[NUM_TESTS];
	int			idx;

	(void)opt;
	(void)name;

	ft_bzero(ret, sizeof(ret));
	idx = 0;

	while (idx < NUM_TESTS) {
		TEST_INFO(idx);

		f_test = TESTS[idx];

		if (NULL == f_test) {
			return (TEST_ERROR(UNSPECIFIED_ERROR));
		}
		ret[idx] = f_test();

		if (SSL_OK != ret[idx]) {
			return (TEST_ERROR(UNSPECIFIED_ERROR));
		}
		idx++;
	}

	TEST_SUMMARY(ret, NUM_TESTS);

	return (SSL_OK);
}
