#include <common.h>
#include <logger.h>
#include <args.h>
#include <libft.h>

static char *__type_info(const char *name, t_opt_type type);

void	args_dump_help(const t_arg_cmd *cmd_arg)
{
	char	*helps;

	if (cmd_arg == NULL) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return ;
	}
	helps = args_dump_helps(cmd_arg);
	ft_printf("%s", helps);
	SSL_FREE(helps);
}

char	*args_dump_helps(const t_arg_cmd *cmd_arg)
{
	const int	indent_size = 4;
	char		indents[indent_size + 1];
	t_ostring	ostring;
	t_node		*node;
	t_arg_cmd	*sub_cmd;
	t_arg_opt	*opt;
	char		*opt_info;
	char		*helps;

	if (cmd_arg == NULL) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	ft_memset(indents, ' ', indent_size);
	indents[indent_size] = '\0';

	ft_ostr_init(&ostring);
	ft_ostr_appendf(&ostring, "USAGE:\n");
	ft_ostr_appendf(&ostring, "%s%s [sub-command [options...]] [options...]\n\n", indents, cmd_arg->name);

	node = ft_htbl_node_next(cmd_arg->sub_cmds, NULL);
	if (node != NULL) {
		ft_ostr_appendf(&ostring, "SUB-COMMANDS:\n");
		while (node != NULL) {
			sub_cmd = (t_arg_cmd *)node->content;
			ft_ostr_appendf(&ostring, "%s %-10s %s\n", indents, sub_cmd->name, sub_cmd->desc);
			node = ft_htbl_node_next(cmd_arg->sub_cmds, node);
		}
		ft_ostr_append_cstr(&ostring, "\n");
	}

	node = ft_htbl_node_next(cmd_arg->opts, NULL);
	if (node != NULL) {
		ft_ostr_appendf(&ostring, "OPTIONS:\n");
		while (node != NULL) {
			opt = (t_arg_opt *)node->content;
			opt_info = __type_info(opt->name, opt->type);
			ft_ostr_appendf(&ostring, "%s %-10s %s\n", indents, opt_info, opt->desc);
			ft_strdel(opt_info);
			node = ft_htbl_node_next(cmd_arg->opts, node);
		}
		ft_ostr_append_cstr(&ostring, "\n");
	}

	node = ft_htbl_node_next(cmd_arg->global_opts, NULL);
	if (node != NULL) {
		ft_ostr_appendf(&ostring, "GLOBAL OPTIONS:\n");
		while (node != NULL) {
			opt = (t_arg_opt *)node->content;
			opt_info = __type_info(opt->name, opt->type);
			ft_ostr_appendf(&ostring, "%s %-30s %s\n", indents, opt_info, opt->desc);
			ft_strdel(opt_info);
			node = ft_htbl_node_next(cmd_arg->global_opts, node);
		}
		ft_ostr_append_cstr(&ostring, "\n");
	}

	helps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);

	return (helps);
}

static char *__type_info(const char *name, t_opt_type type)
{
	const char *type_name;
	char *type_info;

	switch (type) {
		case AP_OPT_TYPE_FLAG:
			type_name = "";
			break;
		case AP_OPT_TYPE_STRING:
			type_name = "<string>";
			break;
		case AP_OPT_TYPE_NUMBER:
			type_name = "<number>";
			break;
		default:
			type_name = "";
	}
	type_info = ft_strjoin_multi(3, name, " ", type_name);

	return (type_info);
}
