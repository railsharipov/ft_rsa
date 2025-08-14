/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_ecb_encode.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/28 17:44:36 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/12 10:45:14 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <common.h>
#include <rand.h>
#include <base64.h>
#include <des.h>
#include <libft/bytes.h>

int des_cbc_encrypt(const uint8_t key[8], const uint8_t iv[8], t_ostring *mes, t_ostring *ciph)
{
	uint64_t	ksched[16];
	uint8_t		ivbuf[8], *ciptr;
	uint64_t	pkey;
	size_t		padsize, messize, cisize, ix;

	if ((NULL == ciph) || (NULL == mes)) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_ostr_init(ciph);

	des_permute_key(&pkey, key);
	des_encrypt_schedule(ksched, &pkey);

	ft_memcpy(ivbuf, iv, 8);
	messize = mes->size;
	padsize = 8 - (messize % 8);
	cisize = messize + padsize;
	SSL_ALLOC(ciptr, cisize);

	ix = 0;
	while (ix < messize) {
		ciptr[ix] = mes->content[ix];
		ix++;
	}
	while (ix < cisize) {
		ciptr[ix++] = padsize;
	}
	ix = 0;
	while (ix < cisize) {
		*(uint64_t *)(ciptr + ix) ^= *(uint64_t *)(ivbuf);

		des_permute_block_init((uint64_t *)(ciptr + ix));
		des_permute_block((uint64_t *)(ciptr + ix), ksched);
		des_permute_block_final((uint64_t *)(ciptr + ix));

		*(uint64_t *)(ciptr + ix) = ft_uint_bswap64(*(uint64_t *)(ciptr + ix));
		*(uint64_t *)(ivbuf) = *(uint64_t *)(ciptr + ix);

		ix += 8;
	}
	ciph->content = ciptr;
	ciph->size = cisize;

	return (SSL_OK);
}
