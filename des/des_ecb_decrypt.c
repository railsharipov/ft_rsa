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

static int	__vectors(const unsigned char *ciph, size_t ciphsize, uint32_t vflag, const char *pass);
static int	__decrypt(const unsigned char *ciph, size_t ciphsize, unsigned char **mes, size_t *messize);
static int	__remove_pad(unsigned char **mes, size_t *messize);

int des_ecb_decrypt(t_des *des, t_ostring *ciph, t_ostring *mes, const char *pass)
{
	if ((NULL == des) || (NULL == ciph) || (NULL == mes)) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	mes->content = NULL;

	if (ciph->size % DES_MES_BLOCK_SIZE != 0) {
		DES_LOG(ERROR, "invalid des encryption");
		return (SSL_ERR);
	}
	__salt = des->salt;
	__key = des->key;

	if (SSL_OK != __vectors((unsigned char *)(ciph->content), ciph->size, des->vflag, pass)) {
		DES_LOG(ERROR, "invalid des encryption");
		return (SSL_ERR);
	}

	des_permute_key(&__permut_key, __key);
	des_decrypt_schedule(__ksched, &__permut_key);

	return (__decrypt((unsigned char *)(ciph->content), ciph->size, &(mes->content), &(mes->size)));
}

static int	__remove_pad(unsigned char **mes, size_t *messize)
{
	unsigned char	padsize;
	unsigned char	ix;

	if (*messize == 0) {
		DES_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	if ((padsize = (*mes)[*messize-1]) > 8) {
		DES_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}

	ix = 0;
	while (ix++ < padsize) {
		*messize -= 1;
		if ((*mes)[*messize] != padsize) {
			DES_LOG(ERROR, UNSPECIFIED_ERROR);
			return (SSL_ERR);
		}
	}
	return (SSL_OK);
}

static int	__vectors(const unsigned char *ciph, size_t ciphsize, uint32_t vflag, const char *pass)
{
	__is_salted = 0;

	if (!SSL_FLAG(DES_K, vflag)) {
		if (!SSL_FLAG(DES_S, vflag)) {
			if (ciphsize < 16) {
				DES_LOG(ERROR, UNSPECIFIED_ERROR);
				return (SSL_ERR);
			}
			if (ft_strncmp((char *)ciph, "Salted__", 8)) {
				DES_LOG(ERROR, UNSPECIFIED_ERROR);
				return (SSL_ERR);
			}
			ft_memcpy(__salt, ciph + 8, 8);
			__is_salted = 1;
		}
		if (SSL_OK != rand_openssl_kdf(__key, __salt, NULL, pass)) {
			DES_LOG(ERROR, UNSPECIFIED_ERROR);
			return (SSL_ERR);
		}
	}
	return (SSL_OK);
}

static int	__decrypt(const unsigned char *ciph, size_t ciphsize, unsigned char **mes, size_t *messize)
{
	size_t	ix;
	unsigned char **mes_ptr;

	mes_ptr = mes;
	*messize = ciphsize - (__is_salted*16);
	SSL_ALLOC(*mes_ptr, *messize);

	if (__is_salted) {
		ciph += 16;
		ciphsize -= 16;
	}
	ix = 0;
	while (ix < ciphsize) {
		(*mes_ptr)[ix] = *ciph++;
		ix++;
	}
	ix = 0;
	while (ix < *messize) {
		des_permute_block_init((uint64_t *)*mes_ptr + ix);
		des_permute_block((uint64_t *)*mes_ptr + ix, __ksched);
		des_permute_block_final((uint64_t *)*mes_ptr + ix);

		*(uint64_t *)(*mes_ptr + ix) = ft_uint_bswap64(*(uint64_t *)(*mes_ptr + ix));
		ix += 8;
	}
	if (SSL_OK != __remove_pad(mes_ptr, messize)) {
		DES_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}
