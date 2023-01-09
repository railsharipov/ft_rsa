/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strup.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/11 17:13:02 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/11 17:15:14 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

char *ft_strup(char *s)
{
	char *s_up;
	size_t idx;

	if (NULL == s)
		return (NULL);

	s_up = ft_strdup(s);

	idx = 0;
	while (s_up[idx] != '\0')
	{
		if (s_up[idx] >= 'a' && s_up[idx] <= 'z')
			s_up[idx] = s_up[idx] - 32;

		idx++;
	}

	return (s_up);
}
