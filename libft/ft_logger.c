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

static FUNC_LOGGER	__f_libft_error_logger;
static uint8_t		__log_level;
static uint8_t		__ansi_colored;

FUNC_LOGGER	ft_logger_get_logger(void)
{
	return (__f_libft_error_logger);
}

void	ft_logger_set_logger(FUNC_LOGGER f_logger)
{
	__f_libft_error_logger = f_logger;
}

void	ft_logger_enable_ansi_color(void)
{
	__ansi_colored = 1;
}

void	ft_logger_disable_ansi_color(void)
{
	__ansi_colored = 0;
}

void	ft_logger_set_level(uint8_t level)
{
	if (level < LIBFT_NUM_LOG_LEVELS) {
		__log_level = level;
	} else {
		__log_level = LIBFT_LOG_LEVEL_INFO;
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
	char	*level_prefix;
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
	if (level > __log_level) {
		return (0);
	}

	if (level == LIBFT_LOG_LEVEL_CRIT) {
		level_prefix = __ansi_colored ? TXT_MAGEN(FT_LOGGER_CRIT_LOG_PREFIX) : FT_LOGGER_CRIT_LOG_PREFIX;
	} else if (level == LIBFT_LOG_LEVEL_ERROR) {
		level_prefix = __ansi_colored ? TXT_B_RED(FT_LOGGER_ERROR_LOG_PREFIX) : FT_LOGGER_ERROR_LOG_PREFIX;
	} else if (level == LIBFT_LOG_LEVEL_WARN) {
		level_prefix = __ansi_colored ? TXT_YELL(FT_LOGGER_WARN_LOG_PREFIX) : FT_LOGGER_WARN_LOG_PREFIX;
	} else if (level == LIBFT_LOG_LEVEL_DEBUG) {
		level_prefix = __ansi_colored ? TXT_BLUE(FT_LOGGER_DEBUG_LOG_PREFIX) : FT_LOGGER_DEBUG_LOG_PREFIX;
	} else if (level == LIBFT_LOG_LEVEL_TRACE) {
		level_prefix = __ansi_colored ? TXT_CYAN(FT_LOGGER_TRACE_LOG_PREFIX) : FT_LOGGER_TRACE_LOG_PREFIX;
	} else {
		level_prefix = FT_LOGGER_INFO_LOG_PREFIX;
	}

	if (NULL == func_name && NULL == file_name) {
		debug_info = ft_strdup("");
	} else {
		if (__ansi_colored) {
			ft_sprintf(&debug_info, TXT_YELL(" (%s, %s:%d)"), func_name, file_name, line_number);
		} else {
			ft_sprintf(&debug_info, " (%s, %s:%d)", func_name, file_name, line_number);
		}
	}

	ft_vsprintf(&mes, fmt, va_arg);

	full_mes = ft_strjoin_multi(3, level_prefix, mes, debug_info);

	ret = __f_libft_error_logger(full_mes);

	LIBFT_FREE(debug_info);
	LIBFT_FREE(full_mes);
	LIBFT_FREE(mes);

	return (ret);
}