/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_hexbin.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/25 16:16:45 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/28 15:11:30 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

static const int	A[128] = {
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	48,	48,	48,	48,	48,	48,	48,	48,	48,	48,	0,	0,	0,	0,	0,	0,
	0,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,
	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	0,	0,	0,	0,	0,
	0,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,
	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	0,	0,	0,	0,	0,
};

void	ft_hexbin(void *bin, const char *hex, size_t hexsize)
{
	unsigned char	*buf;
	unsigned char	*ptr;
	size_t			bufsize;
	ssize_t			ix;

	if ((NULL == hex) || (NULL == bin))
		return ;

	bufsize = 2 * NBITS_TO_NBYTES(4 * hexsize);
	LIBFT_ALLOC(buf, bufsize);
	ft_bzero(buf, bufsize);

	ptr = (unsigned char *)buf+bufsize-1;

	for (ix = hexsize-1; ix >= 0; ix--)
		*ptr-- = hex[ix] - A[hex[ix] & 0x7F];

	ptr = (unsigned char *)buf;

	for (ix = 0; ix < bufsize/2; ix++)
	{
		((unsigned char *)bin)[ix] = *ptr++ << 4;
		((unsigned char *)bin)[ix] |= *ptr++;
	}
	LIBFT_FREE(buf);
}
