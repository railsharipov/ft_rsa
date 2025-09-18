#include <common.h>
#include <args.h>
#include <libft/htable.h>

t_args_opt	*args_cmd_get_opt(const t_args_cmd *cmd, const char *opt_name)
{
	t_args_opt *opt;

	if (cmd == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	if (opt_name == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	opt = ft_htbl_get(cmd->opts, opt_name);
	if (opt == NULL) {
		ARGP_LOG(ERROR, "unknown option: %s", opt_name);
		return (NULL);
	}
	return (opt);
}