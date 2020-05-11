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

char	*ft_strdup(const char *s)
{
	char	*ptr;
	char	*ref;

	if (NULL == s)
	{
		return (NULL);
	}
	LIBFT_ALLOC(ptr, ft_strlen(s) + 1);
	if (NULL == ptr)
	{
		return (NULL);
	}
	ref = ptr;
	while (*s != '\0')
	{
		*ptr++ = *s++;
	}
	*ptr = '\0';
	return (ref);
}
