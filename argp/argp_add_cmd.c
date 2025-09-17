#include <common.h>
#include <argp.h>
#include <libft/htable.h>

static void	__func_del_cmd(void *content);

int	argp_add_cmd(t_ap_parser *parser, const t_ap_cmd *cmd)
{
	if (parser == NULL || cmd == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_htbl_add_with_f_del(parser->htbl, cmd, cmd->name, __func_del_cmd);

	return (SSL_OK);
}

static void	__func_del_cmd(void *content)
{
	t_ap_cmd *cmd = (t_ap_cmd *)content;
	SSL_FREE(cmd->name);
	SSL_FREE(cmd->desc);
	ft_htbl_del(cmd->opts);
	LIBFT_FREE(cmd);
}