/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/11 19:49:24 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/11 19:57:48 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft/string.h>

char	*ft_strncat(char *s1, const char *s2, size_t n)
{
	size_t	i;
	size_t	j;

	if (NULL == s1 || NULL == s2)
		return (s1);

	i = 0;
	while (s1[i] != 0)
		i++;

	j = 0;
	while (s2[j] != 0 && j < n)
		s1[i++] = s2[j++];

	s1[i] = 0;
	return (s1);
}
