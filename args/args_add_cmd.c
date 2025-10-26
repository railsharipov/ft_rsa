#include <common.h>
#include <args.h>
#include <libft/htable.h>

static void	__func_del_cmd(void *content);

int	args_add_cmd(t_arg_cmd *cmd_arg, const t_arg_cmd *sub_cmd_arg)
{
	if (cmd_arg == NULL || sub_cmd_arg == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_htbl_add_with_f_del(cmd_arg->sub_cmds, (void *)sub_cmd_arg, sub_cmd_arg->name, __func_del_cmd);

	return (SSL_OK);
}

static void	__func_del_cmd(void *content)
{
	t_arg_cmd *cmd = (t_arg_cmd *)content;
	SSL_FREE(cmd->name);
	SSL_FREE(cmd->desc);
	LIBFT_FREE(cmd->opts);
	LIBFT_FREE(cmd);
}