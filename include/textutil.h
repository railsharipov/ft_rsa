#ifndef TEXTUTIL_H
# define TEXTUTIL_H

# include <string.h>
# include <common.h>
# include <libft/logger.h>

# define RK_PRIME 101
# define RK_ALPHA 256

# define TEXTUTIL_LOG(LEVEL, MES, ...)	textutil_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

int		textutil_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int		textutil_find(const char *octets, size_t olen, const char *pattern, size_t patlen);
int		textutil_findf(const char *octets, size_t olen, const char *format, ...);
int		textutil_del_blank(const char *octets, size_t olen, char **p, size_t *psize);
int 	textutil_del_wspace(const char *octets, size_t olen, char **p, size_t *psize);
int 	textutil_del_eolws(const char *octets, size_t olen, char **p, size_t *psize);
int 	textutil_del_eol(const char *octets, size_t olen, char **p, size_t *psize);
int 	textutil_del_empty_lines(const char *octets, size_t olen, char **p, size_t *psize);
int		textutil_insert_delim(char *octets, size_t olen, char **p, size_t *psize, int delim, int step);
int		textutil_seek(const char *octets, size_t olen, const char *pattern, size_t patlen);
int		textutil_seekf(const char *octets, size_t olen, const char *format, ...);
int		textutil_sscanf(const char *octets, size_t olen, const char *format, ...);
int		textutil_sbscanf(const char *octets, size_t olen, const char *format, ...);

#endif
