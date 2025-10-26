#include <args.h>
#include <libft/list.h>

static void	*__func_copy_opt(void *content);

t_cmd	*args_parse(t_arg_cmd *cmd_arg, const char **argv, int argc)
{
	t_arg_cmd *cur_cmd_arg;
	t_arg_opt *cur_opt_arg;
	t_cmd 	*cmd;
	t_htbl  *global_opts;
	int		pos;

	if (cmd_arg == NULL || argv == NULL || argc <= 0) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	if (!ft_streq(argv[0], cmd_arg->name)) {
		ARGP_LOG(ERROR, "unknown command: %s", argv[0]);
		return (NULL);
	}

	global_opts = ft_htbl_create(LIBFT_HT_SIZE);
	ft_htbl_merge_with_f_copy(global_opts, cmd_arg->global_opts, __func_copy_opt);

	pos = 1;
	while (argv[pos] != NULL) {
		cur_cmd_arg = ft_htbl_get(cmd_arg->sub_cmds, argv[pos]);
		cur_opt_arg = ft_htbl_get(cmd_arg->opts, argv[pos]);
		if (cur_opt_arg == NULL) {
			cur_opt_arg = ft_htbl_get(global_opts, argv[pos]);
		}

		if (cur_cmd_arg != NULL) {
			cmd_arg = cur_cmd_arg;
			ft_htbl_merge_with_f_copy(global_opts, cmd_arg->global_opts, __func_copy_opt);
		}
		else if (cur_opt_arg != NULL) {
			cur_opt_arg->pos = pos;
			switch (cur_opt_arg->type) {
				case AP_OPT_TYPE_FLAG:
					cur_opt_arg->value = NULL;
					break;
				case AP_OPT_TYPE_STRING:
					pos++;
					if (argv[pos] == NULL) {
						ARGP_LOG(ERROR, "missing value for `%s` of type string", cur_opt_arg->name);
						return (NULL);
					}
					cur_opt_arg->value = ft_strdup(argv[pos]);
					break;
				case AP_OPT_TYPE_NUMBER:
					pos++;
					if (argv[pos] == NULL) {
						ARGP_LOG(ERROR, "missing value for `%s` of type number", cur_opt_arg->name);
						return (NULL);
					}
					if (ft_str_isnum(argv[pos])) {
						cur_opt_arg->value = ft_strdup(argv[pos]);
					} else {
						ARGP_LOG(ERROR, "invalid value for `%s` of type number: %s", cur_opt_arg->name, argv[pos]);
						return (NULL);
					}
					break;
				default:
					ARGP_LOG(ERROR, "unknown option type: %d", cur_opt_arg->type);
					return (NULL);
			}
			cur_opt_arg->set = 1;
		}
		else {
			ARGP_LOG(ERROR, "%s command: invalid argument: %s", cmd_arg->name, argv[pos]);
			return (NULL);
		}
		pos++;
	}

	SSL_ALLOC(cmd, sizeof(t_cmd));
	cmd->arg_ref = cmd_arg;
	cmd->func = cmd_arg->func;
	cmd->opts = ft_htbl_create(LIBFT_HT_SIZE);
	ft_htbl_merge_with_f_copy(cmd->opts, global_opts, __func_copy_opt);

	return (cmd);
}

static void	*__func_copy_opt(void *content)	{
	return (args_copy_opt(content));
}