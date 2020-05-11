/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/09 17:14:47 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/12 13:55:30 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

void	*ft_memmove(void *dst, const void *src, size_t size)
{
	unsigned char	*p1;
	unsigned char	*p2;
	size_t	ix;

	p1 = (unsigned char *)dst;
	p2 = (unsigned char *)src;
	ix = 0;

	if (dst >= src)
		while (size--)
			p1[size] = p2[size];
	else
	{
		while (ix < size)
		{
			p1[ix] = p2[ix];
			ix++;
		}
	}
	return (dst);
}
