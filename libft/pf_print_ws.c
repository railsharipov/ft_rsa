/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_print_ws.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 09:53:10 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:28:06 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>

int		pf_ws_len(unsigned int *s)
{
	int len;

	len = 0;
	while (s && s[len])
		len++;
	return (len);
}

void	pf_print_ws(t_pf *data, unsigned int *src)
{
	char *s;

	data->size = pf_ws_len(src);
	if (data->prec < (int)data->size && data->flag & PF_PREC)
		data->size = data->prec;
	s = pf_convert_to_utf_8(src, data->size, data->prec);
	pf_print_s(data, s, 0);
	if (s)
		free(s);
}
