#include <args.h>
#include <logger.h>
#include <libft.h>

static int	__parse_opts(t_htbl *opts, t_htbl *global_opts, t_htbl *parsed_opts, const char **argv);
static void	*__func_copy_opt(void *content);

int	args_parse(t_cmd *cmd, t_arg_cmd *cmd_arg, const char **argv, int argc)
{
	t_htbl	*global_opts;
	int		pos;

	if (NULL == cmd) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_bzero(cmd, sizeof(t_cmd));

	if (cmd_arg == NULL || argv == NULL || argc <= 0) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (!ft_streq(argv[0], cmd_arg->name)) {
		SSL_LOG(ERROR, "invalid argument: %s", argv[0]);
		return (SSL_ERR);
	}

	global_opts = ft_htbl_create(LIBFT_HT_SIZE);
	pos = 1;
	while (argv[pos] != NULL) {
		ft_htbl_merge_with_f_copy(global_opts, cmd_arg->global_opts, __func_copy_opt);
		if (!ft_htbl_has(cmd_arg->sub_cmds, argv[pos])) {
			break;
		}
		cmd_arg = ft_htbl_get(cmd_arg->sub_cmds, argv[pos]);
		if (cmd_arg == NULL) {
			SSL_LOG(ERROR, UNEXPECTED_ERROR);
			ft_htbl_del(global_opts);
			return (SSL_ERR);
		}
		pos++;
	}
	cmd->arg_ref = cmd_arg;
	cmd->func = cmd_arg->func;
	cmd->opts = ft_htbl_create(LIBFT_HT_SIZE);

	if (SSL_OK != __parse_opts(cmd_arg->opts, global_opts, cmd->opts, argv + pos)) {
		ft_htbl_del(global_opts);
		ft_htbl_del(cmd->opts);
		return (SSL_ERR);
	}
	ft_htbl_del(global_opts);

	return (SSL_OK);
}

static int	__parse_opts(t_htbl *opts, t_htbl *global_opts, t_htbl *parsed_opts, const char **argv)
{
	t_arg_opt *opt;
	int pos;

	pos = 0;
	while (argv[pos] != NULL) {
		opt = ft_htbl_get(opts, argv[pos]);
		if (opt == NULL) {
			opt = ft_htbl_get(global_opts, argv[pos]);
		}
		if (opt == NULL) {
			SSL_LOG(ERROR, "invalid argument: %s", argv[pos]);
			return (SSL_ERR);
		}
		if (ft_htbl_has(parsed_opts, opt->name)) {
			SSL_LOG(ERROR, "duplicate option: %s", opt->name);
			return (SSL_ERR);
		}
		switch (opt->type) {
			case AP_OPT_TYPE_FLAG:
				ft_htbl_add(parsed_opts, NULL, opt->name);
				break;
			case AP_OPT_TYPE_STRING:
				pos++;
				if (argv[pos] == NULL) {
					SSL_LOG(ERROR, "missing value for `%s` of type string", opt->name);
					return (SSL_ERR);
				}
				ft_htbl_add(parsed_opts, (void *)argv[pos], opt->name);
				break;
			case AP_OPT_TYPE_NUMBER:
				pos++;
				if (argv[pos] == NULL) {
					SSL_LOG(ERROR, "missing value for `%s` of type number", opt->name);
					return (SSL_ERR);
				}
				if (ft_str_isnum(argv[pos])) {
					ft_htbl_add(parsed_opts, (void *)argv[pos], opt->name);
				} else {
					SSL_LOG(ERROR, "invalid value for `%s` of type number: %s", opt->name, argv[pos]);
					return (SSL_ERR);
				}
				break;
			default:
				SSL_LOG(ERROR, "invalid argument type: %d", opt->type);
				return (SSL_ERR);
		}
		pos++;
	}
	return (SSL_OK);
}

static void	*__func_copy_opt(void *content)
{
	return (args_copy_opt(content));
}
