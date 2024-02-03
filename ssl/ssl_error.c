#include <stdarg.h>
#include <libft/string.h>
#include <libft/error.h>
#include <ssl/alloc.h>

FUNC_ERR_LOGGER	ssl_error_get_logger(void)
{
	return (ft_error_get_logger());
}

void	ssl_error_set_logger(FUNC_ERR_LOGGER f_logger)
{
	ft_error_set_logger(f_logger);
}

void	ssl_error_set_level(uint8_t level)
{
	ft_error_set_level(level);
}

int	ssl_error_log(const char *func_name, const char *file_name, int line_number, const char *fmt_prefix, const char *fmt, ...)
{
	va_list	va_arg;
	char	*prefixed_fmt;
	int		ret;

	va_start(va_arg, fmt);
	prefixed_fmt = ft_strjoin(fmt_prefix, fmt);

	ret = ft_verror_log(func_name, file_name, line_number, fmt, va_arg);

	SSL_FREE(prefixed_fmt);
	va_end(va_arg);

	return (ret);
}