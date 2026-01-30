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

static const char *__lvl_prefixes[] = {
	[LIBFT_LOG_LEVEL_CRIT] = FT_LOGGER_CRIT_LOG_PREFIX,
	[LIBFT_LOG_LEVEL_ERROR] = FT_LOGGER_ERROR_LOG_PREFIX,
	[LIBFT_LOG_LEVEL_WARN] = FT_LOGGER_WARN_LOG_PREFIX,
	[LIBFT_LOG_LEVEL_INFO] = FT_LOGGER_INFO_LOG_PREFIX,
	[LIBFT_LOG_LEVEL_DEBUG] = FT_LOGGER_DEBUG_LOG_PREFIX,
	[LIBFT_LOG_LEVEL_TRACE] = FT_LOGGER_TRACE_LOG_PREFIX,
};
static const size_t __lvl_prefixes_size = sizeof(__lvl_prefixes) / sizeof(__lvl_prefixes[0]);

static const char *__colored_lvl_prefixes[] = {
	[LIBFT_LOG_LEVEL_CRIT] = TXT_MAGEN(FT_LOGGER_CRIT_LOG_PREFIX),
	[LIBFT_LOG_LEVEL_ERROR] = TXT_B_RED(FT_LOGGER_ERROR_LOG_PREFIX),
	[LIBFT_LOG_LEVEL_WARN] = TXT_YELL(FT_LOGGER_WARN_LOG_PREFIX),
	[LIBFT_LOG_LEVEL_INFO] = TXT_BLUE(FT_LOGGER_INFO_LOG_PREFIX),
	[LIBFT_LOG_LEVEL_DEBUG] = TXT_CYAN(FT_LOGGER_DEBUG_LOG_PREFIX),
	[LIBFT_LOG_LEVEL_TRACE] = TXT_CYAN(FT_LOGGER_TRACE_LOG_PREFIX),
};
static const size_t __colored_lvl_prefixes_size = sizeof(__colored_lvl_prefixes) / sizeof(__colored_lvl_prefixes[0]);

#define __DEBUG_MSG(TXT, COLORED)	(COLORED ? TXT_YELL(TXT) : TXT)
#define __LVL_PREFIX(LVL, COLORED)	(COLORED ? __colored_lvl_prefixes[LVL%__colored_lvl_prefixes_size] : __lvl_prefixes[LVL%__lvl_prefixes_size])

static int __log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, va_list va_arg);

int	ft_logger_log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, ...)
{
	va_list	va_arg;
	int		ret;

	if (NULL == logger || NULL == logger->log_writer || NULL == fmt) {
		return (0);
	}
	if (level > logger->log_level_thres) {
		return (0);
	}
	va_start(va_arg, fmt);

	ret = __log(func_name, file_name, line_number, logger, level, fmt, va_arg);

	va_end(va_arg);

	return (ret);
}

int	ft_logger_va_log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, va_list va_arg)
{
	if (NULL == logger || NULL == logger->log_writer || NULL == fmt) {
		return (0);
	}
	if (level > logger->log_level_thres) {
		return (0);
	}
	return (__log(func_name, file_name, line_number, logger, level, fmt, va_arg));
}

static int	__log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, va_list va_arg)
{
	const char	*lvl_prefix;
	char	*full_mes;
	char	*debug_mes;
	char	*mes;
	int		ret;

	lvl_prefix = __LVL_PREFIX(level, logger->is_ansi_colored);
	debug_mes = NULL;
	mes = NULL;

	if (level <= logger->debug_info_thres) {
		if (NULL != file_name) {
			ft_sprintf(&debug_mes, __DEBUG_MSG(" (%s:%d)", logger->is_ansi_colored), file_name, line_number);
		} else {
			debug_mes = ft_strdup("");
		}
	}

	ft_vsprintf(&mes, fmt, va_arg);

	if (NULL != func_name) {
		full_mes = ft_strjoin_multi(5, lvl_prefix, func_name, ": ", mes, debug_mes);
	} else {
		full_mes = ft_strjoin_multi(4, lvl_prefix, mes, debug_mes);
	}
	ret = logger->log_writer(full_mes);

	LIBFT_FREE(debug_mes);
	LIBFT_FREE(full_mes);
	LIBFT_FREE(mes);

	return (ret);
}
