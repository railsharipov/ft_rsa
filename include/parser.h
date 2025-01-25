#ifndef PARSER_H
# define PARSER_H

# include <string.h>
# include <common.h>
# include <libft/logger.h>

# define RK_PRIME 101
# define RK_ALPHA 256

# define PARSER_LOG(LEVEL, MES, ...)	parser_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

int		parser_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int		parser_find(const char *octets, int olen, const char *pattern, int patlen);
void	parser_del_blank(const char *arr, int asize, char **p, int *psize);
void	parser_del_wspace(const char *arr, int asize, char **p, int *psize);
void	parser_del_eolws(const char *arr, int asize, char **p, int *psize);
void	parser_del_empty_lines(const char *arr, int asize, char **p, int *psize);
char	*parser_line(const char *arr, int asize, const char *pattern, int patlen);
void	parser_insert_delim(char **arr, size_t *asize, int delim, int step);

#endif
