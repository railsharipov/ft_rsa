#include <common.h>
#include <logger.h>
#include <args.h>
#include "test.h"

static int	__test_args_setup(void);
static int	__test_args_parse(void);

int	test_args(void)
{
	if (SSL_OK != __test_args_setup()) {
		TEST_LOG(ERROR, TEST_SETUP_ERROR);
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
	t_arg_cmd *cmd_arg, *sub_cmd_arg;
	t_arg_opt *opt_arg;
	t_cmd cmd;
	int ret;

	// new command
	cmd_arg = args_new_cmd("test", "test command", free);
	TEST_ASSERT(cmd_arg != NULL);
	TEST_ASSERT(ft_streq(cmd_arg->name, "test"));
	TEST_ASSERT(ft_streq(cmd_arg->desc, "test command"));
	TEST_ASSERT(cmd_arg->func == free);
	TEST_ASSERT(cmd_arg->sub_cmds != NULL);
	TEST_ASSERT(cmd_arg->opts != NULL);
	TEST_ASSERT(cmd_arg->global_opts != NULL);

	// new option
	opt_arg = args_new_opt("-v", "v", AP_OPT_TYPE_STRING);
	TEST_ASSERT(opt_arg != NULL);
	TEST_ASSERT(ft_streq(opt_arg->name, "-v"));
	TEST_ASSERT(ft_streq(opt_arg->desc, "v"));
	TEST_ASSERT(opt_arg->type == AP_OPT_TYPE_STRING);

	// add options to command
	ret = args_add_cmd_opt(cmd_arg, args_new_opt("-v", "v", AP_OPT_TYPE_STRING));
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(ft_htbl_has(cmd_arg->opts, "-v"));
	TEST_ASSERT(ft_htbl_get(cmd_arg->opts, "-v") != NULL);

	ret = args_add_cmd_opt(cmd_arg, args_new_opt("-x", "x", AP_OPT_TYPE_NUMBER));
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(ft_htbl_has(cmd_arg->opts, "-x"));
	TEST_ASSERT(ft_htbl_get(cmd_arg->opts, "-x") != NULL);

	ret = args_add_cmd_opt(cmd_arg, args_new_opt("-i", "i", AP_OPT_TYPE_FLAG));
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(ft_htbl_has(cmd_arg->opts, "-i"));
	TEST_ASSERT(ft_htbl_get(cmd_arg->opts, "-i") != NULL);

	// must have default -h option
	TEST_ASSERT(ft_htbl_has(cmd_arg->opts, "-h"));
	TEST_ASSERT(ft_htbl_get(cmd_arg->opts, "-h") != NULL);

	// parse command
	const char *argv_0[] = {"test", "-v", "all", "-x", "10", "-i", "-h", NULL};
	ft_bzero(&cmd, sizeof(t_cmd));

	ret = args_parse(&cmd, cmd_arg, argv_0, 5);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(cmd.arg_ref == cmd_arg);
	TEST_ASSERT(cmd.func == free);
	TEST_ASSERT(cmd.opts != NULL);
	TEST_ASSERT(ft_htbl_has(cmd.opts, "-v"));
	TEST_ASSERT(ft_streq(ft_htbl_get(cmd.opts, "-v"), "all"));
	TEST_ASSERT(ft_htbl_has(cmd.opts, "-x"));
	TEST_ASSERT(ft_streq(ft_htbl_get(cmd.opts, "-x"), "10"));
	TEST_ASSERT(ft_htbl_has(cmd.opts, "-i"));
	TEST_ASSERT(ft_htbl_get(cmd.opts, "-i") == NULL);
	TEST_ASSERT(ft_htbl_has(cmd.opts, "-h"));
	TEST_ASSERT(ft_htbl_get(cmd.opts, "-h") == NULL);

	// parsed command should have only options that were parsed
	const char *argv_1[] = {"test", "-v", "all", NULL};
	ft_bzero(&cmd, sizeof(t_cmd));

	ret = args_parse(&cmd, cmd_arg, argv_1, 3);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(ft_htbl_has(cmd.opts, "-v"));
	TEST_ASSERT(ft_streq(ft_htbl_get(cmd.opts, "-v"), "all"));
	TEST_ASSERT(!ft_htbl_has(cmd.opts, "-h"));
	TEST_ASSERT(!ft_htbl_has(cmd.opts, "-x"));
	TEST_ASSERT(!ft_htbl_has(cmd.opts, "-i"));

	// add global options
	ret = args_add_global_cmd_opt(cmd_arg, args_new_opt("-g", "g", AP_OPT_TYPE_STRING));
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(ft_htbl_has(cmd_arg->global_opts, "-g"));
	TEST_ASSERT(ft_htbl_get(cmd_arg->global_opts, "-g") != NULL);

	// parse command with global options
	const char *argv_2[] = {"test", "-v", "all", "-h", "-g", "global", NULL};
	ft_bzero(&cmd, sizeof(t_cmd));

	ret = args_parse(&cmd, cmd_arg, argv_2, 6);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(ft_htbl_has(cmd.opts, "-g"));
	TEST_ASSERT(ft_streq(ft_htbl_get(cmd.opts, "-g"), "global"));

	// parse command with unknown option
	const char *argv_3[] = { "test", "-v", "--invalid", "--rip", NULL};
	ft_bzero(&cmd, sizeof(t_cmd));

	ret = args_parse(&cmd, cmd_arg, argv_3, 3);
	TEST_ASSERT(ret == SSL_ERR);

	// parse command with missing value
	const char *argv_4[] = { "test", "-v", NULL};
	ft_bzero(&cmd, sizeof(t_cmd));

	ret = args_parse(&cmd, cmd_arg, argv_4, 2);
	TEST_ASSERT(ret == SSL_ERR);

	// parse command with invalid value type
	const char *argv_5[] = { "test", "-x", "abc", NULL};
	ft_bzero(&cmd, sizeof(t_cmd));

	ret = args_parse(&cmd, cmd_arg, argv_5, 2);
	TEST_ASSERT(ret == SSL_ERR);

	// parse command with duplicate option
	const char *argv_6[] = { "test", "-v", "all", "-v", "single", NULL};
	ft_bzero(&cmd, sizeof(t_cmd));

	ret = args_parse(&cmd, cmd_arg, argv_6, 4);
	TEST_ASSERT(ret == SSL_ERR);

	// parse command with duplicate global option
	const char *argv_7[] = { "test", "-v", "all", "-g", "global", "-g", "cooling", NULL};
	ft_bzero(&cmd, sizeof(t_cmd));

	ret = args_parse(&cmd, cmd_arg, argv_7, 6);
	TEST_ASSERT(ret == SSL_ERR);

	// add sub command
	sub_cmd_arg = args_new_cmd("sub", "sub command", ft_streq);
	TEST_ASSERT(sub_cmd_arg != NULL);
	ret = args_add_sub_cmd(cmd_arg, sub_cmd_arg);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(ft_htbl_has(cmd_arg->sub_cmds, "sub"));
	TEST_ASSERT(ft_htbl_get(cmd_arg->sub_cmds, "sub") == sub_cmd_arg);

	// parse command with sub command with default options
	const char *argv_8[] = { "test", "sub", "-h", "-g", "global", NULL};
	ft_bzero(&cmd, sizeof(t_cmd));

	ret = args_parse(&cmd, cmd_arg, argv_8, 3);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(cmd.arg_ref == sub_cmd_arg);
	TEST_ASSERT(cmd.func == ft_streq);
	TEST_ASSERT(ft_htbl_has(cmd.opts, "-h"));
	TEST_ASSERT(ft_htbl_get(cmd.opts, "-h") == NULL);
	// sub command should have main command global options
	TEST_ASSERT(ft_htbl_has(cmd.opts, "-g"));
	TEST_ASSERT(ft_streq(ft_htbl_get(cmd.opts, "-g"), "global"));

	// add options to sub command
	ret = args_add_cmd_opt(sub_cmd_arg, args_new_opt("-s", "s", AP_OPT_TYPE_STRING));
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(ft_htbl_has(sub_cmd_arg->opts, "-s"));
	TEST_ASSERT(ft_htbl_get(sub_cmd_arg->opts, "-s") != NULL);

	// parse command with sub-commands with options
	const char *argv_9[] = { "test", "sub", "-s", "slurp", "-g", "global", NULL};
	ft_bzero(&cmd, sizeof(t_cmd));

	ret = args_parse(&cmd, cmd_arg, argv_9, 3);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(cmd.arg_ref == sub_cmd_arg);
	TEST_ASSERT(cmd.func == ft_streq);
	TEST_ASSERT(ft_htbl_has(cmd.opts, "-s"));
	TEST_ASSERT(ft_streq(ft_htbl_get(cmd.opts, "-s"), "slurp"));
	// sub command should have main command global options
	TEST_ASSERT(ft_htbl_has(cmd.opts, "-g"));
	TEST_ASSERT(ft_streq(ft_htbl_get(cmd.opts, "-g"), "global"));

	// parse command with invalid argument order: sub-command must not follow an option
	const char *argv_10[] = { "test", "-v", "all", "sub", "-g", "global", "-s", "slurp", NULL};
	ft_bzero(&cmd, sizeof(t_cmd));

	ret = args_parse(&cmd, cmd_arg, argv_10, 8);
	TEST_ASSERT(ret == SSL_ERR);

	return (SSL_OK);
}
