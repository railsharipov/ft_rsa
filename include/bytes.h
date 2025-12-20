#ifndef SSL_BYTES_H
# define SSL_BYTES_H

# include <common.h>
# include <libft/string.h>
# include <libft/logger.h>

# define BYTES_LOG(LEVEL, MES, ...)	bytes_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

typedef struct s_bytes
{
	t_ostring	*ostring;
	ssize_t		seek;
}				t_bytes;

struct  s_io_stream;
enum	e_io_mode;

int		bytes_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int		bytes_new(t_bytes **bytes, t_ostring *ostring);
void	bytes_del(t_bytes *bytes);
int		bytes_stream(struct s_io_stream **stream, t_bytes *bytes, enum e_io_mode mode);
int		bytes_reader(struct s_io_stream **stream, t_bytes *bytes);
int		bytes_writer(struct s_io_stream **stream, t_bytes *bytes);

#endif
