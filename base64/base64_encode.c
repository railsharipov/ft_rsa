#include <common.h>
#include <logger.h>
#include <base64.h>
#include <libft.h>

static const char	SM[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '+', '/'
};

static void	__encode_block(const uint8_t mesblock[3], uint8_t encblock[4])
{
	encblock[0] = SM[( ( mesblock[0]>>2 )&0x3F )];
	encblock[1] = SM[( ( mesblock[0]<<4 )&0x30 )|( ( mesblock[1]>>4 )&0xF )];
	encblock[2] = SM[( ( mesblock[1]<<2 )&0x3C )|( ( mesblock[2]>>6 )&0x3 )];
	encblock[3] = SM[( mesblock[2]&0x3F )];
}

static void	__encode_final_block(const uint8_t *mesblock, size_t mesblock_size, uint8_t encblock[4])
{
	if (mesblock_size == 1) {
		encblock[0] = SM[( ( mesblock[0]>>2 )&0x3F )];
		encblock[1] = SM[( ( mesblock[0]<<4 )&0x30 )];
		encblock[2] = '=';
		encblock[3] = '=';
	}
	else if (mesblock_size == 2) {
		encblock[0] = SM[( ( mesblock[0]>>2 )&0x3F )];
		encblock[1] = SM[( ( mesblock[0]<<4 )&0x30 )|( ( mesblock[1]>>4 )&0xF )];
		encblock[2] = SM[( ( mesblock[1]<<2 )&0x3C )];
		encblock[3] = '=';
	}
}

t_transform_result base64_encode_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_b64_ctx *ctx = vctx;

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(DEBUG, "transform validation: TRANSFORM_ERROR");
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(DEBUG, "transform validation: already TRANSFORM_DONE");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *mesblock = in;
	uint8_t *encblock = out;
	size_t i = 0;
	size_t j = 0;

	if (insize < B64_MES_BLOCK_SIZE) {
		SSL_LOG(DEBUG, "transform update: TRANSFORM_NEED_INPUT");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_INPUT};
	}
	if (outsize < B64_ENC_BLOCK_SIZE) {
		SSL_LOG(DEBUG, "transform update: TRANSFORM_NEED_OUTPUT");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}
	while (i+2 < insize && j+3 < outsize) {
		__encode_block(mesblock + i, encblock + j);
		i += B64_MES_BLOCK_SIZE;
		j += B64_ENC_BLOCK_SIZE;
	}
	return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_OK};
}

t_transform_result base64_encode_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_b64_ctx *ctx = vctx;

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(DEBUG, "transform validation: TRANSFORM_ERROR");
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(DEBUG, "transform validation: already TRANSFORM_DONE");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *mesblock = in;
	uint8_t *encblock = out;
	size_t i = 0;
	size_t j = 0;

	while (i+2 < insize && j+3 < outsize) {
		__encode_block(mesblock + i, encblock + j);
		i += B64_MES_BLOCK_SIZE;
		j += B64_ENC_BLOCK_SIZE;
	}
	if (i + B64_MES_BLOCK_SIZE >= insize) {
		if (j + B64_ENC_BLOCK_SIZE < outsize) {
			__encode_final_block(mesblock + i, insize%B64_MES_BLOCK_SIZE, encblock + j);
			i += insize%B64_MES_BLOCK_SIZE;
			j += B64_ENC_BLOCK_SIZE;
			ctx->done = 1;

			SSL_LOG(DEBUG, "transform final: TRANSFORM_DONE");
			return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_DONE};
		}
		else {
			SSL_LOG(DEBUG, "transform update: TRANSFORM_NEED_OUTPUT");
			return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
		}
	}
	return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_OK};
}
