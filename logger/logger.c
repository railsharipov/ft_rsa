#include <common.h>
#include <libft/logger.h>

static int __log_writer(const char *mes) {
	return (ft_printf("%@%s\n", mes));
}

static t_logger __logger = {
	.log_writer = __log_writer,
	.log_level_thres = LIBFT_LOG_LEVEL_INFO,
	.is_ansi_colored = 1,
	.debug_info_thres = LIBFT_LOG_LEVEL_WARN,
};

int	logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...)
{
	va_list	va_arg;
	int		ret;

	va_start(va_arg, fmt);
	ret = ft_logger_va_log(func_name, file_name, line_number, &__logger, level, fmt, va_arg);
	va_end(va_arg);

	return (ret);
}

t_logger *logger_get_logger(void) {
	return (&__logger);
}