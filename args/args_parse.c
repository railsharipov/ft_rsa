#include <args.h>
#include <libft/list.h>

t_cmd	*args_parse(t_arg_cmd *cmd_arg, const char **argv, int argc)
{
	t_htbl	*opts;
	t_cmd 	*cmd;
	int		pos;

	if (cmd_arg == NULL || argv == NULL || argc <= 0) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	if (!ft_streq(argv[0], cmd_arg->name)) {
		ARGP_LOG(ERROR, "unknown command: %s", argv[0]);
		return (NULL);
	}
	SSL_ALLOC(opts, sizeof(t_htbl));
	opts = ft_htbl_create(LIBFT_HT_SIZE);

	pos = 1;
	while (argv[pos] != NULL) {
		t_arg_cmd *cur_cmd_arg = ft_htbl_get(cmd_arg->sub_cmds, argv[pos]);
		t_arg_opt *cur_opt_arg = ft_htbl_get(cmd_arg->opts, argv[pos]);

		if (cur_cmd_arg != NULL) {
			cmd_arg = cur_cmd_arg;
		}
		else if (cur_opt_arg != NULL) {
			switch (cur_opt_arg->type) {
				case AP_OPT_TYPE_FLAG:
					ft_htbl_add(opts, NULL, cur_opt_arg->name);
					break;
				case AP_OPT_TYPE_STRING:
					pos++;
					if (argv[pos] == NULL) {
						ARGP_LOG(ERROR, "missing value for `%s` of type string", cur_opt_arg->name);
						return (NULL);
					}
					ft_htbl_add(opts, (void *)argv[pos], cur_opt_arg->name);
					break;
				case AP_OPT_TYPE_NUMBER:
					pos++;
					if (argv[pos] == NULL) {
						ARGP_LOG(ERROR, "missing value for `%s` of type number", cur_opt_arg->name);
						return (NULL);
					}
					if (ft_str_isnum(argv[pos])) {
						ft_htbl_add(opts, (void *)argv[pos], cur_opt_arg->name);
					} else {
						ARGP_LOG(ERROR, "invalid value for `%s` of type number: %s", cur_opt_arg->name, argv[pos]);
						return (NULL);
					}
					break;
				default:
					ARGP_LOG(ERROR, "unknown option type: %d", cur_opt_arg->type);
					return (NULL);
			}
		}
		else {
			ARGP_LOG(ERROR, "%s command: invalid argument: %s", cmd_arg->name, argv[pos]);
			ft_htbl_del(opts);
			return (NULL);
		}
		pos++;
	}
	SSL_ALLOC(cmd, sizeof(t_cmd));
	cmd->func = cmd_arg->func;
	cmd->opts = opts;
	cmd->arg = cmd_arg;

	return (cmd);
}
