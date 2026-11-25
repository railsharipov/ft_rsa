#include "libft.h"
#include "libft_v2.h"

static int __log_writer(const char *mes) {
	return (ft_fprintf(&io_v2_stderr_writer, "%s\n", mes));
}

static t_logger __logger = {
	.log_writer = __log_writer,
	.log_level_thres = LIBFT_LOG_LEVEL_INFO,
	.debug_info_thres = LIBFT_LOG_LEVEL_DEBUG,
	.is_ansi_colored = 1,
};

t_logger *libft_get_logger(void)
{
	return (&__logger);
}

void libft_set_log_level(enum e_logger_level level)
{
	__logger.log_level_thres = level;
}

enum e_logger_level libft_get_log_level(void)
{
	return (__logger.log_level_thres);
}
