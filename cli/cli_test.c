#include <common.h>
#include <cli.h>
#include <test.h>

int	cli_test(const char **opt, const char *name)
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
			CLI_LOG(ERROR, UNSPECIFIED_ERROR);
			return (SSL_ERR);
		}
		ret[idx] = f_test();

		if (SSL_OK != ret[idx]) {
			CLI_LOG(ERROR, "test failed");
			return (SSL_ERR);
		}
		idx++;
	}

	TEST_SUMMARY(ret, NUM_TESTS);

	return (SSL_OK);
}
