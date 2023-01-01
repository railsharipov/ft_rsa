/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/09 21:30:47 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/12 21:50:06 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

int	ft_memcmp(const void *p1, const void *p2, size_t n)
{
	unsigned char	*octets_1;
	unsigned char	*octets_2;
	size_t			i;

	octets_1 = (unsigned char *)p1;
	octets_2 = (unsigned char *)p2;

	i = 0;
	while (i < n)
	{
		if (octets_1[i] != octets_2[i])
			return (octets_1[i] - octets_2[i]);
		i++;
	}

	return (0);
}
