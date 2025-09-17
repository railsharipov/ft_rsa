#include <argp.h>

int	argp_parse(t_ap_parser *parser, const char **sargs)
{
	t_ap_cmd *cmd;
	t_ap_opt *opt;
	int		pos;

	if (parser == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (sargs == NULL || *sargs == NULL) {
		return (SSL_OK);
	}
	pos = 0;
	cmd = ft_htbl_get(parser->htbl, sargs[pos]);
	if (cmd == NULL) {
		cmd = ft_htbl_get(parser->htbl, DEFAULT_CMD);
	} else {
		pos++;
	}
	while (sargs[pos] != NULL) {
		opt = ft_htbl_get(cmd->opts, sargs[pos]);
		if (opt == NULL) {
			ARGP_LOG(ERROR, "unknown option for `%s` command: %s", cmd->name, sargs[pos]);
			return (SSL_ERR);
		}
		opt->pos = pos;
		switch (opt->type) {
			case AP_ARG_TYPE_FLAG:
				opt->value = NULL;
				break;
			case AP_ARG_TYPE_STRING:
				pos++;
				if (sargs[pos] == NULL) {
					ARGP_LOG(ERROR, "missing value for `%s` of type string", opt->name);
					return (SSL_ERR);
				}
				opt->value = ft_strdup(sargs[pos]);
				break;
			case AP_ARG_TYPE_NUMBER:
				pos++;
				if (sargs[pos] == NULL) {
					ARGP_LOG(ERROR, "missing value for `%s` of type number", opt->name);
					return (SSL_ERR);
				}
				if (ft_str_isnum(sargs[pos])) {
					opt->value = ft_strdup(sargs[pos]);
				} else {
					ARGP_LOG(ERROR, "invalid value for `%s` of type number: %s", opt->name, sargs[pos]);
					return (SSL_ERR);
				}
				break;
			default:
				ARGP_LOG(ERROR, "unknown option type: %d", opt->type);
				return (SSL_ERR);
		}
		opt->set = 1;
		pos++;
	}
	return (SSL_OK);
}