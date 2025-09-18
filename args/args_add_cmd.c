#include <common.h>
#include <args.h>
#include <libft/htable.h>

static void	__func_del_cmd(void *content);

int	args_add_cmd(t_args *args, const t_args_cmd *cmd)
{
	if (args == NULL || cmd == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_htbl_add_with_f_del(args->htbl, (void *)cmd, cmd->name, __func_del_cmd);

	return (SSL_OK);
}

static void	__func_del_cmd(void *content)
{
	t_args_cmd *cmd = (t_args_cmd *)content;
	SSL_FREE(cmd->name);
	SSL_FREE(cmd->desc);
	ft_htbl_del(cmd->opts);
	LIBFT_FREE(cmd);
}