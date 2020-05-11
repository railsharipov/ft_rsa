/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/09 15:18:22 by rsharipo          #+#    #+#             */
/*   Updated: 2018/08/04 16:18:00 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

void	*ft_memcpy(void *dst, const void *src, size_t srcsize)
{
	size_t			ix;
	unsigned char	*p1;
	unsigned char	*p2;

	ix = 0;
	p1 = (unsigned char *)dst;
	p2 = (unsigned char *)src;
	while (ix < srcsize)
	{
		p1[ix] = p2[ix];
		ix++;
	}
	return (dst);
}
