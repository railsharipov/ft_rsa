/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_permute_key.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/28 16:38:57 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 10:05:29 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ssl/ssl.h>
#include <ssl/des.h>
#include <libft/bytes.h>

static const unsigned char	PKA[56] = {
	57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18,
	10, 2,  59, 51, 43, 35, 27, 19, 11, 3,  60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15, 7,  62, 54, 46, 38, 30, 22,
	14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4
};

void	des_permute_key(uint64_t *permut_key, unsigned char *key)
{
	uint64_t	t_key;
	uint64_t	t_pkey;
	int			ix;

	t_key = ft_uint_bswap64(*(uint64_t *)(key));
	t_pkey = 0;

	ix = 0;
	while (ix < 56)
	{
		t_pkey <<= 1;
		t_pkey |= (t_key >> (64 - PKA[ix])) & 1;
		ix++;
	}
	*permut_key = t_pkey;
}
