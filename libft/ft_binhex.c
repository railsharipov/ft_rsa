/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_binhex.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/25 14:43:45 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/02 08:39:52 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

void	ft_binhex(char *hex, const void *bin, size_t binsize)
{
	char	A[16] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};
	unsigned char	*bptr;
	char			buf[2*binsize];
	size_t			ix;

	if ((NULL == hex) || (NULL == bin))
		return ;

	bptr = (unsigned char *)bin;

	for (ix = 0; ix < 2*binsize; ix++)
		buf[ix] = A[ (( bptr[ix>>1] )>>( 4*((ix+1)&1) )) & 0xF ];

	for (ix = 0; ix < 2*binsize; ix++)
		hex[ix] = buf[ix];
}
