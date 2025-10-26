#include <common.h>
#include <args.h>
#include <libft/htable.h>

t_arg_cmd	*args_new_cmd(const char *name, const char *desc, void *func)
{
	t_arg_cmd *cmd_arg;

	if (name == NULL) {
		name = DEFAULT_CMD;
	}
	if (desc == NULL) {
		desc = DEFAULT_CMD;
	}
	SSL_ALLOC(cmd_arg, sizeof(t_arg_cmd));
	cmd_arg->name = ft_strdup(name);
	cmd_arg->desc = ft_strdup(desc);
	cmd_arg->func = func;
	cmd_arg->sub_cmds = ft_htbl_create(LIBFT_HT_SIZE);
	cmd_arg->opts = ft_htbl_create(LIBFT_HT_SIZE);
	cmd_arg->global_opts = ft_htbl_create(LIBFT_HT_SIZE);
	
	return (cmd_arg);
}
