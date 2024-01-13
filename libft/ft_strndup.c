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

#include <libft/string.h>
#include <libft/alloc.h>

char	*ft_strndup(const char *s, size_t n)
{
	char	*sdup;
	size_t	len;
	size_t	slen;
	size_t	idx;

	if (NULL == s) {
		return (NULL);
	}
	slen = ft_strlen(s);
	len = MIN(n, slen);

	LIBFT_ALLOC(sdup, len + 1);

	if (NULL == sdup) {
		return (NULL);
	}
	idx = 0;
	while (idx < len) {
		sdup[idx] = s[idx];
		idx++;
	}
	sdup[len] = 0;

	return (sdup);
}
