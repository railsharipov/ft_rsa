#ifndef SSL_FILE_H
# define SSL_FILE_H

# include <common.h>
# include <io.h>
# include <libft/string.h>
# include <libft/logger.h>

# define FILE_LOG(LEVEL, MES, ...)	file_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

typedef struct s_file {
    const char *path;
} t_file;

int file_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int file_stream(t_io_v2_stream **stream, const t_file *file, t_io_v2_flag flags);
int file_reader(t_io_v2_stream **stream, const t_file *file);
int file_writer(t_io_v2_stream **stream, const t_file *file);

#endif