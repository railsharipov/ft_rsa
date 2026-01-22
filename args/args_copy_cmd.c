#include <common.h>
#include <args.h>
#include <libft/htable.h>

static void	*__func_copy_cmd(void *content);
static void	*__func_copy_opt(void *content);

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

	return (copy);
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
		args_add_cmd(copy, sub_copy);
	}

	node = ft_htbl_node_next(cmd_arg->opts, NULL);
	if (node != NULL) {
		opt = node->content;
		opt_copy = args_copy_opt(opt);
		args_add_opt(copy, opt_copy);
	}

	return (copy);
}

static void	*__func_copy_opt(void *content)
{
	return (args_copy_opt(content));
}