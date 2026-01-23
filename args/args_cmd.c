#include <common.h>
#include <args.h>
#include <libft/htable.h>

static void	*__func_copy_cmd(void *content);
static void	*__func_copy_opt(void *content);
static void	__func_del_cmd(void *content);
static void	__func_del_opt(void *content);

t_arg_cmd	*args_new_cmd(const char *name, const char *desc, void *func)
{
	t_arg_cmd *cmd_arg;
	t_arg_opt *help_opt_arg;

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

	help_opt_arg = args_new_opt("-h", "show help", AP_OPT_TYPE_FLAG);
	args_add_cmd_opt(cmd_arg, help_opt_arg);

	return (cmd_arg);
}

void	args_del_cmd(t_arg_cmd *cmd_arg)
{
	if (cmd_arg == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return;
	}
	ft_htbl_del(cmd_arg->sub_cmds);
	ft_htbl_del(cmd_arg->opts);
	ft_htbl_del(cmd_arg->global_opts);
	SSL_FREE(cmd_arg->name);
	SSL_FREE(cmd_arg->desc);
	SSL_FREE(cmd_arg);
}

t_arg_cmd	*args_copy_cmd(const t_arg_cmd *cmd_arg)
{
	t_arg_cmd *copy;

	if (cmd_arg == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	copy = args_new_cmd(cmd_arg->name, cmd_arg->desc, cmd_arg->func);

	ft_htbl_merge_with_f_copy(copy->sub_cmds, cmd_arg->sub_cmds, __func_copy_cmd);
	ft_htbl_merge_with_f_copy(copy->opts, cmd_arg->opts, __func_copy_opt);
	ft_htbl_merge_with_f_copy(copy->global_opts, cmd_arg->global_opts, __func_copy_opt);

	return (copy);
}

int	args_add_sub_cmd(t_arg_cmd *cmd_arg, const t_arg_cmd *sub_cmd_arg)
{
	if (cmd_arg == NULL || sub_cmd_arg == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_htbl_add_with_f_del(cmd_arg->sub_cmds, (void *)sub_cmd_arg, sub_cmd_arg->name, __func_del_cmd);

	return (SSL_OK);
}

int	args_add_cmd_opt(t_arg_cmd *cmd_arg, const t_arg_opt *opt_arg)
{
	if (cmd_arg == NULL || opt_arg == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_htbl_add_with_f_del(cmd_arg->opts, (void *)opt_arg, opt_arg->name, __func_del_opt);

	return (SSL_OK);
}

int	args_add_global_cmd_opt(t_arg_cmd *cmd_arg, const t_arg_opt *opt_arg)
{
	if (cmd_arg == NULL || opt_arg == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_htbl_add_with_f_del(cmd_arg->global_opts, (void *)opt_arg, opt_arg->name, __func_del_opt);

	return (SSL_OK);
}

static void	*__func_copy_cmd(void *content)
{
	t_node *node;
	t_arg_cmd *cmd_arg, *sub_cmd, *sub_copy;
	t_arg_opt *opt, *opt_copy;

	cmd_arg = content;
	t_arg_cmd *copy = args_new_cmd(cmd_arg->name, cmd_arg->desc, cmd_arg->func);

	node = ft_htbl_node_next(cmd_arg->sub_cmds, NULL);
	if (node != NULL) {
		sub_cmd = node->content;
		sub_copy = args_copy_cmd(sub_cmd);
		args_add_sub_cmd(copy, sub_copy);
	}

	node = ft_htbl_node_next(cmd_arg->opts, NULL);
	if (node != NULL) {
		opt = node->content;
		opt_copy = args_copy_opt(opt);
		args_add_cmd_opt(copy, opt_copy);
	}

	return (copy);
}

static void	__func_del_opt(void *content)
{
	t_arg_opt *opt_arg = (t_arg_opt *)content;
	args_del_opt(opt_arg);
}

static void	*__func_copy_opt(void *content)
{
	return (args_copy_opt(content));
}

static void	__func_del_cmd(void *content)
{
	t_arg_cmd *cmd = (t_arg_cmd *)content;
	args_del_cmd(cmd);
}