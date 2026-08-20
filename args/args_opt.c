#include <common.h>
#include <logger.h>
#include <args.h>
#include <libft.h>

t_arg_opt	*args_new_opt(const char *name, const char *desc, t_opt_type type)
{
	t_arg_opt *opt_arg = NULL;

	if (name == NULL) {
		SSL_LOG(ERROR, "option name is not set");
		return (NULL);
	}
	switch (type) {
		case AP_OPT_TYPE_FLAG:
		case AP_OPT_TYPE_STRING:
		case AP_OPT_TYPE_NUMBER:
			break;
		default:
			SSL_LOG(ERROR, "unknown option type: %d", type);
			return (NULL);
	}
	SSL_ALLOC(opt_arg, sizeof(t_arg_opt));
	opt_arg->type = type;
	opt_arg->name = ft_strdup(name);
	opt_arg->desc = ft_strdup(desc);

	return (opt_arg);
}

void	args_del_opt(t_arg_opt *opt_arg)
{
	if (opt_arg == NULL) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return;
	}
	SSL_FREE(opt_arg->name);
	SSL_FREE(opt_arg->desc);
	SSL_FREE(opt_arg);
}

t_arg_opt	*args_copy_opt(const t_arg_opt *opt_arg)
{
	t_arg_opt *copy;

	if (opt_arg == NULL) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	copy = args_new_opt(opt_arg->name, opt_arg->desc, opt_arg->type);

	return (copy);
}
