/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/09 13:58:21 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/11 17:23:05 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

void	*ft_memset(void *buf, int c, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
		((unsigned char *)buf)[i++] = (unsigned char)c;

	return (buf);
}
