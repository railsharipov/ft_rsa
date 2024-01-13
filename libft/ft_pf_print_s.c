/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_print_s.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 09:52:14 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:25:48 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <libft/std.h>
#include <libft/string.h>

void	pf_print_s(t_pf *data, char *src, size_t i)
{
	char	*temp;
	char	*s;

	data->size = ft_strlen(src);

	if (data->prec < (int)data->size && data->flag & PF_PREC)
		data->size = data->prec;

	data->str_size = data->size;
	data->space = data->fwid - (int)data->str_size;

	if (data->fwid > (int)data->str_size)
		data->str_size = data->fwid;

	if (!(s = malloc(data->str_size + 1)))
		return ;

	if (!(data->flag & PF_MINUS))
		pf_oper(data, s, &i, (data->flag & PF_ZERO) ? ('0') : (' '));

	if (src)
	{
		ft_memzcpy(s + i, src, data->str_size - i, data->size);

		if (data->type == 'q')
		{
			// 'q' means uppercase format
			temp = ft_strup(s + i);
			ft_strcpy(s + i, temp);
			free(temp);
		}

		i += data->size;
	}

	if (data->flag & PF_MINUS)
		pf_oper(data, s, &i, (' '));

	s[i] = 0;
	pf_out(data, s, i);
	free(s);
}
