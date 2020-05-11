/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/30 17:05:02 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:35:53 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <locale.h>
# include <libft.h>

typedef struct	s_pf
{
	char		flag;
	char		len;
	int			fwid;
	int			prec;
	char		type;
	int			num;
	size_t	size;
	size_t	str_size;
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

int				ft_printf(const char *format, ...);
int				ft_fprintf(int fd, const char *format, ...);
int				ft_sprintf(char **buf, const char *format, ...);
t_pf			*pf_l_arg_new(int fd);
void			pf_l_arg_zero(t_pf *data);
void			pf_out(t_pf *data, const char *s, int size);
void			pf_get_data(t_pf *data, const char *s, int *i);
void			pf_format_di(t_pf *data, char len, va_list *arg);
void			pf_format_oux(t_pf *data, char len, va_list *arg);
void			pf_format_s(t_pf *data, char len, va_list *arg);
void			pf_format_c(t_pf *data, char len, va_list *arg);
void			pf_print_di(t_pf *data, intmax_t n, char flag, size_t i);
void			pf_print_oux(t_pf *data, uintmax_t n, char flag, size_t i);
void			pf_print_c(t_pf *data, char *c, size_t size);
void			pf_print_wc(t_pf *data, unsigned int c);
void			pf_print_s(t_pf *data, char *src, size_t i);
void			pf_print_ws(t_pf *data, unsigned int *src);
void			pf_print_else(t_pf *data, char flag, size_t i);
void			pf_oper(t_pf *data, char *s, size_t *i, char op);
char			*pf_convert_to_utf_8(unsigned int *ws, int size, int prec);

#endif
