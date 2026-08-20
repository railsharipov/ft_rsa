#include <common.h>
#include <logger.h>
#include <libft.h>

static int __log_writer(const char *mes) {
	return (ft_printf("%@%s\n", mes));
}

static t_logger __logger = {
	.log_writer = __log_writer,
	.log_level_thres = LIBFT_LOG_LEVEL_INFO,
	.debug_info_thres = LIBFT_LOG_LEVEL_DEBUG,
	.is_ansi_colored = 1,
};

t_logger *logger_get_logger(void)
{
	return (&__logger);
}
