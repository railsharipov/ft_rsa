#include <common.h>
#include <args.h>
#include <libft/htable.h>

static void	__func_del_opt(void *content);

int	args_add_global_opt(t_arg_cmd *cmd_arg, const t_arg_opt *opt_arg)
{
	if (cmd_arg == NULL || opt_arg == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_htbl_add_with_f_del(cmd_arg->global_opts, (void *)opt_arg, opt_arg->name, __func_del_opt);

	return (SSL_OK);
}

static void	__func_del_opt(void *content)
{
	t_arg_opt *opt = (t_arg_opt *)content;
	SSL_FREE(opt->name);
	SSL_FREE(opt->desc);
	SSL_FREE(opt->value);
	LIBFT_FREE(opt);
}