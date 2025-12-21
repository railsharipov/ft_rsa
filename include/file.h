#ifndef SSL_FILE_H
# define SSL_FILE_H

# include <common.h>
# include <libft/string.h>
# include <libft/logger.h>

# define FILE_LOG(LEVEL, MES, ...)	file_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

typedef struct s_file
{
    char	*path;
	int		fd;
	ssize_t	seek;
}				t_file;

struct  s_io_stream;
enum	e_io_v2_mode;

int		file_new(t_file **file, const char *path);
void	file_del(t_file *file);
int		file_stream(struct s_io_stream **stream, t_file *file, enum e_io_v2_mode mode);
int		file_reader(struct s_io_stream **stream, t_file *file);
int		file_writer(struct s_io_stream **stream, t_file *file);

#endif