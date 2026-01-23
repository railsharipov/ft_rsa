#ifndef SSL_BYTES_H
# define SSL_BYTES_H

# include <common.h>
# include <io.h>
# include <libft/string.h>
# include <libft/logger.h>

# define BYTES_LOGGER_NAME	"bytes"
# define BYTES_LOG(LEVEL, MES, ...)	ft_logger_log(__func__, __FILE__, __LINE__, NULL, BYTES_LOGGER_NAME, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

int bytes_reader(t_io_v2_stream **stream, t_ostring *ostring);
int bytes_writer(t_io_v2_stream **stream, t_ostring *ostring);

#endif
