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

char	*ft_strdup(const char *s)
{
	char	*sdup;
	char	*ref;

	if (NULL == s)
		return (NULL);

	LIBFT_ALLOC(sdup, ft_strlen(s) + 1);

	if (NULL == sdup)
		return (NULL);

	ref = sdup;

	while (*s != '\0')
		*sdup++ = *s++;

	*sdup = '\0';

	return (ref);
}
