#include <ft_ssl.h>
#include <ssl_error.h>

static int	__error_is_set = SSL_FALSE;

int error_dump_error(
	int error, const char *func_name, const char *file_name, int line_number)
{
	const char	*message;

	message = error_to_string(error);

	if (errno && !__error_is_set)
	{
		ft_printf("%@%s, %s:%d: ", func_name, file_name, line_number);
		perror(NULL);
	}
	else if (GET_ERROR(error) == UNSPECIFIED_ERROR)
	{
		if (__error_is_set)
			ft_printf("%@	in %s, %s:%d\n", func_name, file_name, line_number);
		else
			ft_printf("%@%s, %s:%d: error\n", func_name, file_name, line_number);
	}
	else
	{
		if (NULL == message)
			ft_printf("%@%s, %s:%d: error\n", func_name, file_name, line_number);
		else
			ft_printf("%@%s, %s:%d: %s\n", func_name, file_name, line_number, message);
	}
	__error_is_set = SSL_TRUE;

	return (UNSPECIFIED_ERROR);
}
