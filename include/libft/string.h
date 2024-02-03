/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/09 13:36:51 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 09:00:28 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_STRING_H
# define LIBFT_STRING_H

# include <string.h>
# include <libft/common.h>

typedef	struct	s_ostring
{
	char		*content;
	size_t		size;
}				t_ostring;

t_ostring	*ft_ostr_init(void);
t_ostring	*ft_ostr_new(size_t);
t_ostring	*ft_ostr_dup(t_ostring *);
void		ft_ostr_del(t_ostring *);
void		ft_ostr_clean(t_ostring *);
void		ft_ostr_append(t_ostring *ostring, void *content, size_t size);
void		ft_ostr_prepend(t_ostring *ostring, void *content, size_t size);
char		*ft_ostr_to_cstr(t_ostring *ostring, size_t idx, size_t len);
void		ft_ostr_set_content(t_ostring *ostring, void *content, size_t size);
void		*ft_ostr_get_content(t_ostring *ostring);
size_t		ft_ostr_get_size(t_ostring *ostring);

void		ft_strdel(char *s);
size_t		ft_strlen(const char *s);
char		*ft_strdup(const char *s);
char		*ft_strndup(const char *s, size_t n);
char		*ft_strrev(const char *s);
char		*ft_strcpy(char *dst, const char *src);
char		*ft_strncpy(char *dst, const char *src, size_t len);
char		*ft_strcat(char *s1, const char *s2);
char		*ft_strncat(char *s1, const char *s2, size_t n);
char		*ft_strchr(const char *s, int c);
char		*ft_strrchr(const char *s, int c);
int			ft_strcmp(const char *s1, const char *s2);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
char		*ft_strsub(char const *s, unsigned int start, size_t len);
char		*ft_strjoin(char const *s1, char const *s2);
char		*ft_strjoin_multi(int n, ...);
char		**ft_strsplit(char const *s, char c);
char		*ft_strup(char *s);
char		*ft_strlow(char *s);
int			ft_str_isalpha(const char *s);
int			ft_str_isalnum(const char *s);
int			ft_str_isdigit(const char *s);
int			ft_str_isascii(const char *s);
int			ft_str_isprint(const char *s);
int			ft_str_ishex(const char *s);
int			ft_isalpha(int c);
int			ft_isdigit(int c);
int			ft_isalnum(int c);
int			ft_isascii(int c);
int			ft_isprint(int c);
int			ft_iseol(int c);
int			ft_iseolws(int c);
int			ft_iswspace(int c);
int			ft_isblank(int c);
int			ft_ishex(int c);
int			ft_toupper(int c);
int			ft_tolower(int c);
void		ft_putstr(char const *s);
void		ft_putstr_fd(int fd, char const *s);

#endif
