#include <common.h>
#include <cli.h>
#include <test.h>
#include <pem.h>
#include <argp.h>

const struct {
	const char *name;
	FUNC_TEST f_test;
} TEST_MAP[] = {
	{"libft", test_libft},
	{"argparser", test_argp},
	{"textutil", test_textutil},
	{"io", test_io},
	{"bnum", test_bnum},
	{"json", test_json},
	{"base64", test_base64},
	{"hash", test_hash},
	{"des", test_des},
	{"der", test_der},
	{"pem", test_pem},
	{"asn", test_asn},
};

const int NUM_TESTS = sizeof(TEST_MAP) / sizeof(TEST_MAP[0]);

#define __CLI_TEST_LINE_BREAK	"----------------------------------------"

int	cli_test(const char **opt, const char *name)
{
	FUNC_TEST	f_test;
	int			ret[NUM_TESTS];
	int			idx;
	int			num_passed;

	const char 	*flag = *opt;
	const char 	*flag_val = *(opt+1);;

	(void)name;

	TEST_LOG_SET_LEVEL(ERROR);

	if (NULL != flag && NULL != flag_val && ft_strcmp(flag, "-v") == 0) {
		if (ft_strcmp(flag_val, "all") == 0) {
			TEST_LOG_SET_LEVEL(DEBUG);
		} else {
			TEST_LOG(ERROR, "invalid flag value: %s: %s", flag, flag_val);
			return (SSL_ERR);
		}
	}

	ft_bzero(ret, sizeof(ret));
	num_passed = 0;

	idx = 0;
	while (idx < NUM_TESTS) {
		if (idx < 0 || idx >= NUM_TESTS) {
			TEST_LOG(ERROR, INVALID_INPUT_ERROR);
			return (SSL_ERR);
		}
		CLI_LOG(INFO, TXT_CYAN("testing %s"), TEST_MAP[idx].name);

		f_test = TEST_MAP[idx].f_test;

		if (NULL == f_test) {
			CLI_LOG(ERROR, "unknown test");
			return (SSL_ERR);
		}
		ret[idx] = f_test();

		if (SSL_OK != ret[idx]) {
			CLI_LOG(ERROR, "test failed");
		} else {
			num_passed++;
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
			CLI_LOG(INFO, TXT_B_GREEN("PASS") TXT_CYAN(" %s"), TEST_MAP[idx].name);
		} else {
			CLI_LOG(INFO, TXT_B_RED("FAIL") TXT_CYAN(" %s"), TEST_MAP[idx].name);
		}
		idx++;
	}
	CLI_LOG(INFO, __CLI_TEST_LINE_BREAK);

	return (SSL_OK);
}
