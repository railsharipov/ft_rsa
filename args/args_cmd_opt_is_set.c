#include <common.h>
#include <args.h>
#include <libft/htable.h>

int args_cmd_opt_is_set(const t_args_cmd *cmd, const char *opt_name)
{
	t_args_opt *opt;

	if (cmd == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}
	if (opt_name == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}
	opt = ft_htbl_get(cmd->opts, opt_name);
	if (opt == NULL) {
		ARGP_LOG(ERROR, "checking unknown option: %s", opt_name);
		return (0);
	}
	return (opt->set);
}