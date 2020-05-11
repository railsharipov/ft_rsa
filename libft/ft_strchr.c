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

#include <libft.h>

char	*ft_strchr(const char *s, int c)
{
	int	i;
	int	len;

	if (NULL == s)
		return (NULL);
	
	i = 0;
	len = ft_strlen(s);
	while (i <= len)
		if (s[i++] == (char)c)
			return ((char *)(s + i - 1));
	return (NULL);
}
