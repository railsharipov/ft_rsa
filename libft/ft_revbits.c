/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bytes_reverse_bits.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/25 16:24:24 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/25 16:25:27 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

void	ft_bytes_reverse_bits(void *ptr, size_t size)
{
	unsigned char	*p;
	unsigned char	num;
	size_t			i;
	size_t			j;

	p = ptr;
	i = 0;
	while (i < size)
	{
		num = 0;
		j = 0;
		while (j < 8)
		{
			if (p[i] & (1 << j))
				num |= (1 << (7 - j));
			j++;
		}
		p[i] = num;
		i++;
	}
}
