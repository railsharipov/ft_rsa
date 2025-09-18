#include <common.h>
#include <argp.h>
#include <libft/htable.h>

t_ap_cmd	*argp_new_cmd(const char *name, const char *desc, void *func)
{
	t_ap_cmd *cmd;

	if (name == NULL) {
		name = DEFAULT_CMD;
	}
	if (desc == NULL) {
		desc = DEFAULT_CMD;
	}
	SSL_ALLOC(cmd, sizeof(t_ap_cmd));
	cmd->func = func;
	cmd->name = ft_strdup(name);
	cmd->desc = ft_strdup(desc);
	cmd->opts = ft_htbl_create(LIBFT_HT_SIZE);

	return (cmd);
}