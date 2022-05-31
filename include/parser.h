#ifndef PARSER_H
# define PARSER_H

# define RK_PRIME 101
# define RK_ALPHA 256

int		parser_find(const char *, int, const char *, int);
void	parser_del_blank(const char *, int, char **, int *);
void	parser_del_wspace(const char *, int, char **, int *);
void	parser_del_eolws(const char *, int, char **, int *);
void	parser_del_empty_lines(const char *, int, char **, int *);
char	*parser_line(const char *, int, const char *, int);
void	parser_insert_delim(char **, size_t *, int, int);

#endif
