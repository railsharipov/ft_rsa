#include <common.h>
#include <args.h>

t_arg_opt	*args_copy_opt(const t_arg_opt *opt_arg)
{
	t_arg_opt *copy;

	if (opt_arg == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	copy = args_new_opt(opt_arg->name, opt_arg->desc, opt_arg->type);
	copy->value = ft_strdup(opt_arg->value);
	copy->pos = opt_arg->pos;
	copy->set = opt_arg->set;

	return (copy);
}