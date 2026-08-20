#include <common.h>
#include <logger.h>
#include <base64.h>
#include <des.h>
#include <hash.h>
#include <rsa.h>
#include <cmd.h>
#include <args.h>
#include <pem.h>
#include <libft.h>

#include "test.h"

#define __TEST_LINE_BREAK	"----------------------------------------"
#define __TEST_TYPE_PASS	SSL_OK
#define __TEST_TYPE_FAIL	SSL_ERR

typedef int (*t_func_test_cmd)(t_htbl *opts);

static int __app_log_writer(const char *mes) {
	return (ft_printf("%@...%s\n", mes));
}

static int __test_log_writer(const char *mes) {
	return (ft_printf("%@%s\n", mes));
}

static int __get_log_level_by_name(const char *name) {
	if (ft_streq(name, "debug")) {
		return (LIBFT_LOG_LEVEL_DEBUG);
	}
	else if (ft_streq(name, "trace")) {
		return (LIBFT_LOG_LEVEL_TRACE);
	}
	else if (ft_streq(name, "warn")) {
		return (LIBFT_LOG_LEVEL_WARN);
	}
	else if (ft_streq(name, "info")) {
		return (LIBFT_LOG_LEVEL_INFO);
	}
	else if (ft_streq(name, "error")) {
		return (LIBFT_LOG_LEVEL_ERROR);
	}
	else if (ft_streq(name, "crit")) {
		return (LIBFT_LOG_LEVEL_CRIT);
	}
	else {
		TEST_LOG(ERROR, "invalid log level: %s", name);
		TEST_LOG(WARN, "falling back to INFO log level");
		return (LIBFT_LOG_LEVEL_INFO);
	}
}

typedef struct __s_test {
	const char *name;
	t_func_test f_test;
} __t_test;

static const __t_test __TESTS[] = {
	{ "libft",		test_libft	},
	{ "textutil",	test_textutil	},
	{ "io",			test_io		},
	{ "bnum",		test_bnum	},
	{ "json",		test_json	},
	{ "base64",		test_base64	},
	{ "hash",		test_hash	},
	{ "des",		test_des	},
	{ "der",		test_der	},
	{ "pem",		test_pem	},
	{ "asn",		test_asn	},
	{ "args",		test_args	},
};
const int __NUM_TESTS = sizeof(__TESTS)/sizeof(__TESTS[0]);

static int __cmd_run_tests(t_htbl *opts)
{
	// configure loggers
	t_logger *__app_logger = logger_get_logger();
	*__app_logger = (t_logger){
		.log_writer = __app_log_writer,
		.log_level_thres = LIBFT_LOG_LEVEL_CRIT,
		.debug_info_thres = LIBFT_LOG_LEVEL_TRACE,
		.is_ansi_colored = 0,
	};

	t_logger *__libft_logger = libft_get_logger();
	*__libft_logger = (t_logger){
		.log_writer = __app_log_writer,
		.log_level_thres = LIBFT_LOG_LEVEL_CRIT,
		.debug_info_thres = LIBFT_LOG_LEVEL_TRACE,
		.is_ansi_colored = 0,
	};

	t_logger *__test_logger = test_get_logger();
	*__test_logger = (t_logger){
		.log_writer = __test_log_writer,
		.log_level_thres = LIBFT_LOG_LEVEL_INFO,
		.debug_info_thres = LIBFT_LOG_LEVEL_TRACE,
		.is_ansi_colored = 1,
	};
	// set app log level
	if (ft_htbl_has(opts, "--app-logs")) {
		const char *log_level_name = ft_htbl_get(opts, "--app-logs");
		uint8_t log_level_thres = __get_log_level_by_name(log_level_name);
		__app_logger->log_level_thres = log_level_thres;
		__libft_logger->log_level_thres = log_level_thres;
	}
	// set test log level
	if (ft_htbl_has(opts, "--test-logs")) {
		const char *log_level_name = ft_htbl_get(opts, "--test-logs");
		uint8_t log_level_thres = __get_log_level_by_name(log_level_name);
		__test_logger->log_level_thres = log_level_thres;
	}
	// define a list of tests to run
	t_node *test_list = NULL;
	if (ft_htbl_has(opts, "--tests")) {
		const char *test_names = ft_htbl_get(opts, "--tests");
		char **test_names_list = ft_strsplit(test_names, ',');

		while (*test_names_list != NULL) {
			t_func_test f_test = NULL;
			for (int i = 0; i < __NUM_TESTS; i++) {
				__t_test test = __TESTS[i];
				if (ft_streq(test.name, *test_names_list)) {
					f_test = test.f_test;
				}
			}
			if (NULL == f_test) {
				TEST_LOG(ERROR, "unknown test: %s", *test_names_list);
				return (1);
			}
			ft_lst_append(&test_list, ft_node_new(*test_names_list, f_test, sizeof(t_func_test)));
			test_names_list++;
		}
	}
	else {
		for (int i = 0; i < __NUM_TESTS; i++) {
			__t_test test = __TESTS[i];
			ft_lst_append(&test_list, ft_node_new(test.name, test.f_test, sizeof(t_func_test)));
		}
	}
	// run tests
	int num_tests = ft_lst_size(test_list);
	int num_passed = 0;
	for (t_node *test_node = test_list; test_node != NULL; test_node = test_node->next) {
		TEST_LOG(ALWAYS, TXT_CYAN("testing %s"), test_node->key);
		t_func_test f_test = test_node->content;

		if (SSL_OK == f_test()) {
			test_node->type = __TEST_TYPE_PASS;
			num_passed++;
		} else {
			test_node->type = __TEST_TYPE_FAIL;
		}
	}
	// print summary
	if (!ft_htbl_has(opts, "--no-summary")) {
		TEST_LOG(ALWAYS, __TEST_LINE_BREAK);

		if (num_passed == num_tests) {
			TEST_LOG(ALWAYS, "SUMMARY: " TXT_B_GREEN("ALL PASS"));
		} else {
			TEST_LOG(ALWAYS, "SUMMARY: " TXT_B_RED("FAIL"));
		}
		TEST_LOG(ALWAYS, "tested: %d, pass: %d, fail: %d", num_tests, num_passed, num_tests - num_passed);
		TEST_LOG(ALWAYS, __TEST_LINE_BREAK);

		for (t_node *test_node = test_list; test_node != NULL; test_node = test_node->next) {
			if (SSL_OK == test_node->type) {
				TEST_LOG(ALWAYS, TXT_B_GREEN("PASS") TXT_CYAN(" %s"), test_node->key);
			} else {
				TEST_LOG(ALWAYS, TXT_B_RED("FAIL") TXT_CYAN(" %s"), test_node->key);
			}
		}
		TEST_LOG(ALWAYS, __TEST_LINE_BREAK);
	}
	return (0);
}

static int __cmd_list_tests(t_htbl *opts)
{
	t_logger *__test_logger = test_get_logger();
	*__test_logger = (t_logger){
		.log_writer = __test_log_writer,
		.log_level_thres = LIBFT_LOG_LEVEL_INFO,
		.debug_info_thres = LIBFT_LOG_LEVEL_WARN,
		.is_ansi_colored = 1,
	};

	TEST_LOG(ALWAYS, __TEST_LINE_BREAK);

	for (int i = 0; i < __NUM_TESTS; i++) {
		__t_test test = __TESTS[i];
		TEST_LOG(ALWAYS, "%s", test.name);
	}
	TEST_LOG(ALWAYS, __TEST_LINE_BREAK);
	return (0);
}

int	main(int ac, const char **av)
{
	// Register tests
	t_htbl *tests_tbl = ft_htbl_create(LIBFT_HT_SIZE);
	ft_htbl_add(tests_tbl, test_libft, "libft");
	ft_htbl_add(tests_tbl, test_textutil, "textutil");
	ft_htbl_add(tests_tbl, test_io, "io");
	ft_htbl_add(tests_tbl, test_bnum, "bnum");
	ft_htbl_add(tests_tbl, test_json, "json");
	ft_htbl_add(tests_tbl, test_base64, "base64");
	ft_htbl_add(tests_tbl, test_hash, "hash");
	ft_htbl_add(tests_tbl, test_des, "des");
	ft_htbl_add(tests_tbl, test_der, "der");
	ft_htbl_add(tests_tbl, test_pem, "pem");
	ft_htbl_add(tests_tbl, test_asn, "asn");
	ft_htbl_add(tests_tbl, test_args, "args");

	// Define command args
	t_arg_cmd *run_cmd_arg = args_new_cmd("run", "run tests", __cmd_run_tests);
	args_add_cmd_opt(run_cmd_arg, args_new_opt("--app-logs", "set application logs level, default=crit", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(run_cmd_arg, args_new_opt("--test-logs", "set test logs level, default=info", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(run_cmd_arg, args_new_opt("--tests", "comma-delimitied list of tests to run", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(run_cmd_arg, args_new_opt("--no-summary", "do not print summary", AP_OPT_TYPE_FLAG));

	t_arg_cmd *list_cmd_arg = args_new_cmd("list", "list available tests", __cmd_list_tests);

	t_arg_cmd *main_cmd_arg = args_new_cmd(av[0], "Tests", NULL);
	args_add_sub_cmd(main_cmd_arg, run_cmd_arg);
	args_add_sub_cmd(main_cmd_arg, list_cmd_arg);

	// Parse command args
	t_cmd cmd = {0};
	if (SSL_OK != args_parse(&cmd, main_cmd_arg, av, ac)) {
		TEST_LOG(ERROR, INVALID_INPUT_ERROR);
		if (cmd.arg_ref != NULL) {
			args_dump_help(cmd.arg_ref);
		} else {
			args_dump_help(main_cmd_arg);
		}
		return (1);
	}

	// Run command
	if (ft_htbl_has(cmd.opts, "-h")) {
		args_dump_help(cmd.arg_ref);
		return (0);
	}
	t_func_test_cmd f_cmd = cmd.func;
	if (f_cmd != NULL) {
		return (f_cmd(cmd.opts));
	} else {
		args_dump_help(cmd.arg_ref);
		return (0);
	}
}
