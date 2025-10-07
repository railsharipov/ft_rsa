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
	ft_ostr_appendf(&ostring, "\t%s, %s\n", cmd->name, cmd->desc);

	node = ft_htbl_node_next(cmd->opts, NULL);
	while (node != NULL) {
		t_args_opt *opt = (t_args_opt *)node->content;
		ft_ostr_appendf(&ostring, "\t\t%s, %s\n", opt->name, opt->desc);
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
	ft_ostr_appendf(&ostring, "USAGE:\n\tcommand [options]\n");

	node = ft_htbl_node_next(args->htbl, NULL);
	if (node != NULL) {
		ft_ostr_appendf(&ostring, "COMMANDS:\n");
	}
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