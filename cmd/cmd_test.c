#include <common.h>
#include <args.h>
#include <cmd.h>
#include <test.h>
#include <pem.h>

#define __CLI_TEST_LINE_BREAK	"----------------------------------------"

t_logger *__extern_test_logger = NULL;

int	cmd_test(const t_cmd *cmd)
{
	const struct {
		const char *name;
		t_func_test f_test;
	} __TESTS[] = {
		{"libft", test_libft},
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
		{"args", test_args},
	};
	const int NUM_TESTS = sizeof(__TESTS) / sizeof(__TESTS[0]);

	t_func_test	f_test;
	int			ret[NUM_TESTS];
	int			idx, num_passed;
	t_logger	logger;

	logger = *ft_logger_get_default_logger();
	logger.log_level = LIBFT_LOG_LEVEL_INFO;
	logger.debug_info_thres = LIBFT_LOG_LEVEL_WARN;
	__extern_test_logger = &logger;

	ft_bzero(ret, sizeof(ret));
	num_passed = 0;

	idx = 0;
	while (idx < NUM_TESTS) {
		if (idx < 0 || idx >= NUM_TESTS) {
			TEST_LOG(ERROR, INVALID_INPUT_ERROR);
			return (SSL_ERR);
		}
		TEST_LOG(INFO, TXT_CYAN("testing %s"), __TESTS[idx].name);

		f_test = __TESTS[idx].f_test;

		if (NULL == f_test) {
			TEST_LOG(ERROR, "unknown test");
			return (SSL_ERR);
		}

		__extern_test_logger = ft_logger_get_default_logger();
		ret[idx] = f_test();
		__extern_test_logger = &logger;

		if (SSL_OK != ret[idx]) {
			TEST_LOG(ERROR, "test failed");
		} else {
			num_passed++;
		}
		idx++;
	}

	TEST_LOG(INFO, __CLI_TEST_LINE_BREAK);

	if (num_passed == NUM_TESTS) {
		TEST_LOG(INFO, "SUMMARY: " TXT_B_GREEN("ALL PASS"));
	} else {
		TEST_LOG(INFO, "SUMMARY: " TXT_B_RED("FAIL"));
	}

	TEST_LOG(INFO, "tested: %d, pass: %d, fail: %d", NUM_TESTS, num_passed, NUM_TESTS - num_passed);
	TEST_LOG(INFO, __CLI_TEST_LINE_BREAK);

	idx = 0;
	while (idx < NUM_TESTS) {
		if (SSL_OK == ret[idx]) {
			num_passed++;
			TEST_LOG(INFO, TXT_B_GREEN("PASS") TXT_CYAN(" %s"), __TESTS[idx].name);
		} else {
			TEST_LOG(INFO, TXT_B_RED("FAIL") TXT_CYAN(" %s"), __TESTS[idx].name);
		}
		idx++;
	}
	TEST_LOG(INFO, __CLI_TEST_LINE_BREAK);

	return (SSL_OK);
}
