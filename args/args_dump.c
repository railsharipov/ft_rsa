#include <common.h>
#include <args.h>
#include <libft/htable.h>

char	*args_dump_cmd_helps(const t_args_cmd *cmd)
{
	t_ostring	ostring;
	t_node		*node;
	char		*helps;

	if (cmd == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	ft_ostr_init(&ostring);
	ft_ostr_append(&ostring, "Usage: ", 7);
	ft_ostr_append(&ostring, cmd->name, ft_strlen(cmd->name));
	ft_ostr_append(&ostring, " [options]\n", 11);
	ft_ostr_append(&ostring, cmd->desc, ft_strlen(cmd->desc));
	ft_ostr_append(&ostring, "\n", 1);

	node = ft_htbl_node_next(cmd->opts, NULL);
	while (node != NULL) {
		t_args_opt *opt = (t_args_opt *)node->content;
		ft_ostr_append(&ostring, opt->name, ft_strlen(opt->name));
		ft_ostr_append(&ostring, " ", 1);
		ft_ostr_append(&ostring, opt->desc, ft_strlen(opt->desc));
		ft_ostr_append(&ostring, "\n", 1);
		node = ft_htbl_node_next(cmd->opts, node);
	}

	helps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);

	return (helps);
}

char	*args_dump_helps(const t_args *args)
{
	t_ostring	ostring;
	t_node		*node;
	char		*cmd_helps, *helps;

	if (args == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	ft_ostr_init(&ostring);
	node = ft_htbl_node_next(args->htbl, NULL);
	while (node != NULL) {
		t_args_cmd *cmd = (t_args_cmd *)node->content;
		cmd_helps = args_dump_cmd_helps(cmd);
		ft_ostr_append(&ostring, cmd_helps, ft_strlen(cmd_helps));
		ft_ostr_append(&ostring, "\n", 1);
		SSL_FREE(cmd_helps);
		node = ft_htbl_node_next(args->htbl, node);
	}

	helps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);

	return (helps);
}

void	args_dump_help(const t_args *args)
{
	char	*helps;

	if (args == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return ;
	}
	helps = args_dump_helps(args);
	ft_printf("%s", helps);
	SSL_FREE(helps);
}