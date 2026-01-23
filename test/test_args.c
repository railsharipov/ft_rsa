#include <common.h>
#include <args.h>
#include <test.h>

static int	__test_args_setup(void);
static int	__test_args_parse(void);

int	test_args(void)
{
	if (SSL_OK != __test_args_setup()) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		TEST_FAIL();
	}

	return (
		__test_args_parse()
	);
}

static int	__test_args_setup(void)
{
	return (SSL_OK);
}

static int	__test_args_parse(void)
{
	// const char *sargs[] = {"test", "-v", "all", "-h", "10", NULL};
	// const char *sargs_2[] = { "test", "-v", "all", "-i", NULL};
	// t_args args;
	// t_args_cmd *cmd;
	// t_args_opt *opt;
	// int ret;

	// // test 1
	// ret = args_init(&args);
	// TEST_ASSERT(ret == SSL_OK);

	// cmd = args_cmd_new("test", NULL, NULL);
	// TEST_ASSERT(cmd != NULL);

	// opt = args_opt_new("-v", "test", AP_OPT_TYPE_STRING);
	// TEST_ASSERT(opt != NULL);
	// ret = args_add_sub_cmd(cmd, opt);
	// TEST_ASSERT(ret == SSL_OK);
	// opt = args_opt_new("-h", "test", AP_OPT_TYPE_NUMBER);
	// TEST_ASSERT(opt != NULL);
	// ret = args_add_sub_cmd(cmd, opt);
	// TEST_ASSERT(ret == SSL_OK);
	// opt = args_opt_new("-i", "test", AP_OPT_TYPE_FLAG);
	// TEST_ASSERT(opt != NULL);
	// ret = args_add_sub_cmd(cmd, opt);
	// TEST_ASSERT(ret == SSL_OK);

	// ret = args_add_sub_cmd(&args, cmd);
	// TEST_ASSERT(ret == SSL_OK);

	// ret = args_parse(&args, sargs);
	// TEST_ASSERT(ret == SSL_OK);

	// cmd = args_get_cmd(&args, "test");
	// TEST_ASSERT(cmd != NULL);
	// TEST_ASSERT(ft_strcmp(cmd->name, "test") == 0);
	// TEST_ASSERT(cmd->func == NULL);

	// opt = args_get_cmd_opt(cmd, "-v");
	// TEST_ASSERT(opt != NULL);
	// TEST_ASSERT(opt->set == 1);
	// TEST_ASSERT(opt->pos == 1);
	// TEST_ASSERT(opt->type == AP_OPT_TYPE_STRING);
	// TEST_ASSERT(ft_strcmp(opt->value, "all") == 0);

	// opt = args_get_cmd_opt(cmd, "-h");
	// TEST_ASSERT(opt != NULL);
	// TEST_ASSERT(opt->set == 1);
	// TEST_ASSERT(opt->pos == 3);
	// TEST_ASSERT(opt->type == AP_OPT_TYPE_NUMBER);
	// TEST_ASSERT(ft_strcmp(opt->value, "10") == 0);

	// opt = args_get_cmd_opt(cmd, "-i");
	// TEST_ASSERT(opt != NULL);
	// TEST_ASSERT(opt->set == 0);
	// TEST_ASSERT(opt->type == AP_OPT_TYPE_FLAG);

	// // test 2
	// ret = args_init(&args);
	// TEST_ASSERT(ret == SSL_OK);

	// cmd = args_cmd_new("test", NULL, NULL);
	// TEST_ASSERT(cmd != NULL);

	// opt = args_opt_new("-v", "test", AP_OPT_TYPE_STRING);
	// TEST_ASSERT(opt != NULL);
	// ret = args_add_sub_cmd(cmd, opt);
	// TEST_ASSERT(ret == SSL_OK);
	// opt = args_opt_new("-h", "test", AP_OPT_TYPE_NUMBER);
	// TEST_ASSERT(opt != NULL);
	// ret = args_add_sub_cmd(cmd, opt);
	// TEST_ASSERT(ret == SSL_OK);
	// opt = args_opt_new("-i", "test", AP_OPT_TYPE_FLAG);
	// TEST_ASSERT(opt != NULL);
	// ret = args_add_sub_cmd(cmd, opt);
	// TEST_ASSERT(ret == SSL_OK);

	// ret = args_add_sub_cmd(&args, cmd);
	// TEST_ASSERT(ret == SSL_OK);

	// ret = args_parse(&args, sargs_2);
	// TEST_ASSERT(ret == SSL_OK);

	// cmd = args_get_cmd(&args, "test");
	// TEST_ASSERT(cmd != NULL);
	// TEST_ASSERT(ft_strcmp(cmd->name, "test") == 0);
	// TEST_ASSERT(cmd->func == NULL);

	// opt = args_get_cmd_opt(cmd, "-i");
	// TEST_ASSERT(opt != NULL);
	// TEST_ASSERT(opt->set == 1);
	// TEST_ASSERT(opt->pos == 3);
	// TEST_ASSERT(opt->type == AP_OPT_TYPE_FLAG);

	// opt = args_get_cmd_opt(cmd, "-v");
	// TEST_ASSERT(opt != NULL);
	// TEST_ASSERT(opt->set == 1);
	// TEST_ASSERT(opt->pos == 1);
	// TEST_ASSERT(opt->type == AP_OPT_TYPE_STRING);
	// TEST_ASSERT(ft_strcmp(opt->value, "all") == 0);

	// opt = args_get_cmd_opt(cmd, "-h");
	// TEST_ASSERT(opt != NULL);
	// TEST_ASSERT(opt->set == 0);
	// TEST_ASSERT(opt->type == AP_OPT_TYPE_NUMBER);

	return (SSL_OK);
}