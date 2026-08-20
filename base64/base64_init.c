#include <common.h>
#include <logger.h>
#include <base64.h>

int	base64_init(t_b64 *base64, t_b64_mode mode)
{
	if (NULL == base64) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_bzero(base64, sizeof(t_b64));
	base64->mode = mode;
	base64->blocksize = (mode == B64_MODE_ENCODE) ? B64_ENC_BLOCK_SIZE : B64_MES_BLOCK_SIZE;
	base64->f_process_block = (mode == B64_MODE_ENCODE) ? base64_encode_block : base64_decode_block;

	return (SSL_OK);
}
