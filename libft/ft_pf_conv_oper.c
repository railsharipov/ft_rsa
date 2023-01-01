/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_conv_oper.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 10:01:45 by rsharipo          #+#    #+#             */
/*   Updated: 2018/08/11 10:03:57 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>

void	pf_oper(t_pf *data, char *s, size_t *i, char op)
{
	size_t	j;

	j = 0;

	if (op == '+' && data->flag & PF_PLUS)
	{
		s[*i + j++] = '+';
	}
	else if (op == '+' && data->flag & PF_SPACE)
	{
		s[*i + j++] = ' ';
	}
	else if (op == '-')
	{
		s[*i + j++] = '-';
	}
	else if (op == '0' && data->space > 0)
	{
		while ((int)j < data->space)
			s[*i + j++] = '0';
	}
	else if (op == ' ' && data->space > 0)
	{
		while ((int)j < data->space)
			s[*i + j++] = ' ';
	}
	else if (op == 'p' && data->prec > (int)data->size)
	{
		while ((int)j < data->prec - (int)data->size)
			s[*i + j++] = '0';
	}
	else if (op == 'x' && ft_strchr("xXp", data->type) && (j = 2))
	{
		ft_strcpy(s + *i, (data->type == 'X' ? "0X" : "0x"));
	}
	
	*i += j;
}
