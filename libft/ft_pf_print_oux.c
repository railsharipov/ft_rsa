/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_print_oux.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 09:50:15 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:27:21 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>

void	pf_oux_to_s(t_pf *data, char *s, uintmax_t n, size_t *i)
{
	uintmax_t	val;
	int			j;

	val = 0;
	j = data->size;
	while (--j >= 0)
	{
		val = n % (uintmax_t)(data->base);
		if (val <= 9)
			s[*i + j] = val + 48;
		else if (val >= 10 && val <= 15)
			s[*i + j] = val + ((data->type == 'X') ? 55 : 87);
		n = n / (uintmax_t)(data->base);
	}
	*i += data->size;
}

void	pf_oux_str_size(t_pf *data, uintmax_t n, size_t size, char type)
{
	if (data->flag & PF_SHARP && type == 'o' && data->prec <= (int)size)
		data->prec = size + 1;
	else if (data->flag & PF_SHARP && ft_strchr("xX", type) && n)
		data->str_size += 2;
	else if (type == 'p')
		data->str_size += 2;
	if ((int)size > data->prec)
		data->str_size += size;
	else
		data->str_size += (size_t)data->prec;
	data->space = data->fwid - (int)(data->str_size);
	if (data->str_size < (size_t)data->fwid)
		data->str_size = (size_t)data->fwid;
}

void	pf_oux_size(t_pf *data, uintmax_t n)
{
	if (n == 0 && data->prec == 0)
		data->size = 0;
	else if (n == 0)
		data->size = 1;
	if (data->type == 'o' && n == 0 && !(data->flag & PF_PREC))
		data->flag &= (~PF_SHARP);
	while (n)
	{
		data->size += 1;
		n = n / (uintmax_t)(data->base);
	}
}

void	pf_print_oux(t_pf *data, uintmax_t n, char flag, size_t i)
{
	char	*s;

	pf_oux_size(data, n);
	pf_oux_str_size(data, n, data->size, data->type);
	if (!(s = malloc(data->str_size + 1)))
		return ;
	if (flag & PF_ZERO && ((n != 0 && flag & PF_SHARP) || data->type == 'p'))
		pf_oper(data, s, &i, ('x'));
	if (!(flag & PF_MINUS))
		pf_oper(data, s, &i, (flag & PF_ZERO) ? ('0') : (' '));
	if (!(flag & PF_ZERO) && ((n != 0 && flag & PF_SHARP) || data->type == 'p'))
		pf_oper(data, s, &i, ('x'));
	pf_oper(data, s, &i, ('p'));
	pf_oux_to_s(data, s, n, &i);
	if (flag & PF_MINUS)
		pf_oper(data, s, &i, (' '));
	s[i] = 0;
	pf_out(data, s, i);
	free(s);
}
