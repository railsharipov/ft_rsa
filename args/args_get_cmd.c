#include <common.h>
#include <args.h>
#include <libft/htable.h>

t_args_cmd	*args_get_cmd(const t_args *args, const char *cmd_name)
{
	t_args_cmd *cmd;

	if (args == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	if (cmd_name == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	cmd = ft_htbl_get(args->htbl, cmd_name);
	if (cmd == NULL) {
		ARGP_LOG(ERROR, "unknown command: %s", cmd_name);
		return (NULL);
	}
	return (cmd);
}