#include <common.h>
#include <argp.h>
#include <libft/htable.h>

static void	__func_del_opt(void *content);

t_ap_cmd	*argp_new_cmd(const char *name, const char *desc, const t_ap_opt_def *opt_def_arr, size_t arr_size, void *func)
{
	t_ap_cmd *cmd;

	if (name == NULL) {
		name = DEFAULT_CMD;
	}
	if (desc == NULL) {
		desc = DEFAULT_CMD;
	}
	SSL_ALLOC(cmd, sizeof(t_ap_cmd));
	cmd->func = func;
	cmd->name = ft_strdup(name);
	cmd->desc = ft_strdup(desc);
	cmd->opts = ft_htbl_create(LIBFT_HT_SIZE);

	for (size_t i = 0; i < arr_size; i++) {
		t_ap_opt_def opt_def = opt_def_arr[i];
		t_ap_opt *opt = NULL;
		if (opt_def.key == NULL) {
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
		opt->name = ft_strdup(opt_def.key);
		opt->value = NULL;
		opt->pos = 0;
		opt->set = 0;
		ft_htbl_add_with_f_del(cmd->opts, opt, opt->name, __func_del_opt);
	}

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