/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strnstr.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/21 23:19:17 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/12 10:31:44 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

static int	__find(const char *hay, const char *nee, size_t len)
{
	size_t	i;

	i = 0;
	while (hay[i] != 0 && nee[i] != 0 && i < len)
		if (nee[i] != hay[i] || (hay[++i] == 0 && nee[i] != 0))
			return (0);
	if (nee[i] != 0)
		return (0);
	return (1);
}

char		*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	if (needle[j] == 0)
		return ((char *)haystack);
	while (haystack[i] != 0 && i < len)
	{
		if (haystack[i] == needle[0] && __find(haystack + i, needle, len - i))
			return ((char *)(haystack + i));
		i++;
	}
	return (NULL);
}
