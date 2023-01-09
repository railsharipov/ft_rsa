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

void	ft_intbytes(char *buf, intmax_t integer, int intsize)
{
	int		idx;
	int		shift;

	if (!buf)
		return ;
	
	idx = 0;
	while (idx < intsize)
	{
		shift = (intsize - idx - 1) * 8;
		buf[idx] = (unsigned char)((integer >> shift) & 0xff);
		idx++;
	}
}
