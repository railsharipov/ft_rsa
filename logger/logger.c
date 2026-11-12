#include <common.h>
#include <logger.h>
#include <libft.h>

static int __log_writer(const char *mes) {
	return (ft_fprintf(2, "%s\n", mes));
}

static t_logger __logger = {
	.log_writer = __log_writer,
	.log_level_thres = LIBFT_LOG_LEVEL_INFO,
	.debug_info_thres = LIBFT_LOG_LEVEL_DEBUG,
	.is_ansi_colored = 1,
};

t_logger *logger_get_logger(void)			{ return (&__logger); }
t_logger_level logger_get_log_level(void)	{ return (__logger.log_level_thres); }

void logger_set_log_level(t_logger_level level)		{ __logger.log_level_thres = level; }

void logger_set_log_level_always(void)	{ __logger.log_level_thres = LIBFT_LOG_LEVEL_ALWAYS; }
void logger_set_log_level_crit(void)	{ __logger.log_level_thres = LIBFT_LOG_LEVEL_CRIT; }
void logger_set_log_level_error(void)	{ __logger.log_level_thres = LIBFT_LOG_LEVEL_ERROR; }
void logger_set_log_level_warn(void)	{ __logger.log_level_thres = LIBFT_LOG_LEVEL_WARN; }
void logger_set_log_level_info(void)	{ __logger.log_level_thres = LIBFT_LOG_LEVEL_INFO; }
void logger_set_log_level_debug(void)	{ __logger.log_level_thres = LIBFT_LOG_LEVEL_DEBUG; }
void logger_set_log_level_trace(void)	{ __logger.log_level_thres = LIBFT_LOG_LEVEL_TRACE; }
