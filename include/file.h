#ifndef SSL_FILE_H
# define SSL_FILE_H

# include <common.h>
# include <io.h>
#include <libft.h>

typedef struct s_file {
    char *path;
} t_file;

int file_read_all(const char *path, t_ostring *ostring);
int file_write_all(const char *path, t_ostring *ostring);

#endif
