/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ssl_bytes.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/06 10:47:08 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/05 08:45:31 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_ssl.h>

uint32_t	util_bswap32(uint32_t x)
{
	return (
		((x & 0xFF000000) >> 24)
		| ((x & 0x00FF0000) >> 8)
		| ((x & 0x0000FF00) << 8)
		| ((x & 0x000000FF) << 24)
	);
}

uint64_t	util_bswap64(uint64_t x)
{
	return (
		((x & 0xFF00000000000000) >> 56)
		| ((x & 0x00FF000000000000) >> 40)
		| ((x & 0x0000FF0000000000) >> 24)
		| ((x & 0x000000FF00000000) >> 8)
		| ((x & 0x00000000FF000000) << 8)
		| ((x & 0x0000000000FF0000) << 24)
		| ((x & 0x000000000000FF00) << 40)
		| ((x & 0x00000000000000FF) << 56)
	);
}

t_uint128	util_bswap128(t_uint128 x)
{
	t_uint128	y;

	y = ((t_uint128)(0xFF)) << 120;
	return (
		((x & y) >> 120)
		| (((x & (y >> 8)) >> 104))
		| (((x & (y >> 16)) >> 88))
		| (((x & (y >> 24)) >> 72))
		| (((x & (y >> 32)) >> 56))
		| (((x & (y >> 40)) >> 40))
		| (((x & (y >> 48)) >> 24))
		| (((x & (y >> 56)) >> 8))
		| (((x & (y >> 64)) >> 8))
		| (((x & (y >> 72)) >> 24))
		| (((x & (y >> 80)) >> 40))
		| (((x & (y >> 88)) >> 56))
		| (((x & (y >> 96)) >> 72))
		| (((x & (y >> 104)) >> 88))
		| (((x & (y >> 112)) >> 104))
	);
}

void		util_xor(char *res, char *buf1, char *buf2, size_t size)
{
	size_t	i;

	i = 0;
	while (i < size)
	{
		res[i] = buf1[i] ^ buf2[i];
		i++;
	}
}

int			util_lmbit(uint64_t num, int bits)
{
	bits = MIN(bits, 8 * sizeof(num));

	for (int i = bits; i > 0; i--)
	{
		if ((((uint64_t)1)<<(i-1)) & num)
			return (i);
	}
	return (0);
}

uint64_t	util_bytes_to_uint64(char *bytes, int nbytes)
{
	uint64_t	num;
	int			idx;

	num = 0;
	nbytes = MIN(nbytes, sizeof(uint64_t));

	idx = 0;
	while (idx < nbytes)
	{
		num <<= 8;
		num |= (uint64_t)bytes[idx++];
	}
	return (num);
}

int			util_rmbit(uint64_t num, int bits)
{
	for (int i = 1; i <= bits; i++)
	{
		if ((1<<(i-1)) & num)
			return (i);
	}
	return (0);
}

void		util_lshift_bytes(unsigned char *bytes, int size, int shift)
{
	unsigned char	*ptr;
	unsigned int	tmp;
	unsigned int	rem;

	if ((NULL == bytes) || (shift <= 0))
		return ;

	ptr = bytes + size-1;
	rem = 0;

	while (size-- > 0)
	{
		tmp = *ptr;
		*ptr = (*ptr << shift) | rem;
		rem = tmp >> (CHAR_BIT-shift);
		ptr--;
	}
}

void		util_rshift_bytes(unsigned char *bytes, int size, int shift)
{
	unsigned int	tmp;
	unsigned int	rem;

	if ((NULL == bytes) || (shift <= 0))
		return ;

	rem = 0;

	while (size-- > 0)
	{
		tmp = *bytes;
		*bytes = (*bytes >> shift) | rem;
		rem = tmp << (CHAR_BIT-shift);
	}
}
