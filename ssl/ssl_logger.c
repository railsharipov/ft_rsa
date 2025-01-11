#include <stdarg.h>
#include <libft/string.h>
#include <libft/logger.h>
#include <ssl/alloc.h>

FUNC_LOGGER	ssl_logger_get_logger(void)
{
	return (ft_logger_get_logger());
}

void	ssl_logger_set_logger(FUNC_LOGGER f_logger)
{
	ft_logger_set_logger(f_logger);
}

void	ssl_logger_set_level(uint8_t level)
{
	ft_logger_set_level(level);
}

void	ssl_logger_enable_ansi_color(void)
{
	ft_logger_enable_ansi_color();
}

void	ssl_logger_disable_ansi_color(void)
{
	ft_logger_disable_ansi_color();
}

int	ssl_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt_prefix, const char *fmt, ...)
{
	va_list	va_arg;
	char	*prefixed_fmt;
	char	*level_prefix;
	int		ret;

	va_start(va_arg, fmt);

	if (NULL != fmt_prefix) {
		prefixed_fmt = ft_strjoin(fmt_prefix, fmt);
	} else {
		prefixed_fmt = ft_strdup(fmt);
	}

	ret = ft_logger_va_log(func_name, file_name, line_number, level, prefixed_fmt, va_arg);

	SSL_FREE(prefixed_fmt);
	va_end(va_arg);

	return (ret);
}