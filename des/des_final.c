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

static int __encrypt_final(t_des *des, t_iodes *out);
static int __decrypt_final(t_des *des, t_iodes *out);

int des_final(t_des *des, t_iodes *out)
{
	if (NULL == des || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (des->mode == DES_MODE_DECRYPT) {
		return (__decrypt_final(des, out));
	}
	else {
		return (__encrypt_final(des, out));
	}
}

static int __encrypt_final(t_des *des, t_iodes *out)
{
	if (NULL == des || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (des->bufsize > DES_BLOCK_SIZE) {
		DES_LOG(ERROR, UNEXPECTED_ERROR);
		return SSL_ERR;
	}
	// Pad up to DES block size. Every octet of padding must be equal to padding size.
	ft_memset(des->buf + des->bufsize, (int)(DES_BLOCK_SIZE - des->bufsize), DES_BLOCK_SIZE - des->bufsize);

	// Encrypt padded block.
	des->f_permute_block(des, (uint64_t *)des->buf);
	
	if (io_write(out, (char *)des->buf, DES_BLOCK_SIZE) != DES_BLOCK_SIZE) {
		DES_LOG(ERROR, IO_WRITE_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int __decrypt_final(t_des *des, t_iodes *out)
{
	size_t	to_write, padsize;

	if (NULL == des || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	// At this point we expect buffer to have complete final block.
	if (des->bufsize != DES_BLOCK_SIZE) {
		DES_LOG(ERROR, "bad cipher size");
		return SSL_ERR;
	}
	// Process final block.
	des->f_permute_block(des, (uint64_t *)des->buf);

	padsize = des->buf[DES_BLOCK_SIZE - 1];
	if (padsize == 0 || padsize > DES_BLOCK_SIZE) {
		DES_LOG(ERROR, "bad cipher pad size");
		return SSL_ERR;
	}
	// Write last block without pad octets.
	to_write = DES_BLOCK_SIZE - padsize;
	if (io_write(out, (char *)des->buf, to_write) != to_write) {
		DES_LOG(ERROR, IO_WRITE_ERROR);
		return (SSL_ERR);
	}
	des->bufsize = 0;
	des->messize += to_write;

	return (SSL_OK);
}