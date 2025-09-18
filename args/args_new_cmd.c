#include <common.h>
#include <args.h>
#include <libft/htable.h>

t_args_cmd	*args_new_cmd(const char *name, const char *desc, void *func)
{
	t_args_cmd *cmd;

	if (name == NULL) {
		name = DEFAULT_CMD;
	}
	if (desc == NULL) {
		desc = DEFAULT_CMD;
	}
	SSL_ALLOC(cmd, sizeof(t_args_cmd));
	cmd->func = func;
	cmd->name = ft_strdup(name);
	cmd->desc = ft_strdup(desc);
	cmd->opts = ft_htbl_create(LIBFT_HT_SIZE);

	return (cmd);
}
