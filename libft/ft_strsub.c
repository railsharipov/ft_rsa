/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strsub.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/11 10:24:21 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/12 17:31:38 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft/string.h>
#include <libft/alloc.h>

char	*ft_strsub(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	char	*subs;

	if (NULL == s)
		return (NULL);

	LIBFT_ALLOC(subs, len + 1);

	i = 0;
	while (i < len)
		subs[i++] = s[start++];

	subs[i] = 0;

	return (subs);
}
