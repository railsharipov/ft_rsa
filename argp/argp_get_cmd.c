#include <common.h>
#include <argp.h>
#include <libft/htable.h>

t_ap_cmd	*argp_get_cmd(t_ap_parser *parser, const char *cmd_name)
{
	t_ap_cmd *cmd;

	if (parser == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	if (cmd_name == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	cmd = ft_htbl_get(parser->htbl, cmd_name);
	if (cmd == NULL) {
		ARGP_LOG(ERROR, "unknown command: %s", cmd_name);
		return (NULL);
	}
	return (cmd);
}