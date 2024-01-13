/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlow.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/11 17:13:02 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/11 17:16:06 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft/string.h>

char	*ft_strlow(char *s)
{
	char	*s_low;
	size_t	idx;

	if (NULL == s)
		return (NULL);

	s_low = ft_strdup(s);

	idx = 0;
	while (s_low[idx] != '\0')
	{
		if (s_low[idx] >= 'A' && s_low[idx] <= 'Z')
			s_low[idx] = s_low[idx] + 32;

		idx++;
	}

	return (s_low);
}
