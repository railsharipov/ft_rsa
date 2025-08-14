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

int des_cbc_decrypt(const uint8_t key[8], const uint8_t iv[8], t_ostring *ciph, t_ostring *mes)
{
	uint64_t	ksched[16];
	uint64_t	pkey;
	uint8_t		ivbuf[8], *mesptr, temp[8];
	size_t		padsize, messize;
	size_t		ix;

	if ((NULL == ciph) || (NULL == mes)) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (ciph->size == 0 || ciph->size % DES_MES_BLOCK_SIZE != 0) {
		DES_LOG(ERROR, "bad cipher size");
		return (SSL_ERR);
	}

	ft_ostr_init(mes);
	ft_memcpy(ivbuf, iv, 8);

	des_permute_key(&pkey, key);
	des_decrypt_schedule(ksched, &pkey);

	messize = ciph->size;
	SSL_ALLOC(mesptr, messize);

	ix = 0;
	while (ix < messize) {
		mesptr[ix] = ciph->content[ix];
		ix++;
	}
	ix = 0;
	while (ix < messize) {
		*(uint64_t *)(temp) = *(uint64_t *)(mesptr + ix);

		des_permute_block_init((uint64_t *)(mesptr + ix));
		des_permute_block((uint64_t *)(mesptr + ix), ksched);
		des_permute_block_final((uint64_t *)(mesptr + ix));

		*(uint64_t *)(mesptr + ix) = ft_uint_bswap64(*(uint64_t *)(mesptr + ix));
		*(uint64_t *)(mesptr + ix) ^= *(uint64_t *)(ivbuf);
		*(uint64_t *)(ivbuf) = *(uint64_t *)(temp);
		ix += 8;
	}
	padsize = mesptr[messize-1];

	if (padsize > 8) {
		DES_LOG(ERROR, "bad pad size");
		return (SSL_ERR);
	}
	ix = 0;
	while (ix++ < padsize) {
		messize -= 1;
		if (mesptr[messize] != padsize) {
			DES_LOG(ERROR, "bad pad byte");
			return (SSL_ERR);
		}
	}
	mes->content = mesptr;
	mes->size = messize;

	return (SSL_OK);
}
