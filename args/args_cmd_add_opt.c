#include <common.h>
#include <args.h>
#include <libft/htable.h>

static void	__func_del_opt(void *content);

int	args_cmd_add_opt(t_args_cmd *cmd, const t_args_opt *opt)
{
	if (cmd == NULL || opt == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_htbl_add_with_f_del(cmd->opts, (void *)opt, opt->name, __func_del_opt);
	return (SSL_OK);
}

static void	__func_del_opt(void *content)
{
	t_args_opt *opt = (t_args_opt *)content;
	SSL_FREE(opt->name);
	SSL_FREE(opt->desc);
	SSL_FREE(opt->value);
	LIBFT_FREE(opt);
}