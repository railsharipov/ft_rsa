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
	DES_LOG(TRACE, "update start");
	if (NULL == des || NULL == in || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (des->mode == DES_MODE_DECRYPT) {
		DES_LOG(DEBUG, "update: decrypting cipher");
		return (__decrypt_update(des, in, out));
	}
	else {
		DES_LOG(DEBUG, "update: encrypting plaintext");
		return (__encrypt_update(des, in, out));
	}
	DES_LOG(TRACE, "update finish");
}

static int __encrypt_update(t_des *des, t_iodes *in, t_iodes *out)
{
	ssize_t	rbytes;

	if (NULL == des || NULL == in || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (des->bufsize > DES_BLOCK_SIZE) {
		DES_LOG(ERROR, UNEXPECTED_ERROR);
		return (SSL_ERR);
	}
	// Process message blocks. Buffer may be partially filled, only read up to block size.
	while ((rbytes = io_read(in, (char *)des->buf + des->bufsize, DES_BLOCK_SIZE - des->bufsize)) > 0) {
		DES_LOG(TRACE, "read %ld bytes", rbytes);
		des->messize += rbytes;
		des->bufsize += rbytes;

		if (des->bufsize != DES_BLOCK_SIZE) {
			break;
		}
		DES_LOG(TRACE, "processing block");
		des->f_permute_block(des, (uint64_t *)des->buf);

		if (io_write(out, (char *)des->buf, des->bufsize) != DES_BLOCK_SIZE) {
			DES_LOG(ERROR, IO_WRITE_ERROR);
			return (SSL_ERR);
		}
		des->bufsize = 0;
	}
	if (rbytes < 0) {
		DES_LOG(ERROR, IO_READ_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int __decrypt_update(t_des *des, t_iodes *in, t_iodes *out)
{
	uint8_t buf[8];
	ssize_t	rbytes;

	if (NULL == des || NULL == in || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (des->bufsize > DES_BLOCK_SIZE) {
		DES_LOG(ERROR, UNEXPECTED_ERROR);
		return (SSL_ERR);
	}
	// Fill the buffer up to block size.
	if ((rbytes = io_read(in, (char *)des->buf + des->bufsize, DES_BLOCK_SIZE - des->bufsize)) < 0) {
		DES_LOG(ERROR, IO_READ_ERROR);
		return (SSL_ERR);
	}
	DES_LOG(TRACE, "read %ld bytes", rbytes);
	rbytes += des->bufsize;

	// Process blocks with 1 block delay.
	while (rbytes == DES_BLOCK_SIZE) {
		// Read next block.
		if ((rbytes = io_read(in, (char *)buf, DES_BLOCK_SIZE)) < 0) {
			DES_LOG(ERROR, IO_READ_ERROR);
			return (SSL_ERR);
		}
		DES_LOG(TRACE, "read %ld bytes", rbytes);
		// Process previous block if there is next block.
		if (rbytes > 0) {
			DES_LOG(TRACE, "processing block");
			des->f_permute_block(des, (uint64_t *)des->buf);
			
			if (io_write(out, (char *)des->buf, DES_BLOCK_SIZE) != DES_BLOCK_SIZE) {
				DES_LOG(ERROR, IO_WRITE_ERROR);
				return (SSL_ERR);
			}
			des->messize += DES_BLOCK_SIZE;

			// Next block becomes previous block.
			*(uint64_t *)des->buf = *(uint64_t *)buf;
			des->bufsize = rbytes;
		}
	}
	return (SSL_OK);
}
