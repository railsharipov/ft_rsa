#include <common.h>
#include <args.h>
#include <libft/htable.h>

t_arg_cmd	*args_get_cmd(const t_arg_cmd *cmd_arg, const char *cmd_name)
{
	if (cmd_arg == NULL || cmd_name == NULL) {
		return (NULL);
	}
	return (ft_htbl_get(cmd_arg->sub_cmds, cmd_name));
}