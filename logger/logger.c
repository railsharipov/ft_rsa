#include <common.h>
#include <libft/logger.h>
#include <libft/htable.h>

static t_logger *__default_logger = NULL;
static t_htbl	*__loggers = NULL;

int	logger_log(const char *logger_name, const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...)
{
	t_logger *logger;
	va_list	va_arg;
	int		ret;

	if (NULL == logger_name) {
		logger = __default_logger;
	} else {
		logger = logger_get_logger(logger_name);
	}
	if (NULL == logger) {
		return (-1);
	}
	va_start(va_arg, fmt);
	ret = ft_logger_va_log(func_name, file_name, line_number, logger, level, fmt, va_arg);
	va_end(va_arg);

	return (ret);
}

int	logger_va_log(const char *logger_name, const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, va_list va_arg)
{
	t_logger *logger;

	if (NULL == logger_name) {
		logger = __default_logger;
	} else {
		logger = logger_get_logger(logger_name);
	}
	if (NULL == logger) {
		return (-1);
	}
	return (ft_logger_va_log(func_name, file_name, line_number, logger, level, fmt, va_arg));
}

t_logger *logger_get_default_logger(void)
{
	return (__default_logger);
}

int logger_set_default_logger(t_logger *logger)
{
	if (NULL == logger) {
		return (-1);
	}
	__default_logger = logger;

	return (0);
}

t_logger *logger_add_logger(const char *logger_name, t_logger *logger)
{
	if (NULL == logger) {
		return (NULL);
	}
	if (NULL == logger_name) {
		return (NULL);
	}
	if (NULL == __loggers) {
		__loggers = ft_htbl_create(LIBFT_HT_SIZE);
		if (NULL == __loggers) {
			return (NULL);
		}
	}
	if (ft_htbl_has(__loggers, logger_name)) {
		return (NULL);
	}
	ft_htbl_add(__loggers, logger, logger_name);

	return (logger);
}

t_logger *logger_get_logger(const char *logger_name)
{
	if (NULL == __loggers) {
		return (NULL);
	}
	if (NULL == logger_name) {
		return (NULL);
	}
	return (ft_htbl_get(__loggers, logger_name));
}
