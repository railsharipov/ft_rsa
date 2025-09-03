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
#include <des.h>

int	des_process_ostr(t_des *des, t_ostring *input, t_ostring *output) {
	t_iodes	in, out;

	if (NULL == des || NULL == input || NULL == output) {
		DES_LOG(ERROR, INVALID_INPUT_ERROR);
	}
	if (SSL_OK != io_osbuf(&in, IO_READ, input)) {
		DES_LOG(ERROR, IO_INIT_ERROR);
	}
	if (SSL_OK != io_osbuf(&out, IO_WRITE, output)) {
		DES_LOG(ERROR, IO_INIT_ERROR);
	}
	if (SSL_OK != des_update(des, &in, &out)) {
		DES_LOG(ERROR, "bad crypt");
		goto label_err;
	}
	if (SSL_OK != des_final(des, &out)) {
		DES_LOG(ERROR, "bad crypt");
		goto label_err;
	}
	return (SSL_OK);

label_err:
	ft_ostr_clear(output);
	return (SSL_ERR);
}