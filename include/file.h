#ifndef SSL_FILE_H
# define SSL_FILE_H

# include <common.h>
# include <io.h>

typedef struct s_file {
    const char *path;
} t_file;

int file_stream(t_io_v2_stream **stream, const t_file *file, t_io_v2_flag flags);
int file_reader(t_io_v2_stream **stream, const t_file *file);
int file_writer(t_io_v2_stream **stream, const t_file *file);

#endif