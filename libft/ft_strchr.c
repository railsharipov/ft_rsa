/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/10 19:10:04 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/10 19:21:24 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft/string.h>

char	*ft_strchr(const char *s, int c)
{
	size_t	i;
	size_t	slen;

	if (NULL == s)
		return (NULL);

	i = 0;
	slen = ft_strlen(s);

	while (i < slen)
	{
		if (s[i] == (char)c)
			return ((char *)(s + i));

		i++;
	}

	return (NULL);
}
