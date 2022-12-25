/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/10 19:23:28 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/10 19:28:28 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

char	*ft_strrchr(const char *s, int c)
{
	ssize_t	slen;

	if (NULL == s)
		return (NULL);
	
	slen = ft_strlen(s);

	while (--slen >= 0)
	{
		if (s[slen] == (char)c)
			return ((char *)(s + slen));
	}

	return (NULL);
}
