#ifndef PARSER_H
# define PARSER_H

# include <string.h>
# include <ssl.h>

# define RK_PRIME 101
# define RK_ALPHA 256

# define PARSER_ERROR(MES, ...)	util_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_ERROR, "parser: ", MES __VA_OPT__(,) __VA_ARGS__)

int		parser_find(const char *, int, const char *, int);
void	parser_del_blank(const char *, int, char **, int *);
void	parser_del_wspace(const char *, int, char **, int *);
void	parser_del_eolws(const char *, int, char **, int *);
void	parser_del_empty_lines(const char *, int, char **, int *);
char	*parser_line(const char *, int, const char *, int);
void	parser_insert_delim(char **, size_t *, int, int);

#endif
