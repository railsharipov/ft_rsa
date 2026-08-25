#include <common.h>
#include <logger.h>
#include <base64.h>
#include <libft.h>

// base64 tail byte character is '='
#define __B64_TAIL_CHAR 61

static const char	SM[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '+', '/'
};

static inline void	__encode_block(const uint8_t mesblock[3], uint8_t encblock[4])
{
	encblock[0] = SM[( ( mesblock[0]>>2 )&0x3F )];
	encblock[1] = SM[( ( mesblock[0]<<4 )&0x30 )|( ( mesblock[1]>>4 )&0xF )];
	encblock[2] = SM[( ( mesblock[1]<<2 )&0x3C )|( ( mesblock[2]>>6 )&0x3 )];
	encblock[3] = SM[( mesblock[2]&0x3F )];
}

static inline void	__encode_final_block(const uint8_t *mesblock, size_t mesblock_size, uint8_t encblock[4])
{
	if (mesblock_size == 1) {
		encblock[0] = SM[( ( mesblock[0]>>2 )&0x3F )];
		encblock[1] = SM[( ( mesblock[0]<<4 )&0x30 )];
		encblock[2] = __B64_TAIL_CHAR;
		encblock[3] = __B64_TAIL_CHAR;
	}
	else if (mesblock_size == 2) {
		encblock[0] = SM[( ( mesblock[0]>>2 )&0x3F )];
		encblock[1] = SM[( ( mesblock[0]<<4 )&0x30 )|( ( mesblock[1]>>4 )&0xF )];
		encblock[2] = SM[( ( mesblock[1]<<2 )&0x3C )];
		encblock[3] = __B64_TAIL_CHAR;
	}
	else if (mesblock_size == 3) {
		encblock[0] = SM[( ( mesblock[0]>>2 )&0x3F )];
		encblock[1] = SM[( ( mesblock[0]<<4 )&0x30 )|( ( mesblock[1]>>4 )&0xF )];
		encblock[2] = SM[( ( mesblock[1]<<2 )&0x3C )|( ( mesblock[2]>>6 )&0x3 )];
		encblock[3] = SM[( mesblock[2]&0x3F )];
	}
}

t_transform_result base64_encode_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_b64_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "encoding is already done");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *mesblock = in;
	uint8_t *encblock = out;
	size_t i = 0;
	size_t j = 0;

	if (insize < B64_MES_BLOCK_SIZE) {
		SSL_LOG(TRACE, "need input");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_INPUT};
	}
	if (outsize < B64_ENC_BLOCK_SIZE) {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}
	while (i+B64_MES_BLOCK_SIZE <= insize && j+B64_ENC_BLOCK_SIZE <= outsize) {
		__encode_block(mesblock + i, encblock + j);
		i += B64_MES_BLOCK_SIZE;
		j += B64_ENC_BLOCK_SIZE;
	}
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_OK};
}

t_transform_result base64_encode_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_b64_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "already complete");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}
	if (insize == 0) {
		SSL_LOG(TRACE, "no input, done");
		ctx->done = 1;
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *mesblock = in;
	uint8_t *encblock = out;
	size_t i = 0;
	size_t j = 0;

	while (i+B64_MES_BLOCK_SIZE < insize && j+B64_ENC_BLOCK_SIZE < outsize) {
		__encode_block(mesblock + i, encblock + j);
		i += B64_MES_BLOCK_SIZE;
		j += B64_ENC_BLOCK_SIZE;
	}
	if (j + B64_ENC_BLOCK_SIZE < outsize) {
		SSL_LOG(TRACE, "final block size %zu", insize-i);
		__encode_final_block(mesblock + i, insize-i, encblock + j);
		i += insize-i;
		j += B64_ENC_BLOCK_SIZE;

		ctx->done = 1;
		SSL_LOG(TRACE, "encoding is complete");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_DONE};
	}
	else {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}
}

int	base64_encode_all(const uint8_t *mes, size_t messize, uint8_t **enc, size_t *encsize)
{
	uint8_t	*omes;
	uint8_t	*oenc;

	SSL_LOG(TRACE, "encoding %zu bytes of message", messize);

	if ((NULL == mes) || (NULL == enc) || (NULL == encsize)) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	omes = (uint8_t *)mes;
	*encsize = CEIL_TO_MULTIPLE(messize, B64_MES_BLOCK_SIZE)/B64_MES_BLOCK_SIZE * B64_ENC_BLOCK_SIZE;
	SSL_ALLOC(*enc, *encsize);
	oenc = *enc;

	if (messize > 0) {
		while (messize > B64_MES_BLOCK_SIZE) {
			__encode_block(omes, oenc);
			omes += B64_MES_BLOCK_SIZE;
			oenc += B64_ENC_BLOCK_SIZE;
			messize -= B64_MES_BLOCK_SIZE;
		}
		__encode_final_block(omes, messize, oenc);
	}
	SSL_LOG(TRACE, "encoded %zu bytes of message", *encsize);

	return (SSL_OK);
}

static const char	UB[128] = {
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	0,	0,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	0,	0,

	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	0,
	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	0
};

static const uint8_t	BM[128] = {
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	62,	0,	0,	0,	63,
	52,	53,	54,	55,	56,	57,	58,	59,	60,	61,	0,	0,	0,	0,	0,	0,

	0,	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,
	15,	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	0,	0,	0,	0,	0,
	0,	26,	27,	28,	29,	30,	31,	32,	33,	34,	35,	36,	37,	38,	39,	40,
	41,	42,	43,	44,	45,	46,	47,	48,	49,	50,	51,	0,	0,	0,	0,	0
};

static inline int	__is_valid_block(const uint8_t encblock[4])
{
	// SSL_LOG(DEBUG, "validated block content: %u %u %u %u", encblock[0], encblock[1], encblock[2], encblock[3]);
	return UB[encblock[0]]+UB[encblock[1]]+UB[encblock[2]]+UB[encblock[3]] == 4;
}

static inline int	__is_valid_final_block(const uint8_t *encblock, size_t encblock_size)
{
	return encblock_size == B64_ENC_BLOCK_SIZE
		&& UB[encblock[0]] == 1
		&& UB[encblock[1]] == 1
		&& (UB[encblock[2]] == 1 || encblock[2] == __B64_TAIL_CHAR)
		&& (UB[encblock[3]] == 1 || encblock[3] == __B64_TAIL_CHAR);
}

static void	__decode_block(const uint8_t encblock[4], uint8_t mesblock[3])
{
	mesblock[0] = ( BM[encblock[0]&0x7F]<<2 )|( BM[encblock[1]&0x7F]>>4 );
	mesblock[1] = ( BM[encblock[1]&0x7F]<<4 )|( BM[encblock[2]&0x7F]>>2 );
	mesblock[2] = ( BM[encblock[2]&0x7F]<<6 )|( BM[encblock[3]&0x7F] );
}

static void	__decode_final_block(const uint8_t encblock[4], uint8_t *mesblock, size_t mesblock_size)
{
	if (mesblock_size == 1) {
		mesblock[0] = ( BM[encblock[0]&0x7F]<<2 )|( BM[encblock[1]&0x7F]>>4 );
	}
	else if (mesblock_size == 2) {
		mesblock[0] = ( BM[encblock[0]&0x7F]<<2 )|( BM[encblock[1]&0x7F]>>4 );
		mesblock[1] = ( BM[encblock[1]&0x7F]<<4 )|( BM[encblock[2]&0x7F]>>2 );
	}
	else if (mesblock_size == 3) {
		mesblock[0] = ( BM[encblock[0]&0x7F]<<2 )|( BM[encblock[1]&0x7F]>>4 );
		mesblock[1] = ( BM[encblock[1]&0x7F]<<4 )|( BM[encblock[2]&0x7F]>>2 );
		mesblock[2] = ( BM[encblock[2]&0x7F]<<6 )|( BM[encblock[3]&0x7F] );
	}
}

t_transform_result base64_decode_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_b64_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "encoding is already done");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *encblock = in;
	uint8_t *mesblock = out;
	size_t i = 0;
	size_t j = 0;

	// Ignore the last input block as it may be required for finalization.
	// Effectively, if input contains only 1 block, it will never be consumed.
	insize = MAX(insize, B64_ENC_BLOCK_SIZE) - B64_ENC_BLOCK_SIZE;

	if (insize < B64_ENC_BLOCK_SIZE) {
		SSL_LOG(TRACE, "need input");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_INPUT};
	}
	if (outsize < B64_MES_BLOCK_SIZE) {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}

	while (i+B64_ENC_BLOCK_SIZE <= insize && j+B64_MES_BLOCK_SIZE <= outsize) {
		if (!__is_valid_block(encblock + i)) {
			SSL_LOG(ERROR, "bad base64 encoding");
			return (t_transform_result){.status = TRANSFORM_ERROR};
		}
		__decode_block(encblock + i, mesblock + j);
		i += B64_ENC_BLOCK_SIZE;
		j += B64_MES_BLOCK_SIZE;
	}
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_OK};
}

t_transform_result base64_decode_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_b64_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "already complete");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}
	if (insize == 0) {
		SSL_LOG(TRACE, "no input, done");
		ctx->done = 1;
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *encblock = in;
	uint8_t *mesblock = out;
	size_t i = 0;
	size_t j = 0;

	while (i+B64_ENC_BLOCK_SIZE < insize && j+B64_MES_BLOCK_SIZE < outsize) {
		if (!__is_valid_block(encblock + i)) {
			SSL_LOG(ERROR, "bad base64 encoding");
			return (t_transform_result){.status = TRANSFORM_ERROR};
		}
		__decode_block(encblock + i, mesblock + j);
		i += B64_ENC_BLOCK_SIZE;
		j += B64_MES_BLOCK_SIZE;
	}
	if (!__is_valid_final_block(encblock + i, insize-i)) {
		SSL_LOG(ERROR, "bad base64 encoding");
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	size_t final_mesblock_size = B64_MES_BLOCK_SIZE;

	if (encblock[i+2] == __B64_TAIL_CHAR && encblock[i+3] == __B64_TAIL_CHAR) {
		final_mesblock_size = 1;
	} else if (encblock[i+3] == __B64_TAIL_CHAR) {
		final_mesblock_size = 2;
	}
	SSL_LOG(TRACE, "final block size %zu", final_mesblock_size);

	if (j + final_mesblock_size < outsize) {
		__decode_final_block(encblock + i, mesblock + j, final_mesblock_size);
		i += B64_ENC_BLOCK_SIZE;
		j += final_mesblock_size;
		ctx->done = 1;

		SSL_LOG(TRACE, "decoding is complete");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_DONE};
	}
	else {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}
}

int	base64_decode_all(const uint8_t *enc, size_t encsize, uint8_t **mes, size_t *messize)
{
	uint8_t	*omes;
	uint8_t	*oenc;

	SSL_LOG(TRACE, "decoding start: encoding size: %zu", encsize);

	if ((NULL == enc) || (NULL == mes) || (NULL == messize)) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	*mes = NULL;

	if (encsize % B64_ENC_BLOCK_SIZE != 0) {
		SSL_LOG(ERROR, "bad base64 encoding");
		return (SSL_ERR);
	}
	oenc = (uint8_t *)enc;
	*messize = CEIL_DIV(encsize, B64_ENC_BLOCK_SIZE) * B64_MES_BLOCK_SIZE;
	SSL_ALLOC(*mes, *messize);
	omes = *mes;

	if (encsize >= B64_ENC_BLOCK_SIZE) {
		while (encsize > B64_ENC_BLOCK_SIZE) {
			if (!__is_valid_block(oenc)) {
				SSL_LOG(ERROR, "bad base64 encoding");
				*messize = 0;
				return (SSL_ERR);
			}
			__decode_block(oenc, omes);
			oenc += B64_ENC_BLOCK_SIZE;
			omes += B64_MES_BLOCK_SIZE;
			encsize -= B64_ENC_BLOCK_SIZE;
		}
		if (!__is_valid_final_block(oenc, encsize)) {
			SSL_LOG(ERROR, "bad base64 encoding");
			*messize = 0;
			return (SSL_ERR);
		}
		if (oenc[2] == __B64_TAIL_CHAR && oenc[3] == __B64_TAIL_CHAR) {
			*messize -= 2;
		} else if (oenc[3] == __B64_TAIL_CHAR) {
			*messize -= 1;
		}
		size_t final_mesblock_size = (*messize%B64_MES_BLOCK_SIZE==0) ? (B64_MES_BLOCK_SIZE) : (*messize%B64_MES_BLOCK_SIZE);
		__decode_final_block(oenc, omes, final_mesblock_size);
	}
	SSL_LOG(TRACE, "decoding is complete: message size: %zu", *messize);

	return (SSL_OK);
}
