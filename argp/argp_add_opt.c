#include <common.h>
#include <argp.h>
#include <libft/htable.h>

static void	__func_del_opt(void *content);

t_ap_cmd	*argp_add_opt(t_ap_cmd *cmd, const t_ap_arg opt_def)
{
	t_ap_opt *opt = NULL;

	if (cmd == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	if (opt_def.name == NULL) {
		ARGP_LOG(ERROR, "option key is not set");
		goto label_error;
	}
	switch (opt_def.type) {
		case AP_ARG_TYPE_FLAG:
		case AP_ARG_TYPE_STRING:
		case AP_ARG_TYPE_NUMBER:
			break;
		default:
			ARGP_LOG(ERROR, "unknown option type: %d", opt_def.type);
			goto label_error;
	}
	SSL_ALLOC(opt, sizeof(t_ap_opt));
	opt->type = opt_def.type;
	opt->name = ft_strdup(opt_def.name);
	opt->value = NULL;
	opt->pos = 0;
	opt->set = 0;
	ft_htbl_add_with_f_del(cmd->opts, opt, opt->name, __func_del_opt);

	return (cmd);

label_error:
	ft_htbl_del(cmd->opts);
	return (NULL);
}

static void	__func_del_opt(void *content)
{
	t_ap_opt *opt = (t_ap_opt *)content;
	SSL_FREE(opt->name);
	SSL_FREE(opt->desc);
	SSL_FREE(opt->value);
	LIBFT_FREE(opt);
}