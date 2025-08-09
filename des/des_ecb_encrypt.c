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

static unsigned char	*__key;

static uint64_t	__permut_key;
static uint64_t	__ksched[16];

static void	__encrypt(const unsigned char *mes, size_t messize, unsigned char **ciph, size_t *ciphsize);

int des_ecb_encrypt(t_des *des, t_ostring *mes, t_ostring *ciph)
{
	if ((NULL == des) || (NULL == ciph) || (NULL == mes)) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ciph->content = NULL;
	__key = des->key;

	des_permute_key(&__permut_key, __key);
	des_encrypt_schedule(__ksched, &__permut_key);

	__encrypt((unsigned char *)(mes->content), mes->size, &ciph->content, &ciph->size);

	return (SSL_OK);
}

static void	__encrypt(const unsigned char *mes, size_t messize, unsigned char **ciph, size_t *ciphsize)
{
	size_t	ix;
	unsigned char	padsize;
	unsigned char	**ciph_ptr;

	ciph_ptr = ciph;
	padsize = 8 - (messize % 8);
	*ciphsize = messize + padsize;
	SSL_ALLOC(*ciph_ptr, *ciphsize);

	ix = 0;
	while (messize-- > 0) {
		(*ciph_ptr)[ix++] = *mes++;
	}
	while (ix < *ciphsize) {
		(*ciph_ptr)[ix++] = padsize;
	}
	ix = 0;
	while (ix < *ciphsize) {
		des_permute_block_init((uint64_t *)(*ciph_ptr + ix));
		des_permute_block((uint64_t *)(*ciph_ptr + ix), __ksched);
		des_permute_block_final((uint64_t *)(*ciph_ptr + ix));

		*(uint64_t *)(*ciph_ptr + ix) = ft_uint_bswap64(*(uint64_t *)(*ciph_ptr + ix));
		ix += 8;
	}
}
