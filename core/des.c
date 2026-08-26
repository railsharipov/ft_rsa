#include <common.h>
#include <logger.h>
#include <rand.h>
#include <base64.h>
#include <des.h>
#include <libft.h>

static const unsigned char	CHA[DES_KSCHED_LEN] = {
	1,  1,  2,  2,  2,  2,  2,  2,
	1,  2,  2,  2,  2,  2,  2,  1
};

static const unsigned char	CSA[48] = {
	14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10,
	23, 19, 12, 4,  26, 8,  16, 7,  27, 20, 13, 2,
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

static const unsigned char	PKA[56] = {
	57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18,
	10, 2,  59, 51, 43, 35, 27, 19, 11, 3,  60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15, 7,  62, 54, 46, 38, 30, 22,
	14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4
};

static const uint64_t	MASK28 = ((uint64_t)1 << 28) - 1;
static const uint64_t	MASK56 = ((uint64_t)1 << 56) - 1;

static void	__init_encrypt_schedule(uint64_t *kshed, const uint8_t *key);
static void	__init_decrypt_schedule(uint64_t *kshed, const uint8_t *key);

int    des_ecb_encrypt_init(t_des_ctx *des, const uint8_t key[8])
{
    if (NULL == des || NULL == key) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    ft_bzero(des, sizeof(t_des_ctx));
    __init_encrypt_schedule(des->ksched, key);

    return (SSL_OK);
}

int    des_ecb_decrypt_init(t_des_ctx *des, const uint8_t key[8])
{
    if (NULL == des || NULL == key) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    ft_bzero(des, sizeof(t_des_ctx));
    __init_decrypt_schedule(des->ksched, key);

    return (SSL_OK);
}

int    des_cbc_encrypt_init(t_des_ctx *des, const uint8_t key[8], const uint8_t iv[8])
{
    if (NULL == des || NULL == key || NULL == iv) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    ft_bzero(des, sizeof(t_des_ctx));
    __init_encrypt_schedule(des->ksched, key);
    ft_memcpy(des->vect, iv, DES_BLOCK_SIZE);

    return (SSL_OK);
}

int    des_cbc_decrypt_init(t_des_ctx *des, const uint8_t key[8], const uint8_t iv[8])
{
    if (NULL == des || NULL == key || NULL == iv) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    ft_bzero(des, sizeof(t_des_ctx));
    __init_decrypt_schedule(des->ksched, key);
    ft_memcpy(des->vect, iv, DES_BLOCK_SIZE);

    return (SSL_OK);
}

static void	__init_encrypt_schedule(uint64_t *ksched, const uint8_t *key)
{
	uint64_t	lblock, rblock;
	uint64_t	temp_key, permuted_key;
	uint64_t	tn;
	int			ix, iy;

	temp_key = *(uint64_t *)key;
# if BYTE_ORDER == LITTLE_ENDIAN
	temp_key = ft_uint_bswap64(temp_key);
# endif
	permuted_key = 0;

	ix = 0;
	while (ix < 56) {
		permuted_key <<= 1;
		permuted_key |= (temp_key >> (64 - PKA[ix])) & 1;
		ix++;
	}
	lblock = permuted_key >> 28;
	rblock = permuted_key & MASK28;

    ix = 0;
    while (ix < 16) {
        lblock = ((lblock << CHA[ix]) & MASK28) | (lblock >> (28 - CHA[ix]));
        rblock = ((rblock << CHA[ix]) & MASK28) | (rblock >> (28 - CHA[ix]));

        tn = ((lblock << 28) & MASK56) | rblock;
        iy = 0;
        while (iy < 48) {
            ksched[ix] <<= 1;
            ksched[ix] |= (tn >> (56 - CSA[iy])) & 1;
            iy++;
        }
        ix++;
    }
}

static void	__init_decrypt_schedule(uint64_t *ksched, const uint8_t *key)
{
	uint64_t	lblock, rblock;
	uint64_t	temp_key, permuted_key;
	uint64_t	tn;
	int			ix, iy;

	temp_key = *(uint64_t *)key;
# if BYTE_ORDER == LITTLE_ENDIAN
	temp_key = ft_uint_bswap64(temp_key);
# endif
	permuted_key = 0;

	ix = 0;
	while (ix < 56) {
		permuted_key <<= 1;
		permuted_key |= (temp_key >> (64 - PKA[ix])) & 1;
		ix++;
	}
	lblock = permuted_key >> 28;
	rblock = permuted_key & MASK28;

    ix = 0;
    while (ix < 16) {
        lblock = ((lblock << CHA[ix]) & MASK28) | (lblock >> (28 - CHA[ix]));
        rblock = ((rblock << CHA[ix]) & MASK28) | (rblock >> (28 - CHA[ix]));

        tn = ((lblock << 28) & MASK56) | rblock;
        iy = 0;
        while (iy < 48) {
            ksched[15-ix] <<= 1;
            ksched[15-ix] |= (tn >> (56 - CSA[iy])) & 1;
            iy++;
        }
        ix++;
    }
}

typedef void (__t_func_permute_block)(t_des_ctx *ctx, uint64_t *block);

static ssize_t __encrypt_update(t_des_ctx *ctx, const char *in, char *out, size_t size, __t_func_permute_block f);
static ssize_t __decrypt_update(t_des_ctx *ctx, const char *in, char *out, size_t size, __t_func_permute_block f);

static ssize_t __encrypt_final(t_des_ctx *ctx, char *out, size_t size, __t_func_permute_block f);
static ssize_t __decrypt_final(t_des_ctx *ctx, char *out, size_t size, __t_func_permute_block f);

static void	__permute_block(uint64_t *ksched, uint64_t *block);

ssize_t des_ecb_encrypt_update(t_des_ctx *ctx, const char *in, char *out, size_t size)
{
	SSL_LOG(TRACE, "running ctx ecb encrypt update");
	return (__encrypt_update(ctx, in, out, size, des_ecb_encrypt_permute_block));
}

ssize_t des_ecb_decrypt_update(t_des_ctx *ctx, const char *in, char *out, size_t size)
{
	SSL_LOG(TRACE, "running ctx ecb decrypt update");
	return (__decrypt_update(ctx, in, out, size, des_ecb_decrypt_permute_block));
}

ssize_t des_cbc_encrypt_update(t_des_ctx *ctx, const char *in, char *out, size_t size)
{
	SSL_LOG(TRACE, "running ctx cbc encrypt update");
	return (__encrypt_update(ctx, in, out, size, des_cbc_encrypt_permute_block));
}

ssize_t des_cbc_decrypt_update(t_des_ctx *ctx, const char *in, char *out, size_t size)
{
	SSL_LOG(TRACE, "running ctx cbc decrypt update");
	return (__decrypt_update(ctx, in, out, size, des_cbc_decrypt_permute_block));
}

ssize_t des_ecb_encrypt_final(t_des_ctx *ctx, char *out, size_t size)
{
	if (NULL == ctx || NULL == out) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	return (__encrypt_final(ctx, out, size, des_ecb_encrypt_permute_block));
}

ssize_t des_ecb_decrypt_final(t_des_ctx *ctx, char *out, size_t size)
{
	if (NULL == ctx || NULL == out) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	return (__decrypt_final(ctx, out, size, des_ecb_decrypt_permute_block));
}

ssize_t des_cbc_encrypt_final(t_des_ctx *ctx, char *out, size_t size)
{
	if (NULL == ctx || NULL == out) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	return (__encrypt_final(ctx, out, size, des_cbc_encrypt_permute_block));
}

ssize_t des_cbc_decrypt_final(t_des_ctx *ctx, char *out, size_t size)
{
	if (NULL == ctx || NULL == out) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	return (__decrypt_final(ctx, out, size, des_cbc_decrypt_permute_block));
}

void des_ecb_encrypt_permute_block(t_des_ctx *ctx, uint64_t *block)
{
	__permute_block(ctx->ksched, block);
}

void des_ecb_decrypt_permute_block(t_des_ctx *ctx, uint64_t *block)
{
	__permute_block(ctx->ksched, block);
}

void des_cbc_encrypt_permute_block(t_des_ctx *ctx, uint64_t *block)
{
	*block ^= *(uint64_t *)ctx->vect;
	__permute_block(ctx->ksched, block);
	*(uint64_t *)ctx->vect = *block;
}

void des_cbc_decrypt_permute_block(t_des_ctx *ctx, uint64_t *block)
{
	uint64_t vect = *block;
	__permute_block(ctx->ksched, block);
	*block ^= *(uint64_t *)ctx->vect;
	*(uint64_t *)ctx->vect = vect;
}

static ssize_t __encrypt_update(t_des_ctx *ctx, const char *in, char *out, size_t size, __t_func_permute_block f)
{
	size_t	in_pos, out_pos;

	SSL_LOG(TRACE, "ctx encrypting");

	if (NULL == ctx || NULL == in || NULL == out) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (ctx->bufsize > DES_BLOCK_SIZE) {
		SSL_LOG(ERROR, UNEXPECTED_ERROR);
		return (-1);
	}
	in_pos = 0;
	out_pos = 0;

	// Process message blocks. Buffer may be partially filled.
	while (in_pos < size) {
		// Fill buffer up to block size
		size_t to_copy = DES_BLOCK_SIZE - ctx->bufsize;
		if (to_copy > size - in_pos) {
			to_copy = size - in_pos;
		}
		ft_memcpy(ctx->buf + ctx->bufsize, in + in_pos, to_copy);
		ctx->bufsize += to_copy;
		ctx->messize += to_copy;
		in_pos += to_copy;
		// If buffer is full, process it
		if (ctx->bufsize == DES_BLOCK_SIZE) {
			f(ctx, (uint64_t *)ctx->buf);
			ft_memcpy(out + out_pos, ctx->buf, DES_BLOCK_SIZE);
			out_pos += DES_BLOCK_SIZE;
			ctx->bufsize = 0;
		}
	}

	SSL_LOG(TRACE, "update finish: wrote %zu bytes", out_pos);
	return (out_pos);
}

static ssize_t __decrypt_update(t_des_ctx *ctx, const char *in, char *out, size_t size, __t_func_permute_block f)
{
	size_t	in_pos, out_pos;

	SSL_LOG(TRACE, "ctx decrypting");

	if (NULL == ctx || NULL == in || NULL == out) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (ctx->bufsize > DES_BLOCK_SIZE) {
		SSL_LOG(ERROR, UNEXPECTED_ERROR);
		return (-1);
	}
	in_pos = 0;
	out_pos = 0;

	// Process blocks with 1 block delay (to handle padding in final)
	while (in_pos < size) {
		// Fill buffer up to block size
		size_t to_copy = DES_BLOCK_SIZE - ctx->bufsize;
		if (to_copy > size - in_pos) {
			to_copy = size - in_pos;
		}
		ft_memcpy(ctx->buf + ctx->bufsize, in + in_pos, to_copy);
		ctx->bufsize += to_copy;
		in_pos += to_copy;

		// If buffer is full and there's more data, process the current block
		if (ctx->bufsize == DES_BLOCK_SIZE && in_pos < size) {
			// Process it
			f(ctx, (uint64_t *)ctx->buf);
			// Write to output
			ft_memcpy(out + out_pos, ctx->buf, DES_BLOCK_SIZE);
			out_pos += DES_BLOCK_SIZE;
			ctx->messize += DES_BLOCK_SIZE;
			// Clear buffer for next block
			ctx->bufsize = 0;
		}
	}
	SSL_LOG(TRACE, "update finish: wrote %zu bytes", out_pos);
	return (out_pos);
}

static ssize_t __encrypt_final(t_des_ctx *ctx, char *out, size_t size, __t_func_permute_block f)
{
	SSL_LOG(TRACE, "encrypt final start");
	if (NULL == ctx || NULL == out) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (ctx->bufsize > DES_BLOCK_SIZE) {
		SSL_LOG(ERROR, UNEXPECTED_ERROR);
		return (-1);
	}
	// Check if output buffer has space for one block
	if (size < DES_BLOCK_SIZE) {
		SSL_LOG(ERROR, "output buffer too small");
		return (-1);
	}
	// Pad up to DES block size. Every octet of padding must be equal to padding size.
	SSL_LOG(TRACE, "pad len: %d", DES_BLOCK_SIZE - ctx->bufsize);
	ft_memset(ctx->buf + ctx->bufsize, (int)(DES_BLOCK_SIZE - ctx->bufsize), DES_BLOCK_SIZE - ctx->bufsize);

	// Encrypt padded block.
	SSL_LOG(DEBUG, "processing final block");
	f(ctx, (uint64_t *)ctx->buf);

	ft_memcpy(out, ctx->buf, DES_BLOCK_SIZE);

	SSL_LOG(TRACE, "encrypt final finish: wrote %d bytes", DES_BLOCK_SIZE);
	return (DES_BLOCK_SIZE);
}

static ssize_t __decrypt_final(t_des_ctx *ctx, char *out, size_t size, __t_func_permute_block f)
{
	size_t	to_write, padsize;

	SSL_LOG(TRACE, "decrypt final start");
	if (NULL == ctx || NULL == out) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	// At this point we expect buffer to have complete final block.
	if (ctx->bufsize != DES_BLOCK_SIZE) {
		SSL_LOG(ERROR, "bad cipher size");
		return (-1);
	}
	// Process final block.
	SSL_LOG(DEBUG, "processing final block");
	f(ctx, (uint64_t *)ctx->buf);

	padsize = ctx->buf[DES_BLOCK_SIZE - 1];
	SSL_LOG(TRACE, "pad len: %d", padsize);
	if (padsize == 0 || padsize > DES_BLOCK_SIZE) {
		SSL_LOG(ERROR, "bad cipher pad size");
		return (-1);
	}
	// Write last block without pad octets.
	to_write = DES_BLOCK_SIZE - padsize;

	// Check if output buffer has space
	if (size < to_write) {
		SSL_LOG(ERROR, "output buffer too small");
		return (-1);
	}

	ft_memcpy(out, ctx->buf, to_write);

	ctx->bufsize = 0;
	ctx->messize += to_write;

	SSL_LOG(TRACE, "decrypt final finish: wrote %zu bytes", to_write);
	return (to_write);
}

static const unsigned char	PMA[64] = {
	58,	50,	42,	34,	26,	18,	10,	2,	60,	52,	44,	36,	28,	20,	12,	4,
	62,	54,	46,	38,	30,	22,	14,	6,	64,	56,	48,	40,	32,	24,	16,	8,
	57,	49,	41,	33,	25,	17,	9,	1,	59,	51,	43,	35,	27,	19,	11,	3,
	61,	53,	45,	37,	29,	21,	13,	5,	63,	55,	47,	39,	31,	23,	15,	7
};

static const unsigned char	PBA[48] = {
	32,	1,	2,	3,	4,	5,	4,	5,	6,	7,	8,	9,	8,	9,	10,	11,
	12,	13,	12,	13,	14,	15,	16,	17,	16,	17,	18,	19,	20,	21,	20,	21,
	22,	23,	24,	25,	24,	25,	26,	27,	28,	29,	28,	29,	30,	31,	32,	1
};

static const unsigned char	PBB[32] = {
	16,	7,	20,	21,	29,	12,	28,	17,	1,	15,	23,	26,	5,	18,	31,	10,
	2,	8,	24,	14,	32,	27,	3,	9,	19,	13,	30,	6,	22,	11,	4,	25
};

static const unsigned char	PCA[64] = {
	40,	8,	48,	16,	56,	24,	64,	32,	39,	7,	47,	15,	55,	23,	63,	31,
	38,	6,	46,	14,	54,	22,	62,	30,	37,	5,	45,	13,	53,	21,	61,	29,
	36,	4,	44,	12,	52,	20,	60,	28,	35,	3,	43,	11,	51,	19,	59,	27,
	34,	2,	42,	10,	50,	18,	58,	26,	33,	1,	41,	9,	49,	17,	57,	25
};

static const unsigned char	SB[8*4*16] = {
	14,	0,	4,	15,	13,	7,	1,	4,	2,	14,	15,	2,	11,	13,	8,	1,
	3,	10,	10,	6,	6,	12,	12,	11,	5,	9,	9,	5,	0,	3,	7,	8,
	4,	15,	1,	12,	14,	8,	8,	2,	13,	4,	6,	9,	2,	1,	11,	7,
	15,	5,	12,	11,	9,	3,	7,	14,	3,	10,	10,	0,	5,	6,	0,	13,

	15,	3,	1,	13,	8,	4,	14,	7,	6,	15,	11,	2,	3,	8,	4,	14,
	9,	12,	7,	0,	2,	1,	13,	10,	12,	6,	0,	9,	5,	11,	10,	5,
	0,	13,	14,	8,	7,	10,	11,	1,	10,	3,	4,	15,	13,	4,	1,	2,
	5,	11,	8,	6,	12,	7,	6,	12,	9,	0,	3,	5,	2,	14,	15,	9,

	10,	13,	0,	7,	9,	0,	14,	9,	6,	3,	3,	4,	15,	6,	5,	10,
	1,	2,	13,	8,	12,	5,	7,	14,	11,	12,	4,	11,	2,	15,	8,	1,
	13,	1,	6,	10,	4,	13,	9,	0,	8,	6,	15,	9,	3,	8,	0,	7,
	11,	4,	1,	15,	2,	14,	12,	3,	5,	11,	10,	5,	14,	2,	7,	12,

	7,	13,	13,	8,	14,	11,	3,	5,	0,	6,	6,	15,	9,	0,	10,	3,
	1,	4,	2,	7,	8,	2,	5,	12,	11,	1,	12,	10,	4,	14,	15,	9,
	10,	3,	6,	15,	9,	0,	0,	6,	12,	10,	11,	1,	7,	13,	13,	8,
	15,	9,	1,	4,	3,	5,	14,	11,	5,	12,	2,	7,	8,	2,	4,	14,

	2,	14,	12,	11,	4,	2,	1,	12,	7,	4,	10,	7,	11,	13,	6,	1,
	8,	5,	5,	0,	3,	15,	15,	10,	13,	3,	0,	9,	14,	8,	9,	6,
	4,	11,	2,	8,	1,	12,	11,	7,	10,	1,	13,	14,	7,	2,	8,	13,
	15,	6,	9,	15,	12,	0,	5,	9,	6,	10,	3,	4,	0,	5,	14,	3,

	12,	10,	1,	15,	10,	4,	15,	2,	9,	7,	2,	12,	6,	9,	8,	5,
	0,	6,	13,	1,	3,	13,	4,	14,	14,	0,	7,	11,	5,	3,	11,	8,
	9,	4,	14,	3,	15,	2,	5,	12,	2,	9,	8,	5,	12,	15,	3,	10,
	7,	11,	0,	14,	4,	1,	10,	7,	1,	6,	13,	0,	11,	8,	6,	13,

	4,	13,	11,	0,	2,	11,	14,	7,	15,	4,	0,	9,	8,	1,	13,	10,
	3,	14,	12,	3,	9,	5,	7,	12,	5,	2,	10,	15,	6,	8,	1,	6,
	1,	6,	4,	11,	11,	13,	13,	8,	12,	1,	3,	4,	7,	10,	14,	7,
	10,	9,	15,	5,	6,	0,	8,	15,	0,	14,	5,	2,	9,	3,	2,	12,

	13,	1,	2,	15,	8,	13,	4,	8,	6,	10,	15,	3,	11,	7,	1,	4,
	10,	12,	9,	5,	3,	6,	14,	11,	5,	0,	0,	14,	12,	9,	7,	2,
	7,	2,	11,	1,	4,	14,	1,	7,	9,	4,	12,	10,	14,	8,	2,	13,
	0,	15,	6,	12,	10,	9,	13,	0,	15,	3,	3,	5,	5,	6,	8,	11
};

static void	__permute_block(uint64_t *ksched, uint64_t *block)
{
	uint64_t	tn, tn2;
	uint32_t	lblock, rblock;
	uint32_t	tblock, pblock;
	int 		ix, iy;

	tn = 0;
	ix = 0;
	while (ix < 64) {
		tn <<= 1;
		tn |= ((*block >> (64 - PMA[ 8*(ix/8)+(7-ix%8) ])) & 1);
		ix++;
	}
	lblock = tn >> 32;
	rblock = tn & ((1UL<<32)-1);

	for (ix = 0; ix < 16; ix++) {
		pblock = 0;
		tn = 0;

		for (iy = 0; iy < 48; iy++) {
			tn <<= 1;
			tn |= ((uint64_t)rblock >> (32 - PBA[iy])) & 1;
		}
		tn = tn ^ ksched[ix];
		tn2 = 0;

		for (iy = 0; iy < 8; iy++) {
			tn2 <<= 4;
			tn2 |= SB[ 64*iy + ((tn >> (42-6*iy)) & 0x3F) ];
		}
		for (iy = 0; iy < 32; iy++) {
			pblock <<= 1;
			pblock |= (tn2 >> (32 - PBB[iy])) & 1;
		}
		tblock = lblock;
		lblock = rblock;
		rblock = tblock ^ pblock;
	}

	*block = (uint64_t)rblock << 32;
	*block |= lblock;

	tn = 0;
	ix = 0;
	while (ix < 64) {
		tn <<= 1;
		tn |= (*block >> (64 - PCA[ix])) & 1;
		ix++;
	}
	*block = tn;

# if BYTE_ORDER == LITTLE_ENDIAN
	*block = ft_uint_bswap64(*block);
# endif
}

t_transform_result des_ecb_encrypt_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_des_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "already done");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *mes = in;
	uint8_t *enc = out;
	size_t i = 0;

	if (insize < DES_BLOCK_SIZE) {
		SSL_LOG(TRACE, "need input");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_NEED_INPUT};
	}
	if (outsize < DES_BLOCK_SIZE) {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_NEED_OUTPUT};
	}
	while (i+DES_BLOCK_SIZE <= insize && i+DES_BLOCK_SIZE <= outsize) {
		ft_memcpy((uint8_t *)enc + i, mes + i, DES_BLOCK_SIZE);
		des_ecb_encrypt_permute_block(ctx, (uint64_t *)(enc + i));
		i += DES_BLOCK_SIZE;
	}
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_OK};
}

t_transform_result des_ecb_encrypt_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_des_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "already done");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *mes = in;
	uint8_t *enc = out;
	size_t i = 0;

	while (i+DES_BLOCK_SIZE <= insize && i+DES_BLOCK_SIZE <= outsize) {
		ft_memcpy(enc + i, mes + i, DES_BLOCK_SIZE);
		des_ecb_encrypt_permute_block(ctx, (uint64_t *)(mes + i));
		i += DES_BLOCK_SIZE;
	}
	if (i + DES_BLOCK_SIZE <= outsize) {
		size_t tailsize = insize%DES_BLOCK_SIZE;
		size_t padsize = DES_BLOCK_SIZE-tailsize;
		SSL_LOG(TRACE, "using pad size: %zu", padsize);

		ft_memcpy(enc + i, mes + i, tailsize);
		ft_memset(enc + i+tailsize, (int)padsize, padsize);
		des_ecb_encrypt_permute_block(ctx, (uint64_t *)(enc + i));
		i = insize;
		ctx->done = 1;
		SSL_LOG(TRACE, "processing is complete");
		return (t_transform_result){.consumed = i, .produced = i + padsize, .status = TRANSFORM_DONE};
	} else {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_NEED_OUTPUT};
	}
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_OK};
}
