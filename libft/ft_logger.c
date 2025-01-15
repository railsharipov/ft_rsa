#include <stdarg.h>
#include <libft/htable.h>
#include <libft/string.h>
#include <libft/logger.h>
#include <libft/common.h>

#define FT_LOGGER_CRIT_LOG_PREFIX	"[crit]  "
#define FT_LOGGER_ERROR_LOG_PREFIX	"[error] "
#define FT_LOGGER_WARN_LOG_PREFIX	"[warn]  "
#define FT_LOGGER_INFO_LOG_PREFIX	"[info]  "
#define FT_LOGGER_DEBUG_LOG_PREFIX	"[debug] "
#define FT_LOGGER_TRACE_LOG_PREFIX	"[trace] "

static int __log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, va_list va_arg);

int	ft_logger_log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, ...)
{
	va_list	va_arg;
	int		ret;

	if (NULL == logger || NULL == logger->f_logger || NULL == fmt) {
		return (0);
	}
	if (level > logger->log_level) {
		return (0);
	}

	va_start(va_arg, fmt);

	ret = __log(func_name, file_name, line_number, logger, level, fmt, va_arg);

	va_end(va_arg);

	return (ret);
}

int	ft_logger_va_log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, va_list va_arg)
{
	if (NULL == logger || NULL == logger->f_logger || NULL == fmt) {
		return (0);
	}
	if (level > logger->log_level) {
		return (0);
	}
	return (__log(func_name, file_name, line_number, logger, level, fmt, va_arg));
}

static int	__log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, va_list va_arg)
{
	char	*full_mes;
	char	*debug_info;
	char	*level_prefix;
	char	*mes;
	int		colored;
	int		ret;

	debug_info = NULL;
	mes = NULL;
	colored = logger->ansi_colored;

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

	if (NULL == func_name && NULL == file_name) {
		debug_info = ft_strdup("");
	} else {
		if (colored) {
			ft_sprintf(&debug_info, TXT_YELL(" (%s, %s:%d)"), func_name, file_name, line_number);
		} else {
			ft_sprintf(&debug_info, " (%s, %s:%d)", func_name, file_name, line_number);
		}
	}

	ft_vsprintf(&mes, fmt, va_arg);

	full_mes = ft_strjoin_multi(3, level_prefix, mes, debug_info);

	ret = logger->f_logger(full_mes);

	LIBFT_FREE(debug_info);
	LIBFT_FREE(full_mes);
	LIBFT_FREE(mes);

	return (ret);
}