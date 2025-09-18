#include <common.h>
#include <args.h>
#include <libft/htable.h>

t_args_opt	*args_new_opt(const char *name, const char *desc, t_args_type type)
{
	t_args_opt *opt = NULL;

	if (name == NULL) {
		ARGP_LOG(ERROR, "option name is not set");
		return (NULL);
	}
	switch (type) {
		case AP_ARG_TYPE_FLAG:
		case AP_ARG_TYPE_STRING:
		case AP_ARG_TYPE_NUMBER:
			break;
		default:
			ARGP_LOG(ERROR, "unknown option type: %d", type);
			return (NULL);
	}
	SSL_ALLOC(opt, sizeof(t_args_opt));
	opt->type = type;
	opt->name = ft_strdup(name);
	opt->desc = ft_strdup(desc);
	opt->value = NULL;
	opt->pos = 0;
	opt->set = 0;

	return (opt);
}