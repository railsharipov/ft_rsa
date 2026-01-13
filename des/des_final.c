/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_final.c                                        :+:      :+:    :+:   */
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

static ssize_t __encrypt_final(t_des *des, char *out, size_t size);
static ssize_t __decrypt_final(t_des *des, char *out, size_t size);

ssize_t des_final(t_des *des, char *out, size_t size)
{
	DES_LOG(TRACE, "final start");
	if (NULL == des || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (des->mode == DES_MODE_DECRYPT) {
		return (__decrypt_final(des, out, size));
	}
	else {
		return (__encrypt_final(des, out, size));
	}
}

static ssize_t __encrypt_final(t_des *des, char *out, size_t size)
{
	DES_LOG(TRACE, "encrypt final start");
	if (NULL == des || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (des->bufsize > DES_BLOCK_SIZE) {
		DES_LOG(ERROR, UNEXPECTED_ERROR);
		return (-1);
	}
	// Check if output buffer has space for one block
	if (size < DES_BLOCK_SIZE) {
		DES_LOG(ERROR, "output buffer too small");
		return (-1);
	}
	// Pad up to DES block size. Every octet of padding must be equal to padding size.
	DES_LOG(TRACE, "pad len: %d", DES_BLOCK_SIZE - des->bufsize);
	ft_memset(des->buf + des->bufsize, (int)(DES_BLOCK_SIZE - des->bufsize), DES_BLOCK_SIZE - des->bufsize);

	// Encrypt padded block.
	DES_LOG(DEBUG, "processing final block");
	des->f_permute_block(des, (uint64_t *)des->buf);

	ft_memcpy(out, des->buf, DES_BLOCK_SIZE);

	DES_LOG(TRACE, "encrypt final finish: wrote %d bytes", DES_BLOCK_SIZE);
	return (DES_BLOCK_SIZE);
}

static ssize_t __decrypt_final(t_des *des, char *out, size_t size)
{
	size_t	to_write, padsize;

	DES_LOG(TRACE, "decrypt final start");
	if (NULL == des || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	// At this point we expect buffer to have complete final block.
	if (des->bufsize != DES_BLOCK_SIZE) {
		DES_LOG(ERROR, "bad cipher size");
		return (-1);
	}
	// Process final block.
	DES_LOG(DEBUG, "processing final block");
	des->f_permute_block(des, (uint64_t *)des->buf);

	padsize = des->buf[DES_BLOCK_SIZE - 1];
	DES_LOG(TRACE, "pad len: %d", padsize);
	if (padsize == 0 || padsize > DES_BLOCK_SIZE) {
		DES_LOG(ERROR, "bad cipher pad size");
		return (-1);
	}
	// Write last block without pad octets.
	to_write = DES_BLOCK_SIZE - padsize;

	// Check if output buffer has space
	if (size < to_write) {
		DES_LOG(ERROR, "output buffer too small");
		return (-1);
	}

	ft_memcpy(out, des->buf, to_write);

	des->bufsize = 0;
	des->messize += to_write;

	DES_LOG(TRACE, "decrypt final finish: wrote %zu bytes", to_write);
	return (to_write);
}
