/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/23 19:59:13 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:18:47 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

char	*ft_strrev(const char *s)
{
	char	*srev;
	size_t	len;
	int		idx;

	if (NULL == s)
		return (NULL);

	len = ft_strlen(s);
	LIBFT_ALLOC(srev, len + 1);

	idx = len-1;
	while (*s != '\0')
		srev[idx--] = *s++;

	srev[len] = '\0';

	return (srev);
}
