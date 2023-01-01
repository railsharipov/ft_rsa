/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/23 19:59:13 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/11 08:49:24 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

void	*ft_memdup(void *src, size_t srcsize)
{
	void	*dst;

	if (NULL == src)
		return (NULL);
	
	LIBFT_ALLOC(dst, srcsize);
	ft_memcpy(dst, src, srcsize);
	
	return (dst);
}
