#include <common.h>
#include <logger.h>
#include <digest.h>
#include <libft.h>

#include "__digest.h"

static t_sha256_word	sched[64];

static const t_sha256_word	K[] = {
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

static const t_sha256_word	HASH_INIT_VECT[] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

void	sha256_init(t_hash *ctx)
{
	ft_bzero(ctx, sizeof(t_hash));
	ft_memcpy(ctx->var, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ft_memcpy(ctx->hash, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ctx->blocksize = SHA256_BLOCK_SIZE;
	ctx->hashsize = SHA256_HASH_SIZE;
}

static void	__update_sched(const t_sha256_word *word);
static void	__rotate(t_sha256_word *var, t_sha256_word ix);
static void	__rotate_hash(t_sha256_word *var, const t_sha256_word *word);
static void	__update_hash(t_sha256_word *var, t_sha256_word *hash);

static void __swap_bytes_32(uint32_t *arr, size_t size);

void	sha256_update_block(t_hash *ctx, const uint8_t mesblock[SHA256_BLOCK_SIZE])
{
	*(uint64_t *)ctx->messize += SHA256_BLOCK_SIZE;
	__rotate_hash((t_sha256_word *)ctx->var, (const t_sha256_word *)mesblock);
	__update_hash((t_sha256_word *)ctx->var, (t_sha256_word *)ctx->hash);
}

void	sha256_final_block(t_hash *ctx, const uint8_t *mesblock, size_t messize)
{
	*(uint64_t *)ctx->messize += messize;
	uint64_t messize_bit_count = ft_uint_bswap64(*(uint64_t *)ctx->messize * 8);

	if (messize < 56) {
		uint8_t block[SHA256_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + 56, (uint8_t *)&messize_bit_count, 8);

		__rotate_hash((t_sha256_word *)ctx->var, (const t_sha256_word *)block);
		__update_hash((t_sha256_word *)ctx->var, (t_sha256_word *)ctx->hash);
	}
	else {
		uint8_t block[2*SHA256_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + SHA256_BLOCK_SIZE+56, (uint8_t *)&messize_bit_count, 8);

		__rotate_hash((t_sha256_word *)ctx->var, (const t_sha256_word *)block);
		__update_hash((t_sha256_word *)ctx->var, (t_sha256_word *)ctx->hash);

		__rotate_hash((t_sha256_word *)ctx->var, (const t_sha256_word *)(block + SHA256_BLOCK_SIZE));
		__update_hash((t_sha256_word *)ctx->var, (t_sha256_word *)ctx->hash);
	}
# if BYTE_ORDER == LITTLE_ENDIAN
	__swap_bytes_32((uint32_t *)ctx->hash, SHA256_HASH_LEN);
# endif
}

void	sha256_update(t_hash *ctx, const unsigned char *mes, size_t messize)
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
			__rotate_hash((t_sha256_word *)ctx->var, word);
			__update_hash((t_sha256_word *)ctx->var, (t_sha256_word *)ctx->hash);
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
		__rotate_hash((t_sha256_word *)ctx->var, word);
		__update_hash((t_sha256_word *)ctx->var, (t_sha256_word *)ctx->hash);
		offset += SHA256_BLOCK_SIZE;
	}
	ft_memcpy(ctx->buf, mes + offset, messize - offset);
	ctx->bufsize = messize - offset;
}

void	sha256_final(t_hash *ctx)
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

static void	__update_sched(const t_sha256_word *word)
{
	int	i;

	i = 0;
	while (i < 16) {
# if BYTE_ORDER == BIG_ENDIAN
		sched[i] = word[i];
# else
		sched[i] = ft_uint_bswap32(word[i]);
# endif
		i++;
	}
	while (i < 64) {
		sched[i] = S1(sched[i-2]) + sched[i-7] + S0(sched[i-15]) + sched[i-16];
		i++;
	}
}

static void	__rotate(t_sha256_word *var, t_sha256_word ix)
{
	t_sha256_word	t1;
	t_sha256_word	t2;

	t1 = var[7] + E1(var[4]) + CH(var[4], var[5], var[6]) + K[ix] + sched[ix];
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

static void	__rotate_hash(t_sha256_word *var, const t_sha256_word *word)
{
	t_sha256_word	ix;

	__update_sched(word);
	ix = 0;
	while (ix < 64) {
		__rotate(var, ix++);
	}
}

static void	__update_hash(t_sha256_word *var, t_sha256_word *hash)
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

static void __swap_bytes_32(uint32_t *arr, size_t size)
{
	size_t ix = 0;
	while (ix < size) {
		arr[ix] = ft_uint_bswap32(arr[ix]);
		ix++;
	}
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
