/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_print_c.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 09:50:54 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:22:30 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>

void	pf_print_c(t_pf *data, char *src, size_t size)
{
	char	*s;
	size_t	i;

	i = 0;
	data->str_size = size;
	data->space = data->fwid - size;
	if (data->fwid > (int)data->str_size)
		data->str_size = (size_t)data->fwid;
	if (!(s = malloc(data->str_size + 1)))
		return ;
	if (!(data->flag & PF_MINUS))
		pf_oper(data, s, &i, (data->flag & PF_ZERO) ? ('0') : (' '));
	if (src)
	{
		ft_memcpy(s + i, src, size);
		i += size;
	}
	if (data->flag & PF_MINUS)
		pf_oper(data, s, &i, (' '));
	s[i] = 0;
	pf_out(data, s, i);
	free(s);
}
