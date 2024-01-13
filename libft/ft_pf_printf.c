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

#include <stdlib.h>
#include <unistd.h>
#include <libft/std.h>
#include <libft/string.h>

static void __flags(char *len, char *flag, char *type);
static void __parse(t_pf *data, const char *s, va_list *arg);
static void __format(t_pf *data, va_list *arg);

int ft_printf(const char *format, ...)
{
	int num;
	va_list arg;
	t_pf *data;

	num = 0;
	va_start(arg, format);

	if (NULL == (data = pf_l_arg_new(STDOUT_FILENO)))
		return (-1);

	if (format)
		__parse(data, format, &arg);

	num = data->num;
	free(data);
	data = NULL;

	return (num);
}

int ft_fprintf(int fd, const char *format, ...)
{
	int num;
	va_list arg;
	t_pf *data;

	num = 0;
	va_start(arg, format);

	if ((fd < 0) || (NULL == (data = pf_l_arg_new(fd))))
		return (-1);

	if (format)
		__parse(data, format, &arg);

	num = data->num;
	free(data);
	data = NULL;

	return (num);
}

int ft_sprintf(char **buf, const char *format, ...)
{
	int num;
	va_list arg;
	t_pf *data;

	num = 0;
	va_start(arg, format);

	if ((NULL == buf) || (NULL == (data = pf_l_arg_new(-1))))
		return (-1);

	*buf = malloc(1);
	data->buf = buf;

	if (format)
		__parse(data, format, &arg);

	num = data->num;
	free(data);
	data = NULL;

	return (num);
}

static void __flags(char *len, char *flag, char *type)
{
	if (ft_strchr("DOUCS", *type) && (*len = PF_L))
		*type += 32;

	if (!(*type == 'x' || *type == 'X' || *type == 'o'))
		*flag &= (~PF_SHARP);

	if (*type == 'p')
	{
		*flag = (*flag & (~PF_SPACE) & (~PF_PLUS) & (~PF_PREC)) | PF_SHARP;
		*len = PF_L;
	}

	if (ft_strchr("diouxXp", *type) && (*flag & PF_MINUS || *flag & PF_PREC))
		*flag &= (~PF_ZERO);

	if (!(*type == 'd' || *type == 'i'))
	{
		*flag &= (~PF_SPACE);
		*flag &= (~PF_PLUS);
	}

	if (*flag & PF_PLUS)
		*flag &= (~PF_SPACE);
}

static void __format(t_pf *data, va_list *arg)
{
	__flags(&(data->len), &(data->flag), &(data->type));

	if (data->type == 'x' || data->type == 'X' || data->type == 'p')
		data->base = 16;

	else if (data->type == 'o')
		data->base = 8;

	if (data->type == '%')
		pf_out(data, "%", 1);

	else if (data->type == '@')
		data->fd = STDERR_FILENO;

	else if (ft_strchr("id", data->type))
		pf_format_di(data, data->len, arg);

	else if (ft_strchr("ouxXp", data->type))
		pf_format_oux(data, data->len, arg);

	else if ((data->type == 's') || (data->type == 'q'))
		pf_format_s(data, data->len, arg);

	else if (data->type == 'c')
		pf_format_c(data, data->len, arg);

	else
		pf_print_else(data, data->flag, 0);
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
			pf_out(data, s + start, i - start);

		if (s[i] == '%' && s[++i])
		{
			if (s[i] == '%' || s[i] == '@')
				data->type = s[i];
			else
				pf_get_data(data, s, &i);
			__format(data, arg);
		}

		pf_l_arg_zero(data);

		if (s[i])
			i++;
	}
}