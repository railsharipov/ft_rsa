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

#ifndef LIBFT_STD_H
# define LIBFT_STD_H

# include <stdarg.h>
# include <stdint.h>
# include <string.h>
# include <locale.h>
# include <libft/common.h>

# define LIBFT_BUFF     256

typedef struct	s_pf
{
	char		flag;
	char		len;
	int			fwid;
	int			prec;
	char		type;
	int			num;
	size_t		size;
	size_t		str_size;
	int			space;
	int			base;
	int			fd;
	char		**buf;
}				t_pf;

typedef enum	e_pf_flag
{
	PF_PREC = (1 << 6),
	PF_SHARP = (1 << 5),
	PF_ZERO = (1 << 4),
	PF_MINUS = (1 << 3),
	PF_PLUS = (1 << 2),
	PF_SPACE = (1 << 1),
	PF_APOS = (1 << 0)
}				t_flag;

typedef enum	e_pf_len
{
	PF_Z = (1 << 5),
	PF_J = (1 << 4),
	PF_LL = (1 << 3),
	PF_L = (1 << 2),
	PF_H = (1 << 1),
	PF_HH = (1 << 0)
}				t_len;

int		    ft_printf(const char *format, ...);
int		    ft_fprintf(int fd, const char *format, ...);
int		    ft_sprintf(char **buf, const char *format, ...);
int		    ft_vprintf(const char *format, va_list arg);
int		    ft_vfprintf(int fd, const char *format, va_list arg);
int		    ft_vsprintf(char **buf, const char *format, va_list arg);

void 		*ft_memset(void *b, int c, size_t len);
void		ft_bzero(void *s, size_t n);
void		*ft_memcpy(void *dst, const void *src, size_t n);
void		*ft_memzcpy(void *dst, const void *src, size_t, size_t);
void		*ft_memmove(void *dst, const void *src, size_t len);
int			ft_memcmp(const void *s1, const void *s2, size_t n);
void		*ft_memdup(void *, size_t);

ssize_t	    ft_atoi(const char *str);
char		*ft_itoa(intmax_t n);
char		*ft_itoa_base(intmax_t value, int base);

int			get_next_line(int fd, char **line);

#endif
