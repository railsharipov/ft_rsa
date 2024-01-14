/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/30 15:04:23 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:25:03 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libft/std.h>
#include <libft/string.h>

static void	__l_arg_zero(t_pf *l);
static t_pf	*__l_arg_new(int fd);

static void __parse(t_pf *data, const char *s, va_list *arg);
static void __format(t_pf *data, va_list *arg);
static void	__get_data(t_pf *data, const char *s, int *i);
static void	__oper(t_pf *data, char *s, size_t *i, char op);

static void __flags(char *len, char *flag, char *type);
static int	__is_flag(char c, char *flag);
static int	__is_len_flag(char c, char next, char *len, int *i);
static void	__format_di(t_pf *data, char len, va_list *arg);
static void	__format_oux(t_pf *data, char len, va_list *arg);
static void	__format_c(t_pf *data, char len, va_list *arg);
static void	__format_s(t_pf *data, char len, va_list *arg);

static void	__out(t_pf *data, const char *s, int size);

static void	__print_c(t_pf *data, char *src, size_t size);
static void	__print_di(t_pf *data, intmax_t n, char flag, size_t i);
static void	__print_s(t_pf *data, char *src, size_t i);
static void	__print_wc(t_pf *data, unsigned int c);
static void	__print_ws(t_pf *data, unsigned int *src);
static void	__print_oux(t_pf *data, uintmax_t n, char flag, size_t i);
static void	__print_else(t_pf *data, char flag, size_t i);

static void	__di_to_s(t_pf *data, char *s, intmax_t n, size_t *i);
static void	__di_str_size(t_pf *data, intmax_t n, size_t size);
static void	__di_size(t_pf *data, intmax_t n);

static void	__oux_to_s(t_pf *data, char *s, uintmax_t n, size_t *i);
static void	__oux_size(t_pf *data, uintmax_t n);
static void	__oux_str_size(t_pf *data, uintmax_t n, size_t size, char type);

static int	__ws_len(unsigned int *s);

static void	__utf_8_size(unsigned int *ws, int size, int *chars);
static void	__encode_utf_8(unsigned int *ws, char *s, int chars, int j);
static char	*__convert_to_utf_8(unsigned int *ws, int size, int prec);

int ft_printf(const char *format, ...)
{
	int num;
	va_list arg;
	t_pf *data;

	num = 0;
	va_start(arg, format);

	if (NULL == (data = __l_arg_new(STDOUT_FILENO))) {
		return (-1);
	}
	if (format) {
		__parse(data, format, &arg);
	}
	va_end(arg);
	num = data->num;
	free(data);

	return (num);
}

int ft_vprintf(const char *format, va_list arg)
{
	int num;
	t_pf *data;

	num = 0;

	if (NULL == (data = __l_arg_new(STDOUT_FILENO))) {
		return (-1);
	}
	if (format) {
		__parse(data, format, &arg);
	}
	va_end(arg);
	num = data->num;
	free(data);

	return (num);
}

int ft_fprintf(int fd, const char *format, ...)
{
	int num;
	va_list arg;
	t_pf *data;

	num = 0;
	va_start(arg, format);

	if ((fd < 0) || (NULL == (data = __l_arg_new(fd)))) {
		return (-1);
	}
	if (format) {
		__parse(data, format, &arg);
	}
	va_end(arg);
	num = data->num;
	free(data);

	return (num);
}

int ft_vfprintf(int fd, const char *format, va_list arg)
{
	int num;
	t_pf *data;

	num = 0;

	if ((fd < 0) || (NULL == (data = __l_arg_new(fd)))) {
		return (-1);
	}
	if (format) {
		__parse(data, format, &arg);
	}
	num = data->num;
	free(data);

	return (num);
}

int ft_sprintf(char **buf, const char *format, ...)
{
	int num;
	va_list arg;
	t_pf *data;

	num = 0;
	va_start(arg, format);

	if ((NULL == buf) || (NULL == (data = __l_arg_new(-1)))) {
		return (-1);
	}
	*buf = malloc(1);
	data->buf = buf;

	if (format) {
		__parse(data, format, &arg);
	}
	va_end(arg);
	num = data->num;
	free(data);

	return (num);
}

int ft_vsprintf(char **buf, const char *format, va_list arg)
{
	int num;
	t_pf *data;

	num = 0;

	if ((NULL == buf) || (NULL == (data = __l_arg_new(-1)))) {
		return (-1);
	}
	*buf = malloc(1);
	data->buf = buf;

	if (format) {
		__parse(data, format, &arg);
	}
	num = data->num;
	free(data);

	return (num);
}

static void	__l_arg_zero(t_pf *l)
{
	if (l)
	{
		l->flag = 0;
		l->len = 0;
		l->fwid = 0;
		l->prec = -1;
		l->type = -1;
		l->size = 0;
		l->str_size = 0;
		l->space = 0;
		l->base = 10;
	}
}

static t_pf	*__l_arg_new(int fd)
{
	t_pf	*new;

	if (NULL == (new = malloc(sizeof(t_pf)))) {
		return (NULL);
	}
	ft_bzero(new, sizeof(t_pf));

	new->fd = fd;
	new->prec = -1;
	new->type = -1;
	new->base = 10;

	return (new);
}

static void __flags(char *len, char *flag, char *type)
{
	if (ft_strchr("DOUCS", *type) && (*len = PF_L)) {
		*type += 32;
	}
	if (!(*type == 'x' || *type == 'X' || *type == 'o')) {
		*flag &= (~PF_SHARP);
	}
	if (*type == 'p') {
		*flag = (*flag & (~PF_SPACE) & (~PF_PLUS) & (~PF_PREC)) | PF_SHARP;
		*len = PF_L;
	}
	if (ft_strchr("diouxXp", *type) && (*flag & PF_MINUS || *flag & PF_PREC)) {
		*flag &= (~PF_ZERO);
	}
	if (!(*type == 'd' || *type == 'i')) {
		*flag &= (~PF_SPACE);
		*flag &= (~PF_PLUS);
	}
	if (*flag & PF_PLUS) {
		*flag &= (~PF_SPACE);
	}
}

static void __format(t_pf *data, va_list *arg)
{
	__flags(&(data->len), &(data->flag), &(data->type));

	if (data->type == 'x' || data->type == 'X' || data->type == 'p') {
		data->base = 16;
	} else if (data->type == 'o') {
		data->base = 8;
	}
	if (data->type == '%') {
		__out(data, "%", 1);
	} else if (data->type == '@') {
		data->fd = STDERR_FILENO;
	} else if (ft_strchr("id", data->type)) {
		__format_di(data, data->len, arg);
	} else if (ft_strchr("ouxXp", data->type)) {
		__format_oux(data, data->len, arg);
	} else if ((data->type == 's') || (data->type == 'q')) {
		__format_s(data, data->len, arg);
	} else if (data->type == 'c') {
		__format_c(data, data->len, arg);
	} else {
		__print_else(data, data->flag, 0);
	}
}

static void	__parse(t_pf *data, const char *s, va_list *arg)
{
	int	i;
	int	start;

	start = 0;

	i = 0;
	while (s[i])
	{
		start = i;

		while (s[i] && s[i] != '%')
			i++;

		if (i > start)
			__out(data, s + start, i - start);

		if (s[i] == '%' && s[++i]) {
			if (s[i] == '%' || s[i] == '@') {
				data->type = s[i];
			} else {
				__get_data(data, s, &i);
			}
			__format(data, arg);
		}

		__l_arg_zero(data);

		if (s[i]) {
			i++;
		}
	}
}

static int	__is_flag(char c, char *flag)
{
	char	check;

	check = *flag;

	if (ft_strchr("#0 -+'", c)) {
		(c == '#') ? (check |= PF_SHARP) : 0;
		(c == '0') ? (check |= PF_ZERO) : 0;
		(c == '-') ? (check |= PF_MINUS) : 0;
		(c == '+') ? (check |= PF_PLUS) : 0;
		(c == 32) ? (check |= PF_SPACE) : 0;
		(c == 39) ? (check |= PF_APOS) : 0;
		*flag |= check;

		return (1);
	}
	return (0);
}

static int	__is_len_flag(char c, char next, char *len, int *i)
{
	char	check;

	check = 0;

	if (!ft_strchr("hljz", c)) {
		return (0);
	}
	if (c == 'h' && next == 'h' && (*i += 1)) {
		check |= PF_HH;
	} else if (c == 'h') {
		check |= PF_H;
	}
	if (c == 'l' && next == 'l' && (*i += 1)) {
		check |= PF_LL;
	} else if (c == 'l') {
		check |= PF_L;
	}
	(c == 'j') ? check |= PF_J : 0;
	(c == 'z') ? check |= PF_Z : 0;

	if (check > *len) {
		*len = check;
	}
	return (1);
}

static void	__get_data(t_pf *data, const char *s, int *i)
{
	if (s[*i] == 0) {
		return ;
	}
	if (__is_flag(s[*i], &(data->flag))) {
		*i += 1;
	} else if (s[*i] >= '0' && s[*i] <= '9') {
		data->fwid = (int)ft_atoi(s + *i);

		while (s[*i] && s[*i] >= '0' && s[*i] <= '9') {
			*i += 1;
		}
	} else if (s[*i] == '.' && (*i += 1)) {
		data->flag |= PF_PREC;
		data->prec = (int)ft_atoi(s + *i);

		while (s[*i] && s[*i] >= '0' && s[*i] <= '9') {
			*i += 1;
		}
	} else if (s[*i] && __is_len_flag(s[*i], s[*i + 1], &(data->len), i)) {
		*i += 1;
	} else {
		data->type = s[*i];
		return ;
	}
	__get_data(data, s, i);
}

static void	__oper(t_pf *data, char *s, size_t *i, char op)
{
	size_t	j;

	j = 0;

	if (op == '+' && data->flag & PF_PLUS) {
		s[*i + j++] = '+';
	} else if (op == '+' && data->flag & PF_SPACE) {
		s[*i + j++] = ' ';
	} else if (op == '-') {
		s[*i + j++] = '-';
	} else if (op == '0' && data->space > 0) {
		while ((int)j < data->space) {
			s[*i + j++] = '0';
		}
	} else if (op == ' ' && data->space > 0) {
		while ((int)j < data->space) {
			s[*i + j++] = ' ';
		}
	} else if (op == 'p' && data->prec > (int)data->size) {
		while ((int)j < data->prec - (int)data->size) {
			s[*i + j++] = '0';
		}
	} else if (op == 'x' && ft_strchr("xXp", data->type) && (j = 2)) {
		ft_strcpy(s + *i, (data->type == 'X' ? "0X" : "0x"));
	}
	*i += j;
}

static void	__format_di(t_pf *data, char len, va_list *arg)
{
	char		type;
	intmax_t	val;

	val = 0;
	type = data->type;

	if (!(data->flag & PF_PREC))
		data->prec = 1;

	if (!len)
		val = (int)va_arg(*arg, int);
	else if (len & PF_HH)
		val = (signed char)va_arg(*arg, int);
	else if (len & PF_H)
		val = (short)va_arg(*arg, int);
	else if (len & PF_LL)
		val = (long long)va_arg(*arg, long long);
	else if (len & PF_L)
		val = (long)va_arg(*arg, long);
	else if (len & PF_J)
		val = va_arg(*arg, intmax_t);
	else if (len & PF_Z)
		val = (size_t)va_arg(*arg, size_t);

	__print_di(data, val, data->flag, 0);
}

static void	__format_oux(t_pf *data, char len, va_list *arg)
{
	char		type;
	uintmax_t	val;

	val = 0;
	type = data->type;

	if (!len)
		val = (unsigned int)va_arg(*arg, unsigned int);
	else if (len & PF_HH)
		val = (unsigned char)va_arg(*arg, unsigned int);
	else if (len & PF_H)
		val = (unsigned short)va_arg(*arg, unsigned int);
	else if (len & PF_LL)
		val = (unsigned long long)va_arg(*arg, unsigned long long);
	else if (len & PF_L)
		val = (unsigned long)va_arg(*arg, unsigned long);
	else if (len & PF_J)
		val = va_arg(*arg, uintmax_t);
	else if (len & PF_Z)
		val = (size_t)va_arg(*arg, size_t);

	__print_oux(data, val, data->flag, 0);
}

static void	__format_c(t_pf *data, char len, va_list *arg)
{
	unsigned int	wc;
	char			c;

	wc = 0;
	c = 0;

	if (len & PF_L)
	{
		wc = va_arg(*arg, unsigned int);
		__print_wc(data, wc);
	}
	else
	{
		c = (char)va_arg(*arg, int);
		__print_c(data, &c, 1);
	}
}

static void	__format_s(t_pf *data, char len, va_list *arg)
{
	unsigned int	*ws;
	char			*s;

	s = NULL;
	ws = NULL;

	if (len & PF_L)
		ws = va_arg(*arg, unsigned int *);
	else
		s = va_arg(*arg, char *);

	if (!s && !ws && !(data->flag & PF_PREC))
	{
		if (!(data->flag & PF_PREC))
			__out(data, "(null)", 6);
		return ;
	}
	if (len & PF_L)
		__print_ws(data, ws);
	else
		__print_s(data, s, 0);
}

static void	__out(t_pf *data, const char *s, int size)
{
	char	*ptr;

	if (data->buf) {
		ptr = malloc(data->num+size+1);

		if (ENOMEM == errno) {
			perror("error :");
			return ;
		}
		if (NULL != *(data->buf)) {
			ft_memcpy(ptr, *(data->buf), data->num);
			free(*(data->buf));
		}
		ft_memcpy(ptr + data->num, s, size);
		ptr[data->num+size] = '\0';

		*(data->buf) = ptr;
		data->num += size;

	} else if (data->fd >= 0) {
		write(data->fd, s, size);
		data->num += size;
	}
}

static void	__print_c(t_pf *data, char *src, size_t size)
{
	char	*s;
	size_t	i;

	i = 0;
	data->str_size = size;
	data->space = data->fwid - size;

	if (data->fwid > (int)data->str_size) {
		data->str_size = (size_t)data->fwid;
	}
	if (!(s = malloc(data->str_size + 1))) {
		return ;
	}
	if (!(data->flag & PF_MINUS)) {
		__oper(data, s, &i, (data->flag & PF_ZERO) ? ('0') : (' '));
	}
	if (src) {
		ft_memcpy(s + i, src, size);
		i += size;
	}
	if (data->flag & PF_MINUS) {
		__oper(data, s, &i, (' '));
	}
	s[i] = 0;
	__out(data, s, i);
	free(s);
}

static void	__di_to_s(t_pf *data, char *s, intmax_t n, size_t *i)
{
	int	j;

	j = data->size;

	while (--j >= 0) {
		if (n < 0) {
			s[*i + j] = -1 * (n % data->base) + 48;
		} else {
			s[*i + j] = n % data->base + 48;
		}
		n = n / data->base;
	}
	*i += data->size;
}

static void	__di_str_size(t_pf *data, intmax_t n, size_t size)
{
	if ((int)size > data->prec) {
		data->str_size += size;
	} else {
		data->str_size += (size_t)data->prec;
	}
	if (n < 0 || data->flag & PF_SPACE || data->flag & PF_PLUS) {
		data->str_size += 1;
	}
	data->space = data->fwid - (int)(data->str_size);

	if (data->str_size < (size_t)data->fwid) {
		data->str_size = (size_t)data->fwid;
	}
}

static void	__di_size(t_pf *data, intmax_t n)
{
	if (n == 0 && data->prec == 0) {
		data->size = 0;
	} else if (n == 0) {
		data->size = 1;
	}
	while (n) {
		data->size += 1;
		n = n / (intmax_t)(data->base);
	}
}

static void	__print_di(t_pf *data, intmax_t n, char flag, size_t i)
{
	char	*s;

	__di_size(data, n);
	__di_str_size(data, n, data->size);

	if (!(s = malloc(data->str_size + 1))) {
		return ;
	}
	if (flag & PF_ZERO) {
		__oper(data, s, &i, (n < 0) ? ('-') : ('+'));
	}
	if (!(flag & PF_MINUS)) {
		__oper(data, s, &i, (flag & PF_ZERO) ? ('0') : (' '));
	}
	if (!(flag & PF_ZERO)) {
		__oper(data, s, &i, (n < 0) ? ('-') : ('+'));
	}
	__oper(data, s, &i, ('p'));
	__di_to_s(data, s, n, &i);

	if (flag & PF_MINUS) {
		__oper(data, s, &i, (' '));
	}
	s[i] = 0;
	__out(data, s, i);
	free(s);
}

static void	__oux_to_s(t_pf *data, char *s, uintmax_t n, size_t *i)
{
	uintmax_t	val;
	int			j;

	val = 0;
	j = data->size;

	while (--j >= 0)
	{
		val = n % (uintmax_t)(data->base);

		if (val <= 9) {
			s[*i + j] = val + 48;
		} else if (val >= 10 && val <= 15) {
			s[*i + j] = val + ((data->type == 'X') ? 55 : 87);
		}
		n = n / (uintmax_t)(data->base);
	}
	*i += data->size;
}

static void	__oux_str_size(t_pf *data, uintmax_t n, size_t size, char type)
{
	if (data->flag & PF_SHARP && type == 'o' && data->prec <= (int)size) {
		data->prec = size + 1;
	} else if (data->flag & PF_SHARP && ft_strchr("xX", type) && n) {
		data->str_size += 2;
	} else if (type == 'p') {
		data->str_size += 2;
	}
	if ((int)size > data->prec) {
		data->str_size += size;
	} else {
		data->str_size += (size_t)data->prec;
	}
	data->space = data->fwid - (int)(data->str_size);

	if (data->str_size < (size_t)data->fwid) {
		data->str_size = (size_t)data->fwid;
	}
}

static void	__oux_size(t_pf *data, uintmax_t n)
{
	if (n == 0 && data->prec == 0) {
		data->size = 0;
	} else if (n == 0) {
		data->size = 1;
	}
	if (data->type == 'o' && n == 0 && !(data->flag & PF_PREC)) {
		data->flag &= (~PF_SHARP);
	}
	while (n) {
		data->size += 1;
		n = n / (uintmax_t)(data->base);
	}
}

static void	__print_oux(t_pf *data, uintmax_t n, char flag, size_t i)
{
	char	*s;

	__oux_size(data, n);
	__oux_str_size(data, n, data->size, data->type);

	if (!(s = malloc(data->str_size + 1))) {
		return ;
	}
	if (flag & PF_ZERO && ((n != 0 && flag & PF_SHARP) || data->type == 'p')) {
		__oper(data, s, &i, ('x'));
	}
	if (!(flag & PF_MINUS)) {
		__oper(data, s, &i, (flag & PF_ZERO) ? ('0') : (' '));
	}
	if (!(flag & PF_ZERO) && ((n != 0 && flag & PF_SHARP) || data->type == 'p')) {
		__oper(data, s, &i, ('x'));
	}
	__oper(data, s, &i, ('p'));
	__oux_to_s(data, s, n, &i);

	if (flag & PF_MINUS) {
		__oper(data, s, &i, (' '));
	}
	s[i] = 0;
	__out(data, s, i);
	free(s);
}

static void	__print_s(t_pf *data, char *src, size_t i)
{
	char	*temp;
	char	*s;

	data->size = ft_strlen(src);

	if (data->prec < (int)data->size && data->flag & PF_PREC) {
		data->size = data->prec;
	}
	data->str_size = data->size;
	data->space = data->fwid - (int)data->str_size;

	if (data->fwid > (int)data->str_size) {
		data->str_size = data->fwid;
	}
	if (!(s = malloc(data->str_size + 1))) {
		return ;
	}
	if (!(data->flag & PF_MINUS)) {
		__oper(data, s, &i, (data->flag & PF_ZERO) ? ('0') : (' '));
	}
	if (src) {
		ft_memzcpy(s + i, src, data->str_size - i, data->size);

		if (data->type == 'q') {
			// 'q' means uppercase format
			temp = ft_strup(s + i);
			ft_strcpy(s + i, temp);
			free(temp);
		}
		i += data->size;
	}
	if (data->flag & PF_MINUS) {
		__oper(data, s, &i, (' '));
	}
	s[i] = 0;
	__out(data, s, i);
	free(s);
}

static void	__print_wc(t_pf *data, unsigned int c)
{
	char	*s;

	s = __convert_to_utf_8(&c, 1, data->prec);
	__print_c(data, s, c ? ft_strlen(s) : 1);

	if (s) {
		free(s);
	}
}

static int	__ws_len(unsigned int *s)
{
	int len;

	len = 0;
	while (s && s[len])
		len++;

	return (len);
}

static void	__print_ws(t_pf *data, unsigned int *src)
{
	char *s;

	data->size = __ws_len(src);

	if (data->prec < (int)data->size && data->flag & PF_PREC)
		data->size = data->prec;

	s = __convert_to_utf_8(src, data->size, data->prec);
	__print_s(data, s, 0);

	if (s)
		free(s);
}

static void	__utf_8_size(unsigned int *ws, int size, int *chars)
{
	int	i;

	i = 0;
	while (ws && i < size)
	{
		if (ws[i] < 0x80) {
			*chars += 1;
		} else if (ws[i] < 0x800) {
			*chars += 2;
		} else if (ws[i] < 0x10000) {
			*chars += 3;
		} else if (ws[i] <= 0x10FFFF) {
			*chars += 4;
		}
		i++;
	}
}

static void	__encode_utf_8(unsigned int *ws, char *s, int chars, int j)
{
	int	tmp;

	tmp = 0;
	while (j < chars)
	{
		tmp = j;

		if (*ws < 0x80) {
			s[j++] = (char)(*ws);
		} else if (*ws < 0x800) {
			s[j++] = (char)(((*ws >> 6) & 0x3F) | 0xC0);
		} else if (*ws < 0x10000) {
			s[j++] = (char)(((*ws >> 12) & 0x3F) | 0xE0);
		} else if (*ws <= 0x10FFFF) {
			s[j++] = (char)(((*ws >> 18) & 0x3F) | 0xF0);
			s[j++] = (char)(((*ws >> 12) & 0x3F) | 0x80);
		}
		if (*ws >= 0x800) {
			s[j++] = (char)(((*ws >> 6) & 0x3F) | 0x80);
		}
		if (*ws >= 0x80) {
			s[j++] = (char)((*ws & 0x3F) | 0x80);
		}
		ws++;
	}
	s[(j == chars) ? j : tmp] = 0;
}

static char	*__convert_to_utf_8(unsigned int *ws, int size, int prec)
{
	int		chars;
	char	*s;

	chars = 0;

	if (ws) {
		__utf_8_size(ws, size, &chars);
	}
	if (prec > 0 && prec < chars) {
		chars = prec;
	}
	if (chars && (s = malloc(chars + 1))) {
		__encode_utf_8(ws, s, chars, 0);
		return (s);
	}
	return (NULL);
}

static void	__print_else(t_pf *data, char flag, size_t i)
{
	char	*s;

	if (data->fwid > 1) {
		data->str_size = data->fwid;
	} else {
		data->str_size = 1;
	}
	data->space = data->fwid - 1;

	if (!(s = malloc(data->str_size + 1))) {
		return ;
	}
	if (!(flag & PF_MINUS)) {
		__oper(data, s, &i, (flag & PF_ZERO) ? ('0') : (' '));
	}
	s[i++] = data->type;

	if (flag & PF_MINUS) {
		__oper(data, s, &i, (' '));
	}
	s[i] = 0;
	__out(data, s, i);
	free(s);
}