#include <common.h>
#include <cli.h>
#include <test.h>

#define __CLI_TEST_LINE_BREAK	"----------------------------------------"

int	cli_test(const char **opt, const char *name)
{
	FUNC_TEST	f_test;
	int			ret[NUM_TESTS];
	int			idx;
	int			num_passed;

	(void)opt;
	(void)name;

	ft_bzero(ret, sizeof(ret));
	num_passed = 0;

	idx = 0;
	while (idx < NUM_TESTS) {
		if (idx < 0 || idx >= TEST_DESC_ARR_SIZE) {
			TEST_LOG(ERROR, INVALID_INPUT_ERROR);
			return (SSL_ERR);
		}
		CLI_LOG(INFO, "TEST MODULE: " TXT_CYAN("%s"), TEST_DESC_ARR[idx]);

		f_test = TESTS[idx];

		if (NULL == f_test) {
			CLI_LOG(ERROR, "unknown test");
			return (SSL_ERR);
		}
		ret[idx] = f_test();

		if (SSL_OK != ret[idx]) {
			CLI_LOG(ERROR, "test failed");
		}
		idx++;
	}

	CLI_LOG(INFO, __CLI_TEST_LINE_BREAK);

	if (num_passed == NUM_TESTS) {
		CLI_LOG(INFO, "SUMMARY: " TXT_B_GREEN("ALL PASS"));
	} else {
		CLI_LOG(INFO, "SUMMARY: " TXT_B_RED("FAIL"));
	}

	CLI_LOG(INFO, "tested: %d, pass: %d, fail: %d", NUM_TESTS, num_passed, NUM_TESTS - num_passed);
	CLI_LOG(INFO, __CLI_TEST_LINE_BREAK);

	idx = 0;
	while (idx < NUM_TESTS) {
		if (SSL_OK == ret[idx]) {
			num_passed++;
			CLI_LOG(INFO, TXT_B_GREEN("PASS") TXT_CYAN(" %s"), TEST_DESC_ARR[idx]);
		} else {
			CLI_LOG(INFO, TXT_B_RED("FAIL") TXT_CYAN(" %s"), TEST_DESC_ARR[idx]);
		}
		idx++;
	}
	CLI_LOG(INFO, __CLI_TEST_LINE_BREAK);

	return (SSL_OK);
}
