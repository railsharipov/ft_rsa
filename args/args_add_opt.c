#include <common.h>
#include <args.h>
#include <libft/htable.h>

static void	__func_del_opt(void *content);

int	args_add_opt(t_arg_cmd *cmd_arg, const t_arg_opt *opt_arg)
{
	if (cmd_arg == NULL || opt_arg == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_htbl_add_with_f_del(cmd_arg->opts, (void *)opt_arg, opt_arg->name, __func_del_opt);

	return (SSL_OK);
}

static void	__func_del_opt(void *content)
{
	t_arg_opt *opt_arg = (t_arg_opt *)content;
	SSL_FREE(opt_arg->name);
	SSL_FREE(opt_arg->desc);
	SSL_FREE(opt_arg->value);
	LIBFT_FREE(opt_arg);
}