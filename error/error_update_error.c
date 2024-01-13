#include <stdio.h>
#include <ssl/ssl.h>
#include <ssl/error.h>

static int	__error_is_set = SSL_FALSE;

int error_update_error(int error)
{
	const char	*message;
	const char	*ctx_name;

	message = error_to_string(error);
	ctx_name = error_ctx_to_string(error);

	if (errno && !__error_is_set)
	{
		ft_printf("%@%s error: ", ctx_name);
		perror(NULL);
	}
	else if (!__error_is_set)
	{
		if (NULL == message)
			ft_printf("%@%s error\n", ctx_name);
		else
			ft_printf("%@%s error: %s\n", ctx_name, message);
	}
	__error_is_set = SSL_TRUE;

	return (UNSPECIFIED_ERROR);
}
