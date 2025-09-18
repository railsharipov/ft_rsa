#include <common.h>
#include <args.h>
#include <libft/htable.h>

int	args_init(t_args *args)
{
	if (args == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return SSL_ERR;
	}
	ft_bzero(args, sizeof(t_args));

	args->htbl = ft_htbl_create(LIBFT_HT_SIZE);

	return SSL_OK;
}