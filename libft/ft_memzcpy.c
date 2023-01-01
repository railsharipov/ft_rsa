/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memzcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/09 15:18:22 by rsharipo          #+#    #+#             */
/*   Updated: 2018/08/04 16:18:00 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

// Copy destsize bytes from src to dest.
// If destsize is bigger than srcsize,
// set remaining bytes (destsize - srcsize) to zero
void	*ft_memzcpy(void *dst, const void *src, size_t dstsize, size_t srcsize)
{
	if (dstsize > srcsize)
	{
		ft_memcpy(dst, src, srcsize);
		ft_memset(dst + srcsize, 0, dstsize - srcsize);
	}
	else
	{
		ft_memcpy(dst, src, dstsize);
	}

	return (dst);
}
