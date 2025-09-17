#include <common.h>
#include <argp.h>
#include <test.h>

static int	__test_argp_setup(void);
static int	__test_argp_parse(void);

int	test_argp(void)
{
	if (SSL_OK != __test_argp_setup()) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		TEST_FAIL();
	}

	return (
		__test_argp_parse()
	);
}

static int	__test_argp_setup(void)
{
	const char *sargs[] = {
		"test",
		"-v",
		"all",
		"-h",
		"10",
		NULL
	};
	const char *sargs_2[] = {
		"test",
		"-v",
		"all",
		"-i",
		NULL
	};
	const t_ap_opt_def opt_def_list[] = {
		{"-v", AP_ARG_TYPE_STRING},
		{"-h", AP_ARG_TYPE_NUMBER},
		{"-i", AP_ARG_TYPE_FLAG},
	};
	const size_t opt_def_list_size = sizeof(opt_def_list) / sizeof(opt_def_list[0]);
	t_ap_parser parser;
	int ret;

	ret = argp_parser_init(&parser);
	TEST_ASSERT(ret == SSL_OK);
	ret = argp_add_cmd(&parser, "test", opt_def_list, opt_def_list_size, NULL);
	TEST_ASSERT(ret == SSL_OK);
	ret = argp_parse(&parser, sargs);
	TEST_ASSERT(ret == SSL_OK);

	t_ap_cmd *cmd;	
	cmd = argp_get_cmd(&parser, "test");
	TEST_ASSERT(cmd != NULL);
	TEST_ASSERT(ft_strcmp(cmd->name, "test") == 0);
	TEST_ASSERT(cmd->func == NULL);

	t_ap_opt *opt;
	opt = argp_get_opt(cmd, "-v");
	TEST_ASSERT(opt != NULL);
	TEST_ASSERT(opt->set == 1);
	TEST_ASSERT(opt->pos == 1);
	TEST_ASSERT(opt->type == AP_ARG_TYPE_STRING);
	TEST_ASSERT(ft_strcmp(opt->value, "all") == 0);

	opt = argp_get_opt(cmd, "-h");
	TEST_ASSERT(opt != NULL);
	TEST_ASSERT(opt->set == 1);
	TEST_ASSERT(opt->pos == 3);
	TEST_ASSERT(opt->type == AP_ARG_TYPE_NUMBER);
	TEST_ASSERT(ft_strcmp(opt->value, "10") == 0);

	opt = argp_get_opt(cmd, "-i");
	TEST_ASSERT(opt != NULL);
	TEST_ASSERT(opt->set == 0);
	TEST_ASSERT(opt->type == AP_ARG_TYPE_FLAG);

	ret = argp_parser_init(&parser);
	TEST_ASSERT(ret == SSL_OK);
	ret = argp_add_cmd(&parser, "test", opt_def_list, opt_def_list_size, NULL);
	TEST_ASSERT(ret == SSL_OK);
	ret = argp_parse(&parser, sargs_2);
	TEST_ASSERT(ret == SSL_OK);

	cmd = argp_get_cmd(&parser, "test");
	TEST_ASSERT(cmd != NULL);
	TEST_ASSERT(ft_strcmp(cmd->name, "test") == 0);
	TEST_ASSERT(cmd->func == NULL);

	opt = argp_get_opt(cmd, "-i");
	TEST_ASSERT(opt != NULL);
	TEST_ASSERT(opt->set == 1);
	TEST_ASSERT(opt->pos == 3);
	TEST_ASSERT(opt->type == AP_ARG_TYPE_FLAG);

	opt = argp_get_opt(cmd, "-v");
	TEST_ASSERT(opt != NULL);
	TEST_ASSERT(opt->set == 1);
	TEST_ASSERT(opt->pos == 1);
	TEST_ASSERT(opt->type == AP_ARG_TYPE_STRING);
	TEST_ASSERT(ft_strcmp(opt->value, "all") == 0);

	opt = argp_get_opt(cmd, "-h");
	TEST_ASSERT(opt != NULL);
	TEST_ASSERT(opt->set == 0);
	TEST_ASSERT(opt->type == AP_ARG_TYPE_NUMBER);

	return (SSL_OK);
}

static int	__test_argp_parse(void)
{
	TEST_PASS();
}