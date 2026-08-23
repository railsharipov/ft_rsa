#include <common.h>
#include <logger.h>
#include <base64.h>
#include <libft.h>

// base64 tail byte character is '='
#define __B64_TAIL_CHAR 61

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

static const unsigned char	BM[128] = {
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

	SSL_LOG(TRACE, "update: input size %zu, output size %zu", insize, outsize);

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

	// Ignore the last input block since as it may be required for finalization.
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

	while (i+B64_ENC_BLOCK_SIZE < insize && j+B64_MES_BLOCK_SIZE < outsize) {
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

	SSL_LOG(TRACE, "final update: input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "decoding is already complete");
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
