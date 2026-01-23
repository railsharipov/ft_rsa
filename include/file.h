#ifndef SSL_FILE_H
# define SSL_FILE_H

# include <common.h>
# include <io.h>
# include <libft/string.h>
# include <libft/logger.h>

# define FILE_LOGGER_NAME	"file"
# define FILE_LOG(LEVEL, MES, ...)	ft_logger_log(__func__, __FILE__, __LINE__, NULL, FILE_LOGGER_NAME, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

typedef struct s_file {
    char *path;
} t_file;

int file_reader(t_io_v2_stream **stream, const char *file_path);
int file_writer(t_io_v2_stream **stream, const char *file_path);

int file_read_all(const char *path, t_ostring *ostring);
int file_write_all(const char *path, t_ostring *ostring);

#endif