/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_pr_str.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 09:24:37 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/02 10:05:11 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>

void	pf_out(t_pf *data, const char *s, int size)
{
	char	*ptr;

	if (data->buf)
	{
		ptr = malloc(data->num+size+1);

		if (ENOMEM == errno)
		{
			perror("FATAL ERROR :");
			exit(-1);
		}
		
		if (NULL != *(data->buf))
		{
			ft_memcpy(ptr, *(data->buf), data->num);
			free(*(data->buf));
		}

		ft_memcpy(ptr + data->num, s, size);
		ptr[data->num+size] = '\0';

		*(data->buf) = ptr;
		data->num += size;
	}
	else if (data->fd >= 0)
	{
		write(data->fd, s, size);
		data->num += size;
	}
}
