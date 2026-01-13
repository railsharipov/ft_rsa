#ifndef SSL_BYTES_H
# define SSL_BYTES_H

# include <common.h>
# include <io.h>
# include <libft/string.h>
# include <libft/logger.h>

# define BYTES_LOG(LEVEL, MES, ...)	bytes_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

int bytes_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int bytes_stream(t_io_v2_stream **stream, const t_ostring *ostring, t_io_v2_flag flags);
int bytes_reader(t_io_v2_stream **stream, const t_ostring *ostring);
int bytes_writer(t_io_v2_stream **stream, const t_ostring *ostring);

#endif
