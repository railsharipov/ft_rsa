/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/21 17:47:29 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/03 10:29:03 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft/string.h>

char	*ft_strcpy(char *dst, const char *src)
{
	size_t	i;

	if (NULL == dst || NULL == src)
		return (dst);

	i = 0;
	while (src[i] != 0)
	{
		dst[i] = src[i];
		++i;
	}
	dst[i] = 0;

	return (dst);
}
