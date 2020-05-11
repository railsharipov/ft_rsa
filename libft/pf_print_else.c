/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_print_else.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 09:54:06 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:25:22 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>

void	pf_print_else(t_pf *data, char flag, size_t i)
{
	char	*s;

	if (data->fwid > 1)
		data->str_size = data->fwid;
	else
		data->str_size = 1;
	data->space = data->fwid - 1;
	if (!(s = malloc(data->str_size + 1)))
		return ;
	if (!(flag & PF_MINUS))
		pf_oper(data, s, &i, (flag & PF_ZERO) ? ('0') : (' '));
	s[i++] = data->type;
	if (flag & PF_MINUS)
		pf_oper(data, s, &i, (' '));
	s[i] = 0;
	pf_out(data, s, i);
	free(s);
}
