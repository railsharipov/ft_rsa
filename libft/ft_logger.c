#include <stdarg.h>
#include <libft/htable.h>
#include <libft/string.h>
#include <libft/logger.h>
#include <libft/common.h>

#define FT_LOGGER_CRIT_LOG_PREFIX	"[crit ] "
#define FT_LOGGER_ERROR_LOG_PREFIX	"[error] "
#define FT_LOGGER_WARN_LOG_PREFIX	"[warn ] "
#define FT_LOGGER_INFO_LOG_PREFIX	"[info ] "
#define FT_LOGGER_DEBUG_LOG_PREFIX	"[debug] "
#define FT_LOGGER_TRACE_LOG_PREFIX	"[trace] "

#define __DEBUG_INFO_MSG(TXT, COLORED)		(COLORED ? TXT_YELL(TXT) : TXT)

static t_logger __noop_logger = {
	.log_writer = NULL,
	.log_level = LIBFT_LOG_LEVEL_INFO,
	.is_ansi_colored = 1,
	.debug_info_thres = LIBFT_LOG_LEVEL_WARN,
};

static t_logger *__default_logger = &__noop_logger;

static int __log(const char *func_name, const char *file_name, int line_number, t_logger *logger, const char *logger_name, uint8_t level, const char *fmt, va_list va_arg);

int	ft_logger_log(const char *func_name, const char *file_name, int line_number, t_logger *logger, const char *logger_name, uint8_t level, const char *fmt, ...)
{
	va_list	va_arg;
	int		ret;

	if (NULL == logger) {
		logger = __default_logger;
	}
	if (NULL == logger || NULL == logger->log_writer || NULL == fmt) {
		return (0);
	}
	if (level > logger->log_level) {
		return (0);
	}
	va_start(va_arg, fmt);

	ret = __log(func_name, file_name, line_number, logger, logger_name, level, fmt, va_arg);

	va_end(va_arg);

	return (ret);
}

int	ft_logger_va_log(const char *func_name, const char *file_name, int line_number, t_logger *logger, const char *logger_name, uint8_t level, const char *fmt, va_list va_arg)
{
	if (NULL == logger) {
		logger = __default_logger;
	}
	if (NULL == logger || NULL == logger->log_writer || NULL == fmt) {
		return (0);
	}
	if (level > logger->log_level) {
		return (0);
	}
	return (__log(func_name, file_name, line_number, logger, logger_name, level, fmt, va_arg));
}

static int	__log(const char *func_name, const char *file_name, int line_number, t_logger *logger, const char *logger_name, uint8_t level, const char *fmt, va_list va_arg)
{
	char	*full_mes;
	char	*debug_info;
	char	*level_prefix;
	char	*mes;
	int		colored, debug_info_thres;
	int		ret;

	debug_info = NULL;
	mes = NULL;
	colored = logger->is_ansi_colored;
	debug_info_thres = logger->debug_info_thres;

	if (level == LIBFT_LOG_LEVEL_CRIT) {
		level_prefix = colored ? TXT_MAGEN(FT_LOGGER_CRIT_LOG_PREFIX) : FT_LOGGER_CRIT_LOG_PREFIX;
	} else if (level == LIBFT_LOG_LEVEL_ERROR) {
		level_prefix = colored ? TXT_B_RED(FT_LOGGER_ERROR_LOG_PREFIX) : FT_LOGGER_ERROR_LOG_PREFIX;
	} else if (level == LIBFT_LOG_LEVEL_WARN) {
		level_prefix = colored ? TXT_YELL(FT_LOGGER_WARN_LOG_PREFIX) : FT_LOGGER_WARN_LOG_PREFIX;
	} else if (level == LIBFT_LOG_LEVEL_DEBUG) {
		level_prefix = colored ? TXT_BLUE(FT_LOGGER_DEBUG_LOG_PREFIX) : FT_LOGGER_DEBUG_LOG_PREFIX;
	} else if (level == LIBFT_LOG_LEVEL_TRACE) {
		level_prefix = colored ? TXT_CYAN(FT_LOGGER_TRACE_LOG_PREFIX) : FT_LOGGER_TRACE_LOG_PREFIX;
	} else {
		level_prefix = FT_LOGGER_INFO_LOG_PREFIX;
	}

	if (level <= debug_info_thres) {
		if (NULL != func_name && NULL != file_name) {
			ft_sprintf(&debug_info, __DEBUG_INFO_MSG(" (%s, %s:%d)", colored), func_name, file_name, line_number);
		} else if (NULL != func_name) {
			ft_sprintf(&debug_info, __DEBUG_INFO_MSG(" (%s)", colored), func_name);
		} else if (NULL != file_name) {
			ft_sprintf(&debug_info, __DEBUG_INFO_MSG(" (%s:%d)", colored), file_name, line_number);
		} else {
			debug_info = ft_strdup("");
		}
	}

	ft_vsprintf(&mes, fmt, va_arg);

	if (NULL != logger_name) {
		full_mes = ft_strjoin_multi(5, level_prefix, logger_name, ": ", mes, debug_info);
	} else {
		full_mes = ft_strjoin_multi(4, level_prefix, mes, debug_info);
	}

	ret = logger->log_writer(full_mes);

	LIBFT_FREE(debug_info);
	LIBFT_FREE(full_mes);
	LIBFT_FREE(mes);

	return (ret);
}

void ft_logger_set_default_logger(t_logger *logger) {
	__default_logger = logger;
}

t_logger *ft_logger_get_default_logger(void) {
	return (__default_logger);
}