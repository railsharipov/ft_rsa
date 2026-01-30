#ifndef TEXTUTIL_H
# define TEXTUTIL_H

# include <string.h>
# include <common.h>
# include <io.h>
# include <libft/logger.h>

# define RK_PRIME 101
# define RK_ALPHA 256

ssize_t	textutil_find(const char *octets, size_t olen, const char *pattern, size_t patlen);
ssize_t	textutil_findf(const char *octets, size_t olen, const char *format, ...);
int		textutil_del_blank(const char *in, size_t inlen, char **out, size_t *outlen);
int		textutil_del_wspace(const char *in, size_t inlen, char **out, size_t *outlen);
int		textutil_del_eolws(const char *in, size_t inlen, char **out, size_t *outlen);
int		textutil_del_eol(const char *in, size_t inlen, char **out, size_t *outlen);
int		textutil_del_empty_lines(const char *in, size_t inlen, char **out, size_t *outlen);
int		textutil_insert_delim(const char *in, size_t inlen, char **out, size_t *outlen, int delim, int step);
ssize_t	textutil_seek(const char *in, size_t inlen, const char *pat, size_t patlen);
ssize_t	textutil_seekf(const char *in, size_t inlen, const char *format, ...);
ssize_t	textutil_bseekf(const char *in, size_t inlen, const char *format, ...);
int		textutil_sscanf(const char *in, size_t inlen, const char *format, ...);
int		textutil_bnscanf(const char *in, size_t inlen, const char *format, ...);
int		textutil_nreplace(const char *in, size_t inlen, char **out, size_t *outlen, const char *pat, size_t patlen, const char *rep, size_t replen);

int     textutil_line_reader(t_io_v2_stream **stream, t_io_v2_stream *upstream, size_t max_line_len);
int     textutil_line_writer(t_io_v2_stream **stream, t_io_v2_stream *upstream, size_t line_len);

#endif
