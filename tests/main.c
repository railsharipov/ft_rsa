#include <common.h>
#include <base64.h>
#include <des.h>
#include <hash.h>
#include <rsa.h>
#include <cmd.h>
#include <args.h>
#include <pem.h>
#include <libft/string.h>
#include <libft/list.h>
#include <libft/htable.h>

#include "test.h"

#define __TEST_LINE_BREAK	"----------------------------------------"

#define __TEST_TYPE_PASS	SSL_OK
#define __TEST_TYPE_FAIL	SSL_ERR

#define TEST_PRINT(FMT, ...)	ft_printf(TXT_B_GRAY("test: ") FMT "\n" __VA_OPT__(, ) __VA_ARGS__)

static int __app_log_writer(const char *mes) {
	return (ft_printf("%@    %s\n", mes));
}
static t_logger __app_logger = {
	.log_writer = __app_log_writer,
	.log_level_thres = LIBFT_LOG_LEVEL_CRIT,
	.is_ansi_colored = 0,
	.debug_info_thres = LIBFT_LOG_LEVEL_TRACE,
};

static int __test_log_writer(const char *mes) {
	return (ft_printf("%@%s\n", mes));
}
static t_logger __test_logger = {
	.log_writer = __test_log_writer,
	.log_level_thres = LIBFT_LOG_LEVEL_INFO,
	.debug_info_thres = LIBFT_LOG_LEVEL_TRACE,
	.is_ansi_colored = 1,
};
t_logger *__test_logger_extern = &__test_logger;

int	main(int ac, const char **av)
{
	t_arg_cmd	*cmd_arg;
	t_cmd		cmd;

	logger_set_default_logger(&__app_logger);

	cmd_arg = args_new_cmd(av[0], "Run tests", NULL);
	args_add_global_cmd_opt(cmd_arg, args_new_opt("--logs", "set application logs level, default=crit", AP_OPT_TYPE_STRING));
	args_add_global_cmd_opt(cmd_arg, args_new_opt("--tests", "comma-delimitied list of tests to run", AP_OPT_TYPE_STRING));
	args_add_global_cmd_opt(cmd_arg, args_new_opt("--no-summary", "do not print summary", AP_OPT_TYPE_FLAG));
	args_add_global_cmd_opt(cmd_arg, args_new_opt("-q", "log failed tests only", AP_OPT_TYPE_FLAG));

	if (SSL_OK != args_parse(&cmd, cmd_arg, av, ac)) {
		TEST_LOG(ERROR, INVALID_INPUT_ERROR);
		if (cmd.arg_ref != NULL) {
			args_dump_help(cmd.arg_ref);
		} else {
			args_dump_help(cmd_arg);
		}
		return (1);
	}

	// handle help
	if (ft_htbl_has(cmd.opts, "-h")) {
		args_dump_help(cmd.arg_ref);
		return (0);
	}

	// set log level
	if (ft_htbl_has(cmd.opts, "--logs")) {
		const char *log_level_name = ft_htbl_get(cmd.opts, "--logs");
		uint8_t log_level_thres;

		if (ft_streq(log_level_name, "debug")) {
			log_level_thres = LIBFT_LOG_LEVEL_DEBUG;
		}
		else if (ft_streq(log_level_name, "trace")) {
			log_level_thres = LIBFT_LOG_LEVEL_TRACE;
		}
		else if (ft_streq(log_level_name, "warn")) {
			log_level_thres = LIBFT_LOG_LEVEL_WARN;
		}
		else if (ft_streq(log_level_name, "info")) {
			log_level_thres = LIBFT_LOG_LEVEL_INFO;
		}
		else if (ft_streq(log_level_name, "error")) {
			log_level_thres = LIBFT_LOG_LEVEL_ERROR;
		}
		else {
			TEST_LOG(ERROR, "invalid log level: %s", log_level_name);
			return (1);
		}
		__app_logger.log_level_thres = log_level_thres;
	}

	if (ft_htbl_has(cmd.opts, "-q")) {
		__test_logger.log_level_thres = LIBFT_LOG_LEVEL_ERROR;
	}

	// register tests
	t_htbl	*tests_tbl = ft_htbl_create(LIBFT_HT_SIZE);

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

	// define a list of tests to run
	t_node	*tests_list = NULL;

	if (ft_htbl_has(cmd.opts, "--tests")) {
		const char *test_names = ft_htbl_get(cmd.opts, "--tests");
		char **test_names_list = ft_strsplit(test_names, ',');

		while (*test_names_list != NULL) {
			t_func_test f_test = ft_htbl_get(tests_tbl, *test_names_list);
			if (NULL == f_test) {
				TEST_LOG(ERROR, "unknown test: %s", *test_names_list);
				return (1);
			}
			ft_lst_append(&tests_list, ft_node_new(*test_names_list, f_test, sizeof(t_func_test)));
			test_names_list++;
		}
	}
	else {
		t_node *node = ft_htbl_node_next(tests_tbl, NULL);
		while (node != NULL) {
			ft_lst_append(&tests_list, ft_node_new(node->key, node->content, sizeof(t_func_test)));
			node = ft_htbl_node_next(tests_tbl, node);
		}
	}

	// run tests
	int num_tests = ft_lst_size(tests_list);
	int num_passed = 0;

	for (t_node *test_node = tests_list; test_node != NULL; test_node = test_node->next) {
		TEST_PRINT(TXT_CYAN("testing %s"), test_node->key);
		t_func_test f_test = test_node->content;

		if (SSL_OK == f_test()) {
			test_node->type = __TEST_TYPE_PASS;
			num_passed++;
		} else {
			test_node->type = __TEST_TYPE_FAIL;
		}
	}

	// print summary
	if (!ft_htbl_has(cmd.opts, "--no-summary")) {
		TEST_PRINT(__TEST_LINE_BREAK);

		if (num_passed == num_tests) {
			TEST_PRINT("SUMMARY: " TXT_B_GREEN("ALL PASS"));
		} else {
			TEST_PRINT("SUMMARY: " TXT_B_RED("FAIL"));
		}
		TEST_PRINT("tested: %d, pass: %d, fail: %d", num_tests, num_passed, num_tests - num_passed);
		TEST_PRINT(__TEST_LINE_BREAK);

		for (t_node *test_node = tests_list; test_node != NULL; test_node = test_node->next) {
			if (SSL_OK == test_node->type) {
				TEST_PRINT(TXT_B_GREEN("PASS") TXT_CYAN(" %s"), test_node->key);
			} else {
				TEST_PRINT(TXT_B_RED("FAIL") TXT_CYAN(" %s"), test_node->key);
			}
		}
		TEST_PRINT(__TEST_LINE_BREAK);
	}

	return (SSL_OK);
}
