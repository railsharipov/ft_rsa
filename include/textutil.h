#ifndef TEXTUTIL_H
# define TEXTUTIL_H

# include <string.h>
# include <common.h>
# include <io.h>
# include <libft/logger.h>

# define RK_PRIME 101
# define RK_ALPHA 256

# define TEXTUTIL_LOGGER_NAME	"textutil"
# define TEXTUTIL_LOG(LEVEL, MES, ...)	ft_logger_log(__func__, __FILE__, __LINE__, NULL, TEXTUTIL_LOGGER_NAME, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

int		textutil_find(const char *octets, size_t olen, const char *pattern, size_t patlen);
int		textutil_findf(const char *octets, size_t olen, const char *format, ...);
ssize_t	textutil_del_blank(const char *in, char *out, size_t len);
ssize_t	textutil_del_wspace(const char *in, char *out, size_t len);
ssize_t	textutil_del_eolws(const char *in, char *out, size_t len);
ssize_t	textutil_del_eol(const char *in, char *out, size_t len);
ssize_t	textutil_del_empty_lines(const char *in, char *out, size_t len);
int		textutil_insert_delim(char *octets, size_t olen, char **p, size_t *psize, int delim, int step);
ssize_t	textutil_seek(const char *in, size_t inlen, const char *pat, size_t patlen);
ssize_t	textutil_seekf(const char *in, size_t inlen, const char *format, ...);
ssize_t	textutil_bseekf(const char *in, size_t inlen, const char *format, ...);
int		textutil_sscanf(const char *in, size_t inlen, const char *format, ...);
int		textutil_bnscanf(const char *in, size_t inlen, const char *format, ...);
ssize_t	textutil_nreplace(const char *in, size_t inlen, char **res, const char *pat, size_t patlen, const char *rep, size_t replen);

int     textutil_line_reader(t_io_v2_stream **stream, t_io_v2_stream *upstream, size_t max_line_len);
int     textutil_line_writer(t_io_v2_stream **stream, t_io_v2_stream *upstream, size_t line_len);

#endif
