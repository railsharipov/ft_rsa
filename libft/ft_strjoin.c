/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/11 10:33:59 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/12 15:34:03 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*res;
	size_t	s1len;
	size_t	s2len;

	if (NULL == s1 && NULL == s2)
		return (NULL);
	
	if (NULL == s1 || NULL == s2)
	{
		if (NULL == s1)
			return (ft_strdup(s2));
		else
			return (ft_strdup(s1));
	}

	s1len = ft_strlen(s1);
	s2len = ft_strlen(s2);

	LIBFT_ALLOC(res, s1len+s2len+1);

	ft_memcpy(res, s1, s1len);
	ft_memcpy(res + s1len, s2, s2len);
	res[s1len + s2len] = 0;

	return (res);
}
