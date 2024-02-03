#include <stdarg.h>
#include <libft/htable.h>
#include <libft/string.h>
#include <libft/error.h>

static FUNC_ERR_LOGGER	__f_libft_error_logger;
static uint8_t			__f_libft_error_level;

FUNC_ERR_LOGGER	ft_error_get_logger(void)
{
	return (__f_libft_error_logger);
}

void	ft_error_set_logger(FUNC_ERR_LOGGER f_logger)
{
	__f_libft_error_logger = f_logger;
}

void	ft_error_set_level(uint8_t level)
{
	if (level < LIBFT_NUM_ERROR_LEVELS) {
		__f_libft_error_level = level;
	} else {
		__f_libft_error_level = LIBFT_ERROR_LEVEL_INFO;
	}
}

int	ft_error_log(const char *func_name, const char *file_name, int line_number, const char *fmt, ...)
{
	va_list	va_arg;
	int		ret;

	va_start(va_arg, fmt);

	ret = ft_verror_log(func_name, file_name, line_number, fmt, va_arg);

	va_end(va_arg);

	return (ret);
}

int ft_error_log_info(const char *fmt, ...)
{
	va_list	va_arg;
	int		ret;

	va_start(va_arg, fmt);

	ret = ft_verror_log_info(fmt, va_arg);

	va_end(va_arg);

	return (ret);
}

int	ft_error_log_debug(const char *func_name, const char *file_name, int line_number, const char *fmt, ...)
{
	va_list	va_arg;
	int		ret;

	va_start(va_arg, fmt);

	ret = ft_verror_log_debug(func_name, file_name, line_number, fmt, va_arg);

	va_end(va_arg);

	return (ret);
}

int	ft_verror_log(const char *func_name, const char *file_name, int line_number, const char *fmt, va_list va_arg)
{
	int		ret;

	if (__f_libft_error_level == LIBFT_ERROR_LEVEL_DEBUG) {
		ret = ft_verror_log_debug(func_name, file_name, line_number, fmt, va_arg);
	} else {
		ret = ft_verror_log_info(fmt, va_arg);
	}
	return (ret);
}

int	ft_verror_log_info(const char *fmt, va_list va_arg)
{
	char	*full_mes;
	char	*mes;
	int		ret;

	if (NULL == __f_libft_error_logger) {
		return (0);
	}
	mes = NULL;

	if (NULL != fmt) {
		ft_vsprintf(&mes, fmt, va_arg);
	} else {
		mes = ft_strdup("error");
	}
	if (errno) {
		full_mes = ft_strjoin_multi(3, mes, ": ", strerror(errno));
	} else {
		full_mes = ft_strdup(mes);
	}
	ret = __f_libft_error_logger(full_mes);

	LIBFT_FREE(full_mes);
	LIBFT_FREE(mes);

	errno = 0;

	return (ret);
}

int	ft_verror_log_debug(const char *func_name, const char *file_name, int line_number, const char *fmt, va_list va_arg)
{
	char	*full_mes;
	char	*debug_mes;
	char	*mes;
	int		ret;

	if (NULL == __f_libft_error_logger) {
		return (0);
	}
	mes = NULL;
	debug_mes = NULL;

	if (NULL == func_name && NULL == file_name) {
		debug_mes = ft_strdup("");
	} else {
		ft_sprintf(&debug_mes, "%s, %s:%d: ", func_name, file_name, line_number);
	}
	if (NULL != fmt) {
		ft_vsprintf(&mes, fmt, va_arg);
	} else {
		mes = ft_strdup("unspecified error");
	}
	if (errno) {
		full_mes = ft_strjoin_multi(4, debug_mes, mes, ": ", strerror(errno));
	} else {
		full_mes = ft_strjoin_multi(2, debug_mes, mes);
	}
	ret = __f_libft_error_logger(full_mes);

	LIBFT_FREE(debug_mes);
	LIBFT_FREE(full_mes);
	LIBFT_FREE(mes);

	errno = 0;

	return (ret);
}