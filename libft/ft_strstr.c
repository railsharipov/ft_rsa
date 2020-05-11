/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strstr.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/21 23:19:17 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/01 11:32:43 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <stdio.h>

static int	ft_scan(const char *hay, const char *nee)
{
	unsigned int	i;

	i = 0;
	while (hay[i] != 0 && nee[i] != 0)
	{
		if (nee[i] != hay[i])
			return (0);
		i++;
	}
	if (nee[i] == 0)
		return (1);
	else
		return (0);
}

char		*ft_strstr(const char *haystack, const char *needle)
{
	int		i;
	int		j;

	i = 0;
	j = 0;
	if (!haystack || !needle)
		return (NULL);
	if (needle[j] == 0)
		return ((char *)haystack);
	while (haystack[i] != 0)
	{
		if (haystack[i] == needle[0] && ft_scan(haystack + i, needle))
			return ((char *)(haystack + i));
		i++;
	}
	return (NULL);
}
