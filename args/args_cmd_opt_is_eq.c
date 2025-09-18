#include <common.h>
#include <args.h>
#include <libft/htable.h>

int args_cmd_opt_is_eq(const t_args_cmd *cmd, const char *opt_name, const char *opt_value)
{
	t_args_opt *opt;

	if (cmd == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}
	if (opt_name == NULL || opt_value == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}
	opt = ft_htbl_get(cmd->opts, opt_name);
	if (opt == NULL) {
		ARGP_LOG(ERROR, "checking value of unknown option: %s", opt_name);
		return (0);
	}
	if (opt->set == 0 || opt->value == NULL) {
		return (0);
	}
	return (ft_strcmp(opt->value, opt_value) == 0);
}