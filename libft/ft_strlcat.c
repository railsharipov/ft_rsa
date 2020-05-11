/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/10 17:20:30 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/12 14:53:56 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	i;
	size_t	j;
	size_t	dlen;

	i = 0;
	j = 0;
	dlen = (ft_strlen(dst) > dstsize) ? dstsize : ft_strlen(dst);
	if (dstsize == 0)
		return (ft_strlen(src));
	while (dst[i] != 0)
		i++;
	while (i < dstsize - 1 && src[j] != 0)
		dst[i++] = src[j++];
	dst[i] = 0;
	return (dlen + ft_strlen(src));
}
