/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base64_encode.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/28 17:44:36 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/12 10:45:14 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <common.h>
#include <base64.h>
#include <bnum.h>

int	base64_init(t_b64 *base64, t_b64_mode mode)
{
	if (NULL == base64) {
		B64_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_bzero(base64, sizeof(t_b64));
	base64->mode = mode;
	base64->blocksize = (mode == B64_MODE_ENCODE) ? B64_ENC_BLOCK_SIZE : B64_MES_BLOCK_SIZE;
	base64->f_process_block = (mode == B64_MODE_ENCODE) ? base64_encode_block : base64_decode_block;

	return (SSL_OK);
}
