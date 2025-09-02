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

static int __encrypt_update(t_des *des, t_iodes *in, t_iodes *out);
static int __decrypt_update(t_des *des, t_iodes *in, t_iodes *out);

int des_update(t_des *des, t_iodes *in, t_iodes *out)
{
	if (NULL == des || NULL == in || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (des->mode == DES_MODE_DECRYPT) {
		return (__decrypt_update(des, in, out));
	}
	else {
		return (__encrypt_update(des, in, out));
	}
}

static int __encrypt_update(t_des *des, t_iodes *in, t_iodes *out)
{
	ssize_t	rbytes;
	size_t	to_read;

	if (NULL == des || NULL == in || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (des->bufsize > DES_BLOCK_SIZE) {
		DES_LOG(ERROR, UNEXPECTED_ERROR);
		return (SSL_ERR);
	}
	// If remaining message octets fill buffer before process.
	if (des->bufsize > 0) {
		to_read = DES_BLOCK_SIZE - des->bufsize;
		if ((rbytes = io_read(in, (char *)des->buf + des->bufsize, to_read)) != to_read) {
			if (rbytes < 0) {
				DES_LOG(ERROR, IO_READ_ERROR);
				return (SSL_ERR);
			} else {
				// Block was not filled so nothing to do.
				des->bufsize += rbytes;
				return (SSL_OK);
			}
		}
		des->messize += to_read;
		des->bufsize += to_read;

		des->f_permute_block(des, (uint64_t *)des->buf);

		if (io_write(out, (char *)des->buf, DES_BLOCK_SIZE) != DES_BLOCK_SIZE) {
			DES_LOG(ERROR, IO_WRITE_ERROR);
			return (SSL_ERR);
		}
		des->bufsize = 0;
	}
	// Process message octets per block size.
	while ((rbytes = io_read(in, (char *)des->buf, DES_BLOCK_SIZE)) > 0) {
		des->messize += rbytes;
		des->bufsize = rbytes;

		if (rbytes == DES_BLOCK_SIZE) {
			des->f_permute_block(des, (uint64_t *)des->buf);

			if (io_write(out, (char *)des->buf, DES_BLOCK_SIZE) != DES_BLOCK_SIZE) {
				DES_LOG(ERROR, IO_WRITE_ERROR);
				return (SSL_ERR);
			}
		}
		else {
			break;
		}
	}
	if (rbytes < 0) {
		DES_LOG(ERROR, IO_READ_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int __decrypt_update(t_des *des, t_iodes *in, t_iodes *out)
{
	uint8_t	prev[DES_BLOCK_SIZE];
	ssize_t	rbytes;
	size_t	to_read, prevsize;

	if (NULL == des || NULL == in || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (des->bufsize > DES_BLOCK_SIZE) {
		DES_LOG(ERROR, UNEXPECTED_ERROR);
		return (SSL_ERR);
	}
	// Fill the buffer up to block size.
	to_read = DES_BLOCK_SIZE - des->bufsize;

	if ((rbytes = io_read(in, (char *)des->buf + des->bufsize, to_read)) != to_read) {
		if (rbytes < 0) {
			DES_LOG(ERROR, IO_READ_ERROR);
			return (SSL_ERR);
		}
		else {
			// Buffer was not filled so nothing to do.
			des->bufsize += rbytes;
			return (SSL_OK);
		}
	}
	des->bufsize = DES_BLOCK_SIZE;

	// We need to process blocks with 1 block delay by using buffer for previous block.
	ft_memcpy(prev, des->buf, des->bufsize);
	prevsize = des->bufsize;
	
	// On each iteration read current block and process previous block.
	while ((rbytes = io_read(in, (char *)des->buf, DES_BLOCK_SIZE)) > 0) {
		des->bufsize = rbytes;

		// Process previous buffer.
		des->f_permute_block(des, (uint64_t *)prev);
			
		if (io_write(out, (char *)prev, DES_BLOCK_SIZE) != DES_BLOCK_SIZE) {
			DES_LOG(ERROR, IO_WRITE_ERROR);
			return (SSL_ERR);
		}
		des->messize += DES_BLOCK_SIZE;

		// Copy current buffer octets to previous buffer.
		ft_memcpy(prev, des->buf, des->bufsize);
		prevsize = des->bufsize;
	}
	if (rbytes < 0) {
		DES_LOG(ERROR, IO_READ_ERROR);
		return (SSL_ERR);
	}
	// Copy remaining block octets into the buffer.
	ft_memcpy(des->buf, prev, prevsize);
	des->bufsize = prevsize;

	return (SSL_OK);
}
