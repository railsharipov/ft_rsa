#include <unistd.h>
#include <sys/fcntl.h>
#include <libft/logger.h>
#include <alloc.h>
#include <common.h>

#define RAND_LOG_PREFIX  "rand: "

static int __f_logger(const char *mes);

static t_logger __logger = {
    .f_logger = __f_logger,
    .log_level = LIBFT_LOG_LEVEL_INFO,
    .ansi_colored = 1,
};

static int  __f_logger(const char *mes) {
    ft_printf("%@%s\n", mes);
    return (SSL_OK);
}

int rand_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...) {
	va_list	va_arg;
	char	*prefixed_fmt;
	char	*level_prefix;
	int		ret;

	if (level > ft_logger_get_level()) {
		return (0);
	}
	va_start(va_arg, fmt);

    prefixed_fmt = ft_strjoin(RAND_LOG_PREFIX, fmt);

	ret = ft_logger_va_log_new(func_name, file_name, line_number, &__logger, level, prefixed_fmt, va_arg);

	SSL_FREE(prefixed_fmt);
	va_end(va_arg);

	return (ret);
}
