#ifndef SSL_LOGGER_H
# define SSL_LOGGER_H

#include <libft.h>

# define SSL_LOG(LEVEL, FMT, ...)	ft_log_log(__func__, __FILE__, __LINE__, logger_get_logger(), LIBFT_LOG_LEVEL_##LEVEL, FMT __VA_OPT__(,) __VA_ARGS__)

t_logger	*logger_get_logger(void);

#endif
