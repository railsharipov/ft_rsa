/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pbkdf2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/21 15:19:23 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 09:56:29 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_des.h>
#include <ssl_hash.h>
#include <ssl_rand.h>
#include <pwd.h>

static char	__buf[160];

/* Password based key derivation function - PBKDF2 */

int	rand_pbkdf2(unsigned char *key, unsigned char *salt, unsigned char *vect)
{
	t_hash	*md5;
	char	*bufptr;
	int		bufsize;
	char	*pass;

	if (NULL == salt)
		return (RAND_ERROR(INVALID_INPUT));
	if (NULL == (pass = ssl_getpass()))
		return (RAND_ERROR(EXPECTED_PASSWORD_INPUT));

	bufptr = __buf;
	ft_memcpy(bufptr, pass, ft_strlen(pass));
	bufsize += ft_strlen(pass);
	ft_memcpy(bufptr + bufsize, salt, 8);
	bufsize += 8;

	md5 = hash_md5_init();
	hash_md5_update(md5, bufptr, FLOOR(bufsize, MD5_BLOCK_SIZE));

	bufptr += FLOOR(bufsize, MD5_BLOCK_SIZE);
	hash_md5_final(md5, bufptr, (bufsize % MD5_BLOCK_SIZE));

	ft_bzero(__buf, sizeof(__buf));
	ft_memcpy(key, md5->hash, 8);

	if (NULL != vect)
		ft_memcpy(vect, md5->hash + 8, 8);
	ft_bzero(md5->hash, md5->size);

	return (SSL_OK);
}
