#include <stdarg.h>
#include <libft/htable.h>
#include <libft/string.h>
#include <libft/error.h>

static FUNC_ERR_LOGGER	__f_libft_error_logger;
static uint8_t			__f_libft_error_level;

FUNC_ERR_LOGGER	ft_logger_get_logger(void)
{
	return (__f_libft_error_logger);
}

void	ft_logger_set_logger(FUNC_ERR_LOGGER f_logger)
{
	__f_libft_error_logger = f_logger;
}

void	ft_logger_set_level(uint8_t level)
{
	if (level < LIBFT_NUM_ERROR_LEVELS) {
		__f_libft_error_level = level;
	} else {
		__f_libft_error_level = LIBFT_ERROR_LEVEL_INFO;
	}
}

int	ft_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...)
{
	va_list	va_arg;
	int		ret;

	va_start(va_arg, fmt);

	ret = ft_logger_va_log(func_name, file_name, line_number, level, fmt, va_arg);

	va_end(va_arg);

	return (ret);
}

int	ft_logger_va_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, va_list va_arg)
{
	char	*full_mes;
	char	*debug_info;
	char	*mes;
	int		ret;

	debug_info = NULL;
	mes = NULL;

	if (NULL == __f_libft_error_logger) {
		return (0);
	}
	if (NULL == fmt) {
		return (0);
	}
	if (level > __f_libft_error_level) {
		return (0);
	}

	if (NULL == func_name && NULL == file_name) {
		debug_info = ft_strdup("");
	} else {
		ft_sprintf(&debug_info, "%s, %s:%d: ", func_name, file_name, line_number);
	}

	ft_vsprintf(&mes, fmt, va_arg);

	full_mes = ft_strjoin_multi(2, debug_info, mes);

	ret = __f_libft_error_logger(full_mes);

	LIBFT_FREE(debug_info);
	LIBFT_FREE(full_mes);
	LIBFT_FREE(mes);

	return (ret);
}