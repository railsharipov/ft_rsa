/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_print_di.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 09:48:54 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:28:28 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>

void	pf_di_to_s(t_pf *data, char *s, intmax_t n, size_t *i)
{
	int	j;

	j = data->size;

	while (--j >= 0)
	{
		if (n < 0)
			s[*i + j] = -1 * (n % data->base) + 48;
		else
			s[*i + j] = n % data->base + 48;

		n = n / data->base;
	}

	*i += data->size;
}

void	pf_di_str_size(t_pf *data, intmax_t n, size_t size)
{
	if ((int)size > data->prec)
		data->str_size += size;
	else
		data->str_size += (size_t)data->prec;

	if (n < 0 || data->flag & PF_SPACE || data->flag & PF_PLUS)
		data->str_size += 1;

	data->space = data->fwid - (int)(data->str_size);

	if (data->str_size < (size_t)data->fwid)
		data->str_size = (size_t)data->fwid;
}

void	pf_di_size(t_pf *data, intmax_t n)
{
	if (n == 0 && data->prec == 0)
		data->size = 0;
	else if (n == 0)
		data->size = 1;

	while (n)
	{
		data->size += 1;
		n = n / (intmax_t)(data->base);
	}
}

void	pf_print_di(t_pf *data, intmax_t n, char flag, size_t i)
{
	char	*s;

	pf_di_size(data, n);
	pf_di_str_size(data, n, data->size);

	if (!(s = malloc(data->str_size + 1)))
		return ;

	if (flag & PF_ZERO)
		pf_oper(data, s, &i, (n < 0) ? ('-') : ('+'));
		
	if (!(flag & PF_MINUS))
		pf_oper(data, s, &i, (flag & PF_ZERO) ? ('0') : (' '));

	if (!(flag & PF_ZERO))
		pf_oper(data, s, &i, (n < 0) ? ('-') : ('+'));

	pf_oper(data, s, &i, ('p'));
	pf_di_to_s(data, s, n, &i);

	if (flag & PF_MINUS)
		pf_oper(data, s, &i, (' '));

	s[i] = 0;
	
	pf_out(data, s, i);
	free(s);
}
