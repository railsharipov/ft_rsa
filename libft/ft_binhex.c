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

static const char	A[16] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

char	*ft_binhex(const void *bin, size_t binsize)
{
	unsigned char	*bptr;
	char			*hptr;
	char			*hex;
	size_t			hexsize;
	size_t			ix;

	if (NULL == bin || binsize == 0)
		return (NULL);

	hexsize = 2*binsize;
	LIBFT_ALLOC(hex, hexsize+1);
	hptr = hex;
	bptr = (unsigned char *)bin;

	for (ix = 0; ix < binsize; ix++)
	{
		*hptr++ = A[bptr[ix]>>4];
		*hptr++ = A[bptr[ix]&0xF];
	}
	*hptr = 0;

	ix = 0;
	while (hex[ix] == '0' && ix < hexsize-1)
		ix++;

	hptr = hex;
	hex = ft_strdup(hptr + ix);
	LIBFT_FREE(hptr);

	return (hex);
}
