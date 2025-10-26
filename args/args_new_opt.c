#include <common.h>
#include <args.h>
#include <libft/htable.h>

t_arg_opt	*args_new_opt(const char *name, const char *desc, t_opt_type type)
{
	t_arg_opt *opt_arg = NULL;

	if (name == NULL) {
		ARGP_LOG(ERROR, "option name is not set");
		return (NULL);
	}
	switch (type) {
		case AP_OPT_TYPE_FLAG:
		case AP_OPT_TYPE_STRING:
		case AP_OPT_TYPE_NUMBER:
			break;
		default:
			ARGP_LOG(ERROR, "unknown option type: %d", type);
			return (NULL);
	}
	SSL_ALLOC(opt_arg, sizeof(t_arg_opt));
	opt_arg->type = type;
	opt_arg->name = ft_strdup(name);
	opt_arg->desc = ft_strdup(desc);
	opt_arg->value = NULL;
	opt_arg->pos = 0;
	opt_arg->set = 0;

	return (opt_arg);
}