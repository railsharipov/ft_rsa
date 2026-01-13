/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_update.c                                       :+:      :+:    :+:   */
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

static ssize_t __encrypt_update(t_des *des, const char *in, char *out, size_t size);
static ssize_t __decrypt_update(t_des *des, const char *in, char *out, size_t size);

ssize_t des_update(t_des *des, const char *in, char *out, size_t size)
{
	DES_LOG(TRACE, "update start");
	if (NULL == des || NULL == in || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (des->mode == DES_MODE_DECRYPT) {
		DES_LOG(DEBUG, "update: decrypting cipher");
		return (__decrypt_update(des, in, out, size));
	}
	else {
		DES_LOG(DEBUG, "update: encrypting plaintext");
		return (__encrypt_update(des, in, out, size));
	}
}

static ssize_t __encrypt_update(t_des *des, const char *in, char *out, size_t size)
{
	size_t	in_pos, out_pos;

	if (NULL == des || NULL == in || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (des->bufsize > DES_BLOCK_SIZE) {
		DES_LOG(ERROR, UNEXPECTED_ERROR);
		return (-1);
	}

	in_pos = 0;
	out_pos = 0;

	// Process message blocks. Buffer may be partially filled.
	while (in_pos < size) {
		// Fill buffer up to block size
		size_t to_copy = DES_BLOCK_SIZE - des->bufsize;
		if (to_copy > size - in_pos) {
			to_copy = size - in_pos;
		}
		ft_memcpy(des->buf + des->bufsize, in + in_pos, to_copy);
		des->bufsize += to_copy;
		des->messize += to_copy;
		in_pos += to_copy;

		// If buffer is full, process it
		if (des->bufsize == DES_BLOCK_SIZE) {
			DES_LOG(TRACE, "processing block");
			des->f_permute_block(des, (uint64_t *)des->buf);

			ft_memcpy(out + out_pos, des->buf, DES_BLOCK_SIZE);
			out_pos += DES_BLOCK_SIZE;
			des->bufsize = 0;
		}
	}

	DES_LOG(TRACE, "update finish: wrote %zu bytes", out_pos);
	return (out_pos);
}

static ssize_t __decrypt_update(t_des *des, const char *in, char *out, size_t size)
{
	size_t	in_pos, out_pos;

	if (NULL == des || NULL == in || NULL == out) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (des->bufsize > DES_BLOCK_SIZE) {
		DES_LOG(ERROR, UNEXPECTED_ERROR);
		return (-1);
	}
	in_pos = 0;
	out_pos = 0;

	// Process blocks with 1 block delay (to handle padding in final)
	while (in_pos < size) {
		// Fill buffer up to block size
		size_t to_copy = DES_BLOCK_SIZE - des->bufsize;
		if (to_copy > size - in_pos) {
			to_copy = size - in_pos;
		}
		ft_memcpy(des->buf + des->bufsize, in + in_pos, to_copy);
		des->bufsize += to_copy;
		in_pos += to_copy;

		// If buffer is full and there's more data, process the current block
		if (des->bufsize == DES_BLOCK_SIZE && in_pos < size) {
			DES_LOG(TRACE, "processing block");

			// Process it
			des->f_permute_block(des, (uint64_t *)des->buf);

			// Write to output
			ft_memcpy(out + out_pos, des->buf, DES_BLOCK_SIZE);
			out_pos += DES_BLOCK_SIZE;
			des->messize += DES_BLOCK_SIZE;

			// Clear buffer for next block
			des->bufsize = 0;
		}
	}
	DES_LOG(TRACE, "update finish: wrote %zu bytes", out_pos);
	return (out_pos);
}
