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

static int		__is_salted;

static unsigned char	*__salt;
static unsigned char	*__key;

static uint64_t	__permut_key;
static uint64_t	__ksched[16];

static int	__vectors(uint32_t vflag, const char *pass);
static void	__encrypt(const unsigned char *mes, size_t messize, unsigned char **ciph, size_t *ciphsize);

int des_ecb_encrypt(t_des *des, t_ostring *mes, t_ostring *ciph, const char *pass)
{
	if ((NULL == des) || (NULL == ciph) || (NULL == mes)) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ciph->content = NULL;
	__salt = des->salt;
	__key = des->key;

	if (SSL_OK != __vectors(des->vflag, pass)) {
		DES_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	des_permute_key(&__permut_key, __key);
	des_encrypt_schedule(__ksched, &__permut_key);

	__encrypt((unsigned char *)(mes->content), mes->size, &ciph->content, &ciph->size);

	return (SSL_OK);
}

static int	__vectors(uint32_t vflag, const char *pass)
{
	uint64_t rand_seed;

	__is_salted = 0;

	if (!SSL_FLAG(DES_K, vflag)) {
		if (!SSL_FLAG(DES_S, vflag)) {
			if (SSL_OK != rand_useed(&rand_seed, sizeof(uint64_t))) {
				DES_LOG(ERROR, UNSPECIFIED_ERROR);
				return (SSL_ERR);
			}
			if (SSL_OK != rand_bytes(rand_seed, __salt, 8)) {
				DES_LOG(ERROR, UNSPECIFIED_ERROR);
				return (SSL_ERR);
			}
			__is_salted = 1;
		}
		if (SSL_OK != rand_openssl_kdf(__key, __salt, NULL, pass)) {
			DES_LOG(ERROR, UNSPECIFIED_ERROR);
			return (SSL_ERR);
		}
	}
	return (SSL_OK);
}

static void	__encrypt(const unsigned char *mes, size_t messize, unsigned char **ciph, size_t *ciphsize)
{
	size_t	ix;
	unsigned char	padsize;
	unsigned char	**ciph_ptr;

	ciph_ptr = ciph;
	padsize = 8 - (messize % 8);
	*ciphsize = messize + padsize + (16 * __is_salted);
	SSL_ALLOC(*ciph_ptr, *ciphsize);

	ix = 0;
	if (__is_salted) {
		ft_memcpy(*ciph_ptr, "Salted__", 8);
		ft_memcpy(*ciph_ptr + 8, __salt, 8);
		ix += 16;
	}
	while (messize-- > 0) {
		(*ciph_ptr)[ix++] = *mes++;
	}
	while (ix < *ciphsize) {
		(*ciph_ptr)[ix++] = padsize;
	}
	ix = 0 + (16 * __is_salted);
	while (ix < *ciphsize) {
		des_permute_block_init((uint64_t *)*ciph_ptr + ix);
		des_permute_block((uint64_t *)*ciph_ptr + ix, __ksched);
		des_permute_block_final((uint64_t *)*ciph_ptr + ix);

		*(uint64_t *)(*ciph_ptr + ix) = ft_uint_bswap64(*(uint64_t *)(*ciph_ptr + ix));
		ix += 8;
	}
}
