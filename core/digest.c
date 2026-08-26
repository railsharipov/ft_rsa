#include <common.h>
#include <logger.h>
#include <digest.h>

#include "__digest.h"

static const t_md5_word	MD5_SCHED[] = {
	0x07, 0x0c, 0x11, 0x16, 0x07, 0x0c, 0x11, 0x16,
	0x07, 0x0c, 0x11, 0x16, 0x07, 0x0c, 0x11, 0x16,
	0x05, 0x09, 0x0e, 0x14, 0x05, 0x09, 0x0e, 0x14,
	0x05, 0x09, 0x0e, 0x14, 0x05, 0x09, 0x0e, 0x14,
	0x04, 0x0b, 0x10, 0x17, 0x04, 0x0b, 0x10, 0x17,
	0x04, 0x0b, 0x10, 0x17, 0x04, 0x0b, 0x10, 0x17,
	0x06, 0x0a, 0x0f, 0x15, 0x06, 0x0a, 0x0f, 0x15,
	0x06, 0x0a, 0x0f, 0x15, 0x06, 0x0a, 0x0f, 0x15
};

static const t_md5_word	MD5_K[] = {
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

static const t_md5_word	MD5_INIT_VECT[] = {
	0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476
};

void	md5_init(t_digest_ctx *ctx)
{
	ft_bzero(ctx, sizeof(t_digest_ctx));
	ft_memcpy(ctx->var, MD5_INIT_VECT, sizeof(MD5_INIT_VECT));
	ft_memcpy(ctx->hash, MD5_INIT_VECT, sizeof(MD5_INIT_VECT));
	ctx->blocksize = MD5_BLOCK_SIZE;
	ctx->hashsize = MD5_HASH_SIZE;
}

static void	__md5_rotate(t_md5_word *var, t_md5_word *t1, t_md5_word *t2, t_md5_word *i, const t_md5_word *word);
static void	__md5_rotate_hash(t_md5_word *var, const t_md5_word *word);
static void	__md5_update_hash(t_md5_word *var, t_md5_word *hash);

void	md5_update_block(t_digest_ctx *ctx, const uint8_t mesblock[MD5_BLOCK_SIZE])
{
	*(uint64_t *)ctx->messize += MD5_BLOCK_SIZE;
	__md5_rotate_hash((t_md5_word *)ctx->var, (const t_md5_word *)mesblock);
	__md5_update_hash((t_md5_word *)ctx->var, (t_md5_word *)ctx->hash);
}

void	md5_final_block(t_digest_ctx *ctx, const uint8_t *mesblock, size_t messize)
{
	*(uint64_t *)ctx->messize += messize;
	uint64_t messize_bit_count = *(uint64_t *)ctx->messize * 8;

	if (messize < 56) {
		uint8_t block[MD5_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + 56, (uint8_t *)&messize_bit_count, 8);

		__md5_rotate_hash((t_md5_word *)ctx->var, (const t_md5_word *)block);
		__md5_update_hash((t_md5_word *)ctx->var, (t_md5_word *)ctx->hash);
	}
	else {
		uint8_t block[2*MD5_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + MD5_BLOCK_SIZE+56, (uint8_t *)&messize_bit_count, 8);

		__md5_rotate_hash((t_md5_word *)ctx->var, (const t_md5_word *)block);
		__md5_update_hash((t_md5_word *)ctx->var, (t_md5_word *)ctx->hash);

		__md5_rotate_hash((t_md5_word *)ctx->var, (const t_md5_word *)(block + MD5_BLOCK_SIZE));
		__md5_update_hash((t_md5_word *)ctx->var, (t_md5_word *)ctx->hash);
	}
}

void	md5_update(t_digest_ctx *ctx, const unsigned char *mes, size_t messize)
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
			__md5_rotate_hash((t_md5_word *)ctx->var, word);
			__md5_update_hash((t_md5_word *)ctx->var, (t_md5_word *)ctx->hash);
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
		__md5_rotate_hash((t_md5_word *)ctx->var, word);
		__md5_update_hash((t_md5_word *)ctx->var, (t_md5_word *)ctx->hash);
		offset += MD5_BLOCK_SIZE;
	}
	ft_memcpy(ctx->buf, mes + offset, messize - offset);
	ctx->bufsize = messize - offset;
}

void	md5_final(t_digest_ctx *ctx)
{
	uint64_t	msize_nbits;
	uint8_t		pad[MD5_BLOCK_SIZE];
	size_t		pad_len;

	if (NULL == ctx) {
		return ;
	}
	msize_nbits = *(uint64_t *)ctx->messize * 8;
	pad_len = (ctx->bufsize < 56) ? (56 - ctx->bufsize) : (MD5_BLOCK_SIZE + 56 - ctx->bufsize);
	ft_bzero(pad, pad_len);
	pad[0] = 0x80;
	md5_update(ctx, pad, pad_len);
	md5_update(ctx, (uint8_t *)&msize_nbits, sizeof(msize_nbits));
}

static void	__md5_rotate(t_md5_word *var, t_md5_word *t1, t_md5_word *t2, t_md5_word *i, const t_md5_word *word)
{
	*t1 = *t1 + var[0] + MD5_K[*i] + word[*t2];
	var[0] = var[3];
	var[3] = var[2];
	var[2] = var[1];
	var[1] = var[1] + LROT(*t1, MD5_SCHED[*i]);
}

static void	__md5_rotate_hash(t_md5_word *var, const t_md5_word *word)
{
	t_md5_word	t1;
	t_md5_word	t2;
	t_md5_word	ix;

	for (ix = 0; ix < 16; ix++) {
		t1 = F0(var[1], var[2], var[3]);
		t2 = ix;
		__md5_rotate(var, &t1, &t2, &ix, word);
	}
	for (; ix < 32; ix++) {
		t1 = G0(var[1], var[2], var[3]);
		t2 = (5 * ix + 1) % 16;
		__md5_rotate(var, &t1, &t2, &ix, word);
	}
	for (; ix < 48; ix++) {
		t1 = H0(var[1], var[2], var[3]);
		t2 = (3 * ix + 5) % 16;
		__md5_rotate(var, &t1, &t2, &ix, word);
	}
	for (; ix < 64; ix++) {
		t1 = I0(var[1], var[2], var[3]);
		t2 = (7 * ix) % 16;
		__md5_rotate(var, &t1, &t2, &ix, word);
	}
}

static void	__md5_update_hash(t_md5_word *var, t_md5_word *hash)
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

static t_sha1_word __sha1_sched[80];

static const t_sha1_word SHA1_K[] = {
	[0 ... 19] = 0x5a827999,
	[20 ... 39] = 0x6ed9eba1,
	[40 ... 59] = 0x8f1bbcdc,
	[60 ... 79] = 0xca62c1d6
};

static const t_sha1_word	SHA1_INIT_VECT[] = {
	0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0
};

void	sha1_init(t_digest_ctx *ctx)
{
	ft_bzero(ctx, sizeof(t_digest_ctx));
	ft_memcpy(ctx->var, SHA1_INIT_VECT, sizeof(SHA1_INIT_VECT));
	ft_memcpy(ctx->hash, SHA1_INIT_VECT, sizeof(SHA1_INIT_VECT));
	ctx->blocksize = SHA1_BLOCK_SIZE;
	ctx->hashsize = SHA1_HASH_SIZE;
}

static void	__sha1_update_sched(const t_sha1_word *word);
static void	__sha1_rotate(t_sha1_word *var, t_sha1_word *t1, t_sha1_word *t2, t_sha1_word ix);
static void	__sha1_rotate_hash(t_sha1_word *var, const t_sha1_word *word);
static void	__sha1_update_hash(t_sha1_word *var, t_sha1_word *hash);

static void __swap_bytes_32(uint32_t *arr, size_t size);

void	sha1_update_block(t_digest_ctx *ctx, const uint8_t mesblock[SHA1_BLOCK_SIZE])
{
	*(uint64_t *)ctx->messize += SHA1_BLOCK_SIZE;
	__sha1_rotate_hash((t_sha1_word *)ctx->var, (const t_sha1_word *)mesblock);
	__sha1_update_hash((t_sha1_word *)ctx->var, (t_sha1_word *)ctx->hash);
}

void	sha1_final_block(t_digest_ctx *ctx, const uint8_t *mesblock, size_t messize)
{
	*(uint64_t *)ctx->messize += messize;
	uint64_t messize_bit_count = ft_uint_bswap64(*(uint64_t *)ctx->messize * 8);

	if (messize < 56) {
		uint8_t block[SHA1_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + 56, (uint8_t *)&messize_bit_count, 8);

		__sha1_rotate_hash((t_sha1_word *)ctx->var, (const t_sha1_word *)block);
		__sha1_update_hash((t_sha1_word *)ctx->var, (t_sha1_word *)ctx->hash);
	}
	else {
		uint8_t block[2*SHA1_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + SHA1_BLOCK_SIZE+56, (uint8_t *)&messize_bit_count, 8);

		__sha1_rotate_hash((t_sha1_word *)ctx->var, (const t_sha1_word *)block);
		__sha1_update_hash((t_sha1_word *)ctx->var, (t_sha1_word *)ctx->hash);

		__sha1_rotate_hash((t_sha1_word *)ctx->var, (const t_sha1_word *)(block + SHA1_BLOCK_SIZE));
		__sha1_update_hash((t_sha1_word *)ctx->var, (t_sha1_word *)ctx->hash);
	}
# if BYTE_ORDER == LITTLE_ENDIAN
	__swap_bytes_32((uint32_t *)ctx->hash, SHA1_HASH_LEN);
# endif
}

void	sha1_update(t_digest_ctx *ctx, const unsigned char *mes, size_t messize)
{
	t_sha1_word	*word;
	size_t		offset;
	size_t		to_fill;

	if (NULL == ctx || (NULL == mes && messize > 0)) {
		return ;
	}
	*(uint64_t *)ctx->messize += messize;
	offset = 0;
	to_fill = SHA1_BLOCK_SIZE - ctx->bufsize;

	if (ctx->bufsize > 0) {
		if (messize >= to_fill) {
			ft_memcpy(ctx->buf + ctx->bufsize, mes, to_fill);
			word = (t_sha1_word *)ctx->buf;
			__sha1_rotate_hash((t_sha1_word *)ctx->var, word);
			__sha1_update_hash((t_sha1_word *)ctx->var, (t_sha1_word *)ctx->hash);
			offset += to_fill;
			ctx->bufsize = 0;
		}
		else {
			ft_memcpy(ctx->buf + ctx->bufsize, mes, messize);
			ctx->bufsize += messize;
			return ;
		}
	}
	while (messize - offset >= SHA1_BLOCK_SIZE) {
		word = (t_sha1_word *)(mes + offset);
		__sha1_rotate_hash((t_sha1_word *)ctx->var, word);
		__sha1_update_hash((t_sha1_word *)ctx->var, (t_sha1_word *)ctx->hash);
		offset += SHA1_BLOCK_SIZE;
	}
	ft_memcpy(ctx->buf, mes + offset, messize - offset);
	ctx->bufsize = messize - offset;
}

void	sha1_final(t_digest_ctx *ctx)
{
	uint64_t	msize_nbits;
	uint8_t		pad[SHA1_BLOCK_SIZE];
	size_t		pad_len;

	if (NULL == ctx) {
		return ;
	}
	msize_nbits = ft_uint_bswap64(*(uint64_t *)ctx->messize * 8);
	pad_len = (ctx->bufsize < 56) ? (56 - ctx->bufsize) : (SHA1_BLOCK_SIZE + 56 - ctx->bufsize);
	ft_bzero(pad, pad_len);
	pad[0] = 0x80;
	sha1_update(ctx, pad, pad_len);
	sha1_update(ctx, (uint8_t *)&msize_nbits, sizeof(msize_nbits));

# if BYTE_ORDER == LITTLE_ENDIAN
	__swap_bytes_32((uint32_t *)ctx->hash, SHA1_HASH_LEN);
# endif
}

static void	__sha1_update_sched(const t_sha1_word *word)
{
	int	i;

	i = 0;
	while (i < 16) {
# if BYTE_ORDER == BIG_ENDIAN
		__sha1_sched[i] = word[i];
# else
		__sha1_sched[i] = ft_uint_bswap32(word[i]);
# endif
		i++;
	}
	while (i < 80) {
		__sha1_sched[i] = LROT((__sha1_sched[i-3]^__sha1_sched[i-8]^__sha1_sched[i-14]^__sha1_sched[i-16]), 1);
		i++;
	}
}

static void	__sha1_rotate(t_sha1_word *var, t_sha1_word *t1, t_sha1_word *t2, t_sha1_word ix)
{
	*t2 = LROT(var[0], 5) + *t1 + var[4] + SHA1_K[ix] + __sha1_sched[ix];

	var[4] = var[3];
	var[3] = var[2];
	var[2] = LROT(var[1], 30);
	var[1] = var[0];
	var[0] = *t2;
}

static void	__sha1_rotate_hash(t_sha1_word *var, const t_sha1_word *word)
{
	t_sha1_word	t1;
	t_sha1_word	t2;
	t_sha1_word	ix;

	__sha1_update_sched(word);
	ix = 0;
	while (ix < 20) {
		t1 = CH(var[1], var[2], var[3]);
		__sha1_rotate(var, &t1, &t2, ix++);
	}
	while (ix < 40) {
		t1 = H0(var[1], var[2], var[3]);
		__sha1_rotate(var, &t1, &t2, ix++);
	}
	while (ix < 60) {
		t1 = MAJ(var[1], var[2], var[3]);
		__sha1_rotate(var, &t1, &t2, ix++);
	}
	while (ix < 80) {
		t1 = H0(var[1], var[2], var[3]);
		__sha1_rotate(var, &t1, &t2, ix++);
	}
}

static void	__sha1_update_hash(t_sha1_word *var, t_sha1_word *hash)
{
	hash[0] = var[0] + hash[0];
	hash[1] = var[1] + hash[1];
	hash[2] = var[2] + hash[2];
	hash[3] = var[3] + hash[3];
	hash[4] = var[4] + hash[4];
	var[0] = hash[0];
	var[1] = hash[1];
	var[2] = hash[2];
	var[3] = hash[3];
	var[4] = hash[4];
}


static const t_sha256_word	SHA224_INIT_VECT[] = {
	0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
	0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
};

void	sha224_init(t_digest_ctx *ctx)
{
	ft_bzero(ctx, sizeof(t_digest_ctx));
	ft_memcpy(ctx->var, SHA224_INIT_VECT, sizeof(SHA224_INIT_VECT));
	ft_memcpy(ctx->hash, SHA224_INIT_VECT, sizeof(SHA224_INIT_VECT));
	ctx->blocksize = SHA256_BLOCK_SIZE;
	ctx->hashsize = SHA224_HASH_SIZE;
}

void	sha224_update(t_digest_ctx *ctx, const unsigned char *mes, size_t messize)
{
	sha256_update(ctx, mes, messize);
}

void	sha224_final(t_digest_ctx *ctx)
{
	sha256_final(ctx);
}


static t_sha256_word	__sha256_sched[64];

static const t_sha256_word	SHA256_K[] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static const t_sha256_word	SHA256_INIT_VECT[] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

void	sha256_init(t_digest_ctx *ctx)
{
	ft_bzero(ctx, sizeof(t_digest_ctx));
	ft_memcpy(ctx->var, SHA256_INIT_VECT, sizeof(SHA256_INIT_VECT));
	ft_memcpy(ctx->hash, SHA256_INIT_VECT, sizeof(SHA256_INIT_VECT));
	ctx->blocksize = SHA256_BLOCK_SIZE;
	ctx->hashsize = SHA256_HASH_SIZE;
}

static void	__sha256_update_sched(const t_sha256_word *word);
static void	__sha256_rotate(t_sha256_word *var, t_sha256_word ix);
static void	__sha256_rotate_hash(t_sha256_word *var, const t_sha256_word *word);
static void	__sha256_update_hash(t_sha256_word *var, t_sha256_word *hash);

void	sha256_update_block(t_digest_ctx *ctx, const uint8_t mesblock[SHA256_BLOCK_SIZE])
{
	*(uint64_t *)ctx->messize += SHA256_BLOCK_SIZE;
	__sha256_rotate_hash((t_sha256_word *)ctx->var, (const t_sha256_word *)mesblock);
	__sha256_update_hash((t_sha256_word *)ctx->var, (t_sha256_word *)ctx->hash);
}

void	sha256_final_block(t_digest_ctx *ctx, const uint8_t *mesblock, size_t messize)
{
	*(uint64_t *)ctx->messize += messize;
	uint64_t messize_bit_count = ft_uint_bswap64(*(uint64_t *)ctx->messize * 8);

	if (messize < 56) {
		uint8_t block[SHA256_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + 56, (uint8_t *)&messize_bit_count, 8);

		__sha256_rotate_hash((t_sha256_word *)ctx->var, (const t_sha256_word *)block);
		__sha256_update_hash((t_sha256_word *)ctx->var, (t_sha256_word *)ctx->hash);
	}
	else {
		uint8_t block[2*SHA256_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + SHA256_BLOCK_SIZE+56, (uint8_t *)&messize_bit_count, 8);

		__sha256_rotate_hash((t_sha256_word *)ctx->var, (const t_sha256_word *)block);
		__sha256_update_hash((t_sha256_word *)ctx->var, (t_sha256_word *)ctx->hash);

		__sha256_rotate_hash((t_sha256_word *)ctx->var, (const t_sha256_word *)(block + SHA256_BLOCK_SIZE));
		__sha256_update_hash((t_sha256_word *)ctx->var, (t_sha256_word *)ctx->hash);
	}
# if BYTE_ORDER == LITTLE_ENDIAN
	__swap_bytes_32((uint32_t *)ctx->hash, SHA256_HASH_LEN);
# endif
}

void	sha256_update(t_digest_ctx *ctx, const unsigned char *mes, size_t messize)
{
	t_sha256_word	*word;
	size_t			offset;
	size_t			to_fill;

	if (NULL == ctx || (NULL == mes && messize > 0)) {
		return ;
	}
	*(uint64_t *)ctx->messize += messize;
	offset = 0;
	to_fill = SHA256_BLOCK_SIZE - ctx->bufsize;

	if (ctx->bufsize > 0) {
		if (messize >= to_fill) {
			ft_memcpy(ctx->buf + ctx->bufsize, mes, to_fill);
			word = (t_sha256_word *)ctx->buf;
			__sha256_rotate_hash((t_sha256_word *)ctx->var, word);
			__sha256_update_hash((t_sha256_word *)ctx->var, (t_sha256_word *)ctx->hash);
			offset += to_fill;
			ctx->bufsize = 0;
		}
		else {
			ft_memcpy(ctx->buf + ctx->bufsize, mes, messize);
			ctx->bufsize += messize;
			return ;
		}
	}
	while (messize - offset >= SHA256_BLOCK_SIZE) {
		word = (t_sha256_word *)(mes + offset);
		__sha256_rotate_hash((t_sha256_word *)ctx->var, word);
		__sha256_update_hash((t_sha256_word *)ctx->var, (t_sha256_word *)ctx->hash);
		offset += SHA256_BLOCK_SIZE;
	}
	ft_memcpy(ctx->buf, mes + offset, messize - offset);
	ctx->bufsize = messize - offset;
}

void	sha256_final(t_digest_ctx *ctx)
{
	uint64_t	msize_nbits;
	uint8_t		pad[SHA256_BLOCK_SIZE];
	size_t		pad_len;

	if (NULL == ctx) {
		return ;
	}
	msize_nbits = ft_uint_bswap64(*(uint64_t *)ctx->messize * 8);
	pad_len = (ctx->bufsize < 56) ? (56 - ctx->bufsize) : (SHA256_BLOCK_SIZE + 56 - ctx->bufsize);
	ft_bzero(pad, pad_len);
	pad[0] = 0x80;
	sha256_update(ctx, pad, pad_len);
	sha256_update(ctx, (uint8_t *)&msize_nbits, sizeof(msize_nbits));

# if BYTE_ORDER == LITTLE_ENDIAN
	__swap_bytes_32((uint32_t *)ctx->hash, SHA256_HASH_LEN);
# endif
}

static void	__sha256_update_sched(const t_sha256_word *word)
{
	int	i;

	i = 0;
	while (i < 16) {
# if BYTE_ORDER == BIG_ENDIAN
		__sha256_sched[i] = word[i];
# else
		__sha256_sched[i] = ft_uint_bswap32(word[i]);
# endif
		i++;
	}
	while (i < 64) {
		__sha256_sched[i] = S1(__sha256_sched[i-2]) + __sha256_sched[i-7] + S0(__sha256_sched[i-15]) + __sha256_sched[i-16];
		i++;
	}
}

static void	__sha256_rotate(t_sha256_word *var, t_sha256_word ix)
{
	t_sha256_word	t1;
	t_sha256_word	t2;

	t1 = var[7] + E1(var[4]) + CH(var[4], var[5], var[6]) + SHA256_K[ix] + __sha256_sched[ix];
	t2 = E0(var[0]) + MAJ(var[0], var[1], var[2]);
	var[7] = var[6];
	var[6] = var[5];
	var[5] = var[4];
	var[4] = var[3] + t1;
	var[3] = var[2];
	var[2] = var[1];
	var[1] = var[0];
	var[0] = t1 + t2;
}

static void	__sha256_rotate_hash(t_sha256_word *var, const t_sha256_word *word)
{
	t_sha256_word	ix;

	__sha256_update_sched(word);
	ix = 0;
	while (ix < 64) {
		__sha256_rotate(var, ix++);
	}
}

static void	__sha256_update_hash(t_sha256_word *var, t_sha256_word *hash)
{
	hash[0] = var[0] + hash[0];
	hash[1] = var[1] + hash[1];
	hash[2] = var[2] + hash[2];
	hash[3] = var[3] + hash[3];
	hash[4] = var[4] + hash[4];
	hash[5] = var[5] + hash[5];
	hash[6] = var[6] + hash[6];
	hash[7] = var[7] + hash[7];
	var[0] = hash[0];
	var[1] = hash[1];
	var[2] = hash[2];
	var[3] = hash[3];
	var[4] = hash[4];
	var[5] = hash[5];
	var[6] = hash[6];
	var[7] = hash[7];
}


static const t_sha512_word	SHA384_INIT_VECT[] = {
	0xcbbb9d5dc1059ed8, 0x629a292a367cd507,
	0x9159015a3070dd17, 0x152fecd8f70e5939,
	0x67332667ffc00b31, 0x8eb44a8768581511,
	0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4
};

void	sha384_init(t_digest_ctx *ctx)
{
	ft_bzero(ctx, sizeof(t_digest_ctx));
	ft_memcpy(ctx->var, SHA384_INIT_VECT, sizeof(SHA384_INIT_VECT));
	ft_memcpy(ctx->hash, SHA384_INIT_VECT, sizeof(SHA384_INIT_VECT));
	ctx->blocksize = SHA512_BLOCK_SIZE;
	ctx->hashsize = SHA384_HASH_SIZE;
}

void	sha384_update(t_digest_ctx *ctx, const unsigned char *mes, size_t messize)
{
	sha512_update(ctx, mes, messize);
}

void	sha384_final(t_digest_ctx *sha384)
{
	sha512_final(sha384);
}


static t_sha512_word	__sha512_sched[80];

static const t_sha512_word	SHA512_K[] = {
	0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f,
	0xe9b5dba58189dbbc, 0x3956c25bf348b538, 0x59f111f1b605d019,
	0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242,
	0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
	0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
	0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3,
	0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 0x2de92c6f592b0275,
	0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
	0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f,
	0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
	0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc,
	0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
	0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6,
	0x92722c851482353b, 0xa2bfe8a14cf10364, 0xa81a664bbc423001,
	0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
	0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
	0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99,
	0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb,
	0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc,
	0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
	0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915,
	0xc67178f2e372532b, 0xca273eceea26619c, 0xd186b8c721c0c207,
	0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba,
	0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
	0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
	0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a,
	0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

static const t_sha512_word	SHA512_INIT_VECT[] = {
	0x6a09e667f3bcc908, 0xbb67ae8584caa73b,
	0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
	0x510e527fade682d1, 0x9b05688c2b3e6c1f,
	0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
};

void	sha512_init(t_digest_ctx *ctx)
{
	ft_bzero(ctx, sizeof(t_digest_ctx));
	ft_memcpy(ctx->var, SHA512_INIT_VECT, sizeof(SHA512_INIT_VECT));
	ft_memcpy(ctx->hash, SHA512_INIT_VECT, sizeof(SHA512_INIT_VECT));
	ctx->blocksize = SHA512_BLOCK_SIZE;
	ctx->hashsize = SHA512_HASH_SIZE;
}

static void	__sha512_update_sched(const t_sha512_word *word);
static void	__sha512_rotate(t_sha512_word *var, t_sha512_word ix);
static void	__sha512_rotate_hash(t_sha512_word *var, const t_sha512_word *word);
static void	__sha512_update_hash(t_sha512_word *var, t_sha512_word *hash);

static void __swap_bytes_64(t_sha512_word *arr, size_t size);

void	sha512_update_block(t_digest_ctx *ctx, const uint8_t mesblock[SHA512_BLOCK_SIZE])
{
	*(uint128_t *)ctx->messize += SHA512_BLOCK_SIZE;
	__sha512_rotate_hash((t_sha512_word *)ctx->var, (const t_sha512_word *)mesblock);
	__sha512_update_hash((t_sha512_word *)ctx->var, (t_sha512_word *)ctx->hash);
}

void	sha512_final_block(t_digest_ctx *ctx, const uint8_t *mesblock, size_t messize)
{
	*(uint128_t *)ctx->messize += messize;
	uint128_t messize_bit_count = ft_uint_bswap128((*(uint128_t *)ctx->messize) * 8);

	if (messize < 112) {
		uint8_t block[SHA512_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + 112, (uint8_t *)&messize_bit_count, 16);

		__sha512_rotate_hash((t_sha512_word *)ctx->var, (const t_sha512_word *)block);
		__sha512_update_hash((t_sha512_word *)ctx->var, (t_sha512_word *)ctx->hash);
	}
	else {
		uint8_t block[2*SHA512_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + SHA512_BLOCK_SIZE+112, (uint8_t *)&messize_bit_count, 16);

		__sha512_rotate_hash((t_sha512_word *)ctx->var, (const t_sha512_word *)block);
		__sha512_update_hash((t_sha512_word *)ctx->var, (t_sha512_word *)ctx->hash);

		__sha512_rotate_hash((t_sha512_word *)ctx->var, (const t_sha512_word *)(block + SHA512_BLOCK_SIZE));
		__sha512_update_hash((t_sha512_word *)ctx->var, (t_sha512_word *)ctx->hash);
	}
# if BYTE_ORDER == LITTLE_ENDIAN
	__swap_bytes_64((t_sha512_word *)ctx->hash, SHA512_HASH_LEN);
# endif
}

void	sha512_update(t_digest_ctx *ctx, const unsigned char *mes, size_t messize)
{
	t_sha512_word	*word;
	size_t			offset;
	size_t			to_fill;

	if (NULL == ctx || (NULL == mes && messize > 0)) {
		return ;
	}
	*(uint128_t *)ctx->messize += messize;
	offset = 0;
	to_fill = SHA512_BLOCK_SIZE - ctx->bufsize;

	if (ctx->bufsize > 0) {
		if (messize >= to_fill) {
			ft_memcpy(ctx->buf + ctx->bufsize, mes, to_fill);
			word = (t_sha512_word *)ctx->buf;
			__sha512_rotate_hash((t_sha512_word *)ctx->var, word);
			__sha512_update_hash((t_sha512_word *)ctx->var, (t_sha512_word *)ctx->hash);
			offset += to_fill;
			ctx->bufsize = 0;
		}
		else {
			ft_memcpy(ctx->buf + ctx->bufsize, mes, messize);
			ctx->bufsize += messize;
			return ;
		}
	}
	while (messize - offset >= SHA512_BLOCK_SIZE) {
		word = (t_sha512_word *)(mes + offset);
		__sha512_rotate_hash((t_sha512_word *)ctx->var, word);
		__sha512_update_hash((t_sha512_word *)ctx->var, (t_sha512_word *)ctx->hash);
		offset += SHA512_BLOCK_SIZE;
	}
	ft_memcpy(ctx->buf, mes + offset, messize - offset);
	ctx->bufsize = messize - offset;
}

void	sha512_final(t_digest_ctx *ctx)
{
	uint8_t		pbuf[SHA512_BLOCK_SIZE * 2];
	size_t		pbsize, offset;
	uint128_t	msize_nbits;

	if (NULL == ctx) {
		return ;
	}
	msize_nbits = ft_uint_bswap128((*(uint128_t *)ctx->messize) * 8);

	pbsize = (ctx->bufsize < 112) ? SHA512_BLOCK_SIZE : 2 * SHA512_BLOCK_SIZE;
	offset = 0;

	ft_memcpy(pbuf + offset, ctx->buf, ctx->bufsize);
	offset += ctx->bufsize;
	ctx->bufsize = 0;

	pbuf[offset] = 0x80;
	offset++;

	ft_bzero(pbuf + offset, pbsize - offset - sizeof(msize_nbits));
	ft_memcpy(pbuf + pbsize - sizeof(msize_nbits), &msize_nbits, sizeof(msize_nbits));

	sha512_update(ctx, pbuf, pbsize);

# if BYTE_ORDER == LITTLE_ENDIAN
	__swap_bytes_64((t_sha512_word *)ctx->hash, 8);
# endif
}

static void	__sha512_update_sched(const t_sha512_word *word)
{
	int	i;

	i = 0;
	while (i < 16) {
# if BYTE_ORDER == BIG_ENDIAN
		__sha512_sched[i] = word[i];
# else
		__sha512_sched[i] = ft_uint_bswap64(word[i]);
# endif
		i++;
	}
	while (i < 80) {
		__sha512_sched[i] = S3(__sha512_sched[i-2]) + __sha512_sched[i-7] + S2(__sha512_sched[i-15]) + __sha512_sched[i-16];
		i++;
	}
}

static void	__sha512_rotate(t_sha512_word *var, t_sha512_word ix)
{
	t_sha512_word	t1;
	t_sha512_word	t2;

	t1 = var[7] + E3(var[4]) + CH(var[4], var[5], var[6]) + SHA512_K[ix] + __sha512_sched[ix];
	t2 = E2(var[0]) + MAJ(var[0], var[1], var[2]);
	var[7] = var[6];
	var[6] = var[5];
	var[5] = var[4];
	var[4] = var[3] + t1;
	var[3] = var[2];
	var[2] = var[1];
	var[1] = var[0];
	var[0] = t1 + t2;
}

static void	__sha512_rotate_hash(t_sha512_word *var, const t_sha512_word *word)
{
	t_sha512_word	ix;

	__sha512_update_sched(word);
	ix = 0;
	while (ix < 80) {
		__sha512_rotate(var, ix++);
	}
}

static void	__sha512_update_hash(t_sha512_word *var, t_sha512_word *hash)
{
	hash[0] = var[0] + hash[0];
	hash[1] = var[1] + hash[1];
	hash[2] = var[2] + hash[2];
	hash[3] = var[3] + hash[3];
	hash[4] = var[4] + hash[4];
	hash[5] = var[5] + hash[5];
	hash[6] = var[6] + hash[6];
	hash[7] = var[7] + hash[7];
	var[0] = hash[0];
	var[1] = hash[1];
	var[2] = hash[2];
	var[3] = hash[3];
	var[4] = hash[4];
	var[5] = hash[5];
	var[6] = hash[6];
	var[7] = hash[7];
}

static const t_sha512_word	SHA512_224_INIT_VECT[] = {
	0x8c3d37c819544da2, 0x73e1996689dcd4d6,
	0x1dfab7ae32ff9c82, 0x679dd514582f9fcf,
	0x0f6d2b697bd44da8, 0x77e36f7304c48942,
	0x3f9d85a86a1d36c8, 0x1112e6ad91d692a1
};

void	sha512_224_init(t_digest_ctx *ctx)
{
	ft_bzero(ctx, sizeof(t_digest_ctx));
	ft_memcpy(ctx->var, SHA512_224_INIT_VECT, sizeof(SHA512_224_INIT_VECT));
	ft_memcpy(ctx->hash, SHA512_224_INIT_VECT, sizeof(SHA512_224_INIT_VECT));
	ctx->blocksize = SHA512_BLOCK_SIZE;
	ctx->hashsize = SHA512_224_HASH_SIZE;
}

void	sha512_224_update(t_digest_ctx *ctx, const unsigned char *mes, size_t messize)
{
	sha512_update(ctx, mes, messize);
}

void	sha512_224_final(t_digest_ctx *ctx)
{
	sha512_final(ctx);
}


static const t_sha512_word	SHA512_256_INIT_VECT[] = {
	0x22312194fc2bf72c, 0x9f555fa3c84c64c2,
	0x2393b86b6f53b151, 0x963877195940eabd,
	0x96283ee2a88effe3, 0xbe5e1e2553863992,
	0x2b0199fc2c85b8aa, 0x0eb72ddc81c52ca2
};

void	sha512_256_init(t_digest_ctx *ctx)
{
	ft_bzero(ctx, sizeof(t_digest_ctx));
	ft_memcpy(ctx->var, SHA512_256_INIT_VECT, sizeof(SHA512_256_INIT_VECT));
	ft_memcpy(ctx->hash, SHA512_256_INIT_VECT, sizeof(SHA512_256_INIT_VECT));
	ctx->blocksize = SHA512_BLOCK_SIZE;
	ctx->hashsize = SHA512_256_HASH_SIZE;
}

void	sha512_256_update(t_digest_ctx *ctx, const unsigned char *mes, size_t messize)
{
	sha512_update(ctx, mes, messize);
}

void	sha512_256_final(t_digest_ctx *ctx)
{
	sha512_final(ctx);
}

static void __swap_bytes_32(uint32_t *arr, size_t size)
{
	size_t ix = 0;
	while (ix < size) {
		arr[ix] = ft_uint_bswap32(arr[ix]);
		ix++;
	}
}

static void __swap_bytes_64(t_sha512_word *arr, size_t size)
{
	size_t ix = 0;
	while (ix < size) {
		arr[ix] = ft_uint_bswap64(arr[ix]);
		ix++;
	}
}

typedef void (__t_func_update)(t_digest_ctx *ctx, const uint8_t *mesblock);
typedef void (__t_func_final)(t_digest_ctx *ctx, const uint8_t *mesblock, size_t messsize);

static t_transform_result __transform_update(void *vctx, __t_func_update f_update, size_t blocksize,
	const void *in, size_t insize, void *out, size_t outsize);

static t_transform_result __transform_final(void *vctx, __t_func_update f_update, __t_func_final f_final, size_t blocksize,
	const void *in, size_t insize, void *out, size_t outsize);

t_transform_result md5_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running MD5 transform update");
	return (__transform_update(vctx, md5_update_block, MD5_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result md5_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running MD5 transform final");
	return (__transform_final(vctx, md5_update_block, md5_final_block, MD5_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha1_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA1 transform update");
	return (__transform_update(vctx, sha1_update_block, SHA1_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha1_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA1 transform final");
	return (__transform_final(vctx, sha1_update_block, sha1_final_block, SHA1_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha224_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA224 transform update");
	return (__transform_update(vctx, sha256_update_block, SHA256_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha224_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA256 transform final");
	return (__transform_final(vctx, sha256_update_block, sha256_final_block, SHA256_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha256_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA256 transform update");
	return (__transform_update(vctx, sha256_update_block, SHA256_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha256_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA256 transform final");
	return (__transform_final(vctx, sha256_update_block, sha256_final_block, SHA256_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha384_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA384 transform update");
	return (__transform_update(vctx, sha512_update_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha384_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA384 transform final");
	return (__transform_final(vctx, sha512_update_block, sha512_final_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha512_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA512 transform update");
	return (__transform_update(vctx, sha512_update_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha512_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA512 transform final");
	return (__transform_final(vctx, sha512_update_block, sha512_final_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha512_224_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA512/224 transform update");
	return (__transform_update(vctx, sha512_update_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha512_224_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA512/224 transform final");
	return (__transform_final(vctx, sha512_update_block, sha512_final_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha512_256_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA512/256 transform update");
	return (__transform_update(vctx, sha512_update_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}
t_transform_result sha512_256_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running SHA512/256 transform final");
	return (__transform_final(vctx, sha512_update_block, sha512_final_block, SHA512_BLOCK_SIZE, in, insize, out, outsize));
}

static t_transform_result __transform_update(void *vctx, __t_func_update f_update, size_t blocksize,
	const void *in, size_t insize, void *out, size_t outsize)
{
	t_digest_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "already done");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *mesin = in;
	const uint8_t *mesout = out;
	size_t i = 0;

	if (insize < blocksize) {
		SSL_LOG(TRACE, "need input");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_NEED_INPUT};
	}
	if (outsize < blocksize) {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_NEED_OUTPUT};
	}
	while (i+blocksize <= insize && i+blocksize <= outsize) {
		f_update(ctx, mesin + i);
		ft_memcpy((uint8_t *)mesout + i, mesin + i, blocksize);
		i += blocksize;
	}
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_OK};
}

static t_transform_result __transform_final(void *vctx, __t_func_update f_update, __t_func_final f_final, size_t blocksize,
	const void *in, size_t insize, void *out, size_t outsize)
{
	t_digest_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "already done");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *mesin = in;
	const uint8_t *mesout = out;
	size_t i = 0;

	while (i+blocksize <= insize && i+blocksize <= outsize) {
		f_update(ctx, mesin + i);
		ft_memcpy((uint8_t *)mesout + i, mesin + i, blocksize);
		i += blocksize;
	}
	if (insize <= outsize) {
		f_final(ctx, mesin + i, insize - i);
		ft_memcpy((uint8_t *)mesout + i, mesin + i, insize - i);
		i = insize;
		ctx->done = 1;
		SSL_LOG(TRACE, "processing is complete");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_DONE};
	} else {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_NEED_OUTPUT};
	}
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = i, .status = TRANSFORM_OK};
}
