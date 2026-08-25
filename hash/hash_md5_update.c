#include <common.h>
#include <logger.h>
#include <hash.h>

static const t_md5_word	SCHED[] = {
	0x07, 0x0c, 0x11, 0x16, 0x07, 0x0c, 0x11, 0x16,
	0x07, 0x0c, 0x11, 0x16, 0x07, 0x0c, 0x11, 0x16,
	0x05, 0x09, 0x0e, 0x14, 0x05, 0x09, 0x0e, 0x14,
	0x05, 0x09, 0x0e, 0x14, 0x05, 0x09, 0x0e, 0x14,
	0x04, 0x0b, 0x10, 0x17, 0x04, 0x0b, 0x10, 0x17,
	0x04, 0x0b, 0x10, 0x17, 0x04, 0x0b, 0x10, 0x17,
	0x06, 0x0a, 0x0f, 0x15, 0x06, 0x0a, 0x0f, 0x15,
	0x06, 0x0a, 0x0f, 0x15, 0x06, 0x0a, 0x0f, 0x15
};

static const t_md5_word	K[] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static void	__rotate(t_md5_word *var, t_md5_word *t1, t_md5_word *t2, t_md5_word *i, const t_md5_word *word);
static void	__rotate_hash(t_md5_word *var, const t_md5_word *word);
static void	__update_hash(t_md5_word *var, t_md5_word *hash);

void	md5_update_block(t_hash *ctx, const uint8_t mesblock[MD5_BLOCK_SIZE])
{
	*(uint64_t *)ctx->messize += MD5_BLOCK_SIZE;
	__rotate_hash((t_md5_word *)ctx->var, (const t_md5_word *)mesblock);
	__update_hash((t_md5_word *)ctx->var, (t_md5_word *)ctx->hash);
}

void	md5_final_block(t_hash *ctx, const uint8_t *mesblock, size_t messize)
{
	*(uint64_t *)ctx->messize += messize;
	uint64_t messize_bit_count = *(uint64_t *)ctx->messize * 8;

	if (messize < 56) {
		uint8_t block[MD5_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + 56, (uint8_t *)&messize_bit_count, 8);

		__rotate_hash((t_md5_word *)ctx->var, (const t_md5_word *)block);
		__update_hash((t_md5_word *)ctx->var, (t_md5_word *)ctx->hash);
	}
	else {
		uint8_t block[2*MD5_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + MD5_BLOCK_SIZE+56, (uint8_t *)&messize_bit_count, 8);

		__rotate_hash((t_md5_word *)ctx->var, (const t_md5_word *)block);
		__update_hash((t_md5_word *)ctx->var, (t_md5_word *)ctx->hash);

		__rotate_hash((t_md5_word *)ctx->var, (const t_md5_word *)(block + MD5_BLOCK_SIZE));
		__update_hash((t_md5_word *)ctx->var, (t_md5_word *)ctx->hash);
	}
}

void	hash_md5_update(t_hash *ctx, const unsigned char *mes, size_t messize)
{
	t_md5_word	*word;
	size_t		offset;
	size_t		to_fill;

	if (NULL == ctx || (NULL == mes && messize > 0)) {
		return ;
	}
	*(uint64_t *)ctx->messize += messize;
	offset = 0;
	to_fill = MD5_BLOCK_SIZE - ctx->bufsize;

	if (ctx->bufsize > 0) {
		if (messize >= to_fill) {
			ft_memcpy(ctx->buf + ctx->bufsize, mes, to_fill);
			word = (t_md5_word *)ctx->buf;
			__rotate_hash((t_md5_word *)ctx->var, word);
			__update_hash((t_md5_word *)ctx->var, (t_md5_word *)ctx->hash);
			offset += to_fill;
			ctx->bufsize = 0;
		}
		else {
			ft_memcpy(ctx->buf + ctx->bufsize, mes, messize);
			ctx->bufsize += messize;
			return ;
		}
	}
	while (messize - offset >= MD5_BLOCK_SIZE) {
		word = (t_md5_word *)(mes + offset);
		__rotate_hash((t_md5_word *)ctx->var, word);
		__update_hash((t_md5_word *)ctx->var, (t_md5_word *)ctx->hash);
		offset += MD5_BLOCK_SIZE;
	}
	ft_memcpy(ctx->buf, mes + offset, messize - offset);
	ctx->bufsize = messize - offset;
}

static void	__rotate(t_md5_word *var, t_md5_word *t1, t_md5_word *t2, t_md5_word *i, const t_md5_word *word)
{
	*t1 = *t1 + var[0] + K[*i] + word[*t2];
	var[0] = var[3];
	var[3] = var[2];
	var[2] = var[1];
	var[1] = var[1] + LROT(*t1, SCHED[*i]);
}

static void	__rotate_hash(t_md5_word *var, const t_md5_word *word)
{
	t_md5_word	t1;
	t_md5_word	t2;
	t_md5_word	ix;

	for (ix = 0; ix < 16; ix++) {
		t1 = F0(var[1], var[2], var[3]);
		t2 = ix;
		__rotate(var, &t1, &t2, &ix, word);
	}
	for (; ix < 32; ix++) {
		t1 = G0(var[1], var[2], var[3]);
		t2 = (5 * ix + 1) % 16;
		__rotate(var, &t1, &t2, &ix, word);
	}
	for (; ix < 48; ix++) {
		t1 = H0(var[1], var[2], var[3]);
		t2 = (3 * ix + 5) % 16;
		__rotate(var, &t1, &t2, &ix, word);
	}
	for (; ix < 64; ix++) {
		t1 = I0(var[1], var[2], var[3]);
		t2 = (7 * ix) % 16;
		__rotate(var, &t1, &t2, &ix, word);
	}
}

static void	__update_hash(t_md5_word *var, t_md5_word *hash)
{
	hash[0] = var[0] + hash[0];
	hash[1] = var[1] + hash[1];
	hash[2] = var[2] + hash[2];
	hash[3] = var[3] + hash[3];
	var[0] = hash[0];
	var[1] = hash[1];
	var[2] = hash[2];
	var[3] = hash[3];
}
