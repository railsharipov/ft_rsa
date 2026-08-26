/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rand_openssl_kdf.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/21 15:19:23 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 09:56:29 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <common.h>
#include <logger.h>
#include <des.h>
#include <digest.h>
#include <rand.h>
#include <pwd.h>

/*
** Implements OpenSSL's proprietary key derivation function (EVP_BytesToKey with MD5).
** This is NOT PBKDF2.
** It derives a key and optionally an IV from a password and a salt.
** The salt must be 8 bytes. The derived key is 8 bytes, and the IV is 8 bytes.
*/

int	rand_openssl_kdf(unsigned char *key, unsigned char *salt, unsigned char *vect, const char *pass)
{
	t_hash	md5;
	char	buf[160] = {0};
	size_t	pass_len;

	if (NULL == salt) {
		SSL_LOG(ERROR, "salt is required");
		return (SSL_ERR);
	}
	if (NULL == pass) {
		SSL_LOG(ERROR, "password is required");
		return (SSL_ERR);
	}

	pass_len = ft_strlen(pass);
	ft_memcpy(buf, pass, pass_len);
	ft_memcpy(buf + pass_len, salt, 8);

	md5_init(&md5);
	md5_update(&md5, (unsigned char *)buf, pass_len + 8);
	md5_final(&md5);

	ft_memcpy(key, md5.hash, 8);

	if (NULL != vect) {
		ft_memcpy(vect, md5.hash + 8, 8);
	}
	ft_bzero(md5.hash, md5.hashsize);

	return (SSL_OK);
}
