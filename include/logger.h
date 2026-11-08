#ifndef SSL_LOGGER_H
# define SSL_LOGGER_H

#include <libft.h>

#define SSL_LOG(LEVEL, FMT, ...) ft_log_log(__func__, __FILE__, __LINE__, logger_get_logger(), LIBFT_LOG_LEVEL_##LEVEL, FMT __VA_OPT__(,) __VA_ARGS__)

t_logger		*logger_get_logger(void);
t_logger_level	logger_get_log_level(void);

void	logger_set_log_level(t_logger_level level);
void	logger_set_log_level_always(void);
void	logger_set_log_level_crit(void);
void	logger_set_log_level_error(void);
void	logger_set_log_level_warn(void);
void	logger_set_log_level_info(void);
void	logger_set_log_level_debug(void);
void	logger_set_log_level_trace(void);

#endif
