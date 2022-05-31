/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_intchar.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/25 15:35:40 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/25 15:53:42 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

char	*ft_intchar(char *buf, intmax_t integer, int int_bytes)
{
	int		j;
	int		shift;

	if (!buf)
		return (NULL);
	j = 0;
	while (j < int_bytes)
	{
		shift = (int_bytes - j - 1) * 8;
		buf[j] = (unsigned char)((integer >> shift) & 0xff);
		j++;
	}
	return (buf);
}
