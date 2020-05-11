/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcoll.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/19 10:06:32 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/26 14:50:01 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

char	*ft_strcoll(char *src)
{
	char	tmp[ft_strlen(src) + 1];
	size_t	i;
	size_t	j;
	size_t	len;

	if (!src)
		return (NULL);
	len = ft_strlen(src);
	i = 0;
	j = 0;
	while (i < len)
	{
		if (src[i] != ' ' && src[i] != '\t' && src[i] != '\n')
			tmp[j++] = src[i];
		i++;
	}
	tmp[j] = 0;
	ft_bzero(src, len);
	ft_strcpy(src, tmp);
	return (src);
}
