#include <common.h>
#include <logger.h>
#include <hash.h>
#include <libft.h>

static t_sha512_word	sched[80];

static const t_sha512_word	K[] = {
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

static void	__update_sched(const t_sha512_word *word);
static void	__rotate(t_sha512_word *var, t_sha512_word ix);
static void	__rotate_hash(t_sha512_word *var, const t_sha512_word *word);
static void	__update_hash(t_sha512_word *var, t_sha512_word *hash);

static void __swap_bytes_64(t_sha512_word *arr, size_t size);

void	sha512_update_block(t_hash *ctx, const uint8_t mesblock[SHA512_BLOCK_SIZE])
{
	*(uint128_t *)ctx->messize += SHA512_BLOCK_SIZE;
	__rotate_hash((t_sha512_word *)ctx->var, (const t_sha512_word *)mesblock);
	__update_hash((t_sha512_word *)ctx->var, (t_sha512_word *)ctx->hash);
}

void	sha512_final_block(t_hash *ctx, const uint8_t *mesblock, size_t messize)
{
	*(uint128_t *)ctx->messize += messize;
	uint128_t messize_bit_count = ft_uint_bswap128((*(uint128_t *)ctx->messize) * 8);

	if (messize < 112) {
		uint8_t block[SHA512_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + 112, (uint8_t *)&messize_bit_count, 8);

		__rotate_hash((t_sha512_word *)ctx->var, (const t_sha512_word *)block);
		__update_hash((t_sha512_word *)ctx->var, (t_sha512_word *)ctx->hash);
	}
	else {
		uint8_t block[2*SHA512_BLOCK_SIZE] = {0};
		ft_memcpy(block, mesblock, messize);
		block[messize] = 0x80;
		ft_memcpy(block + SHA512_BLOCK_SIZE+112, (uint8_t *)&messize_bit_count, 8);

		__rotate_hash((t_sha512_word *)ctx->var, (const t_sha512_word *)block);
		__update_hash((t_sha512_word *)ctx->var, (t_sha512_word *)ctx->hash);

		__rotate_hash((t_sha512_word *)ctx->var, (const t_sha512_word *)(block + SHA512_BLOCK_SIZE));
		__update_hash((t_sha512_word *)ctx->var, (t_sha512_word *)ctx->hash);
	}
# if BYTE_ORDER == LITTLE_ENDIAN
	__swap_bytes_64((t_sha512_word *)ctx->hash, SHA512_HASH_LEN);
# endif
}

void	hash_sha512_update_stream(t_hash *ctx, t_iodes *iodes)
{
	char	buf[1024 * SHA512_BLOCK_SIZE];
	size_t	rbytes;

	if (NULL == ctx || NULL == iodes) {
		return ;
	}
	while ((rbytes = io_read(iodes, buf, sizeof(buf))) > 0) {
		hash_sha512_update(ctx, (uint8_t *)buf, rbytes);
	}
}

void	hash_sha512_update(t_hash *ctx, const unsigned char *mes, size_t messize)
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
			__rotate_hash((t_sha512_word *)ctx->var, word);
			__update_hash((t_sha512_word *)ctx->var, (t_sha512_word *)ctx->hash);
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
		__rotate_hash((t_sha512_word *)ctx->var, word);
		__update_hash((t_sha512_word *)ctx->var, (t_sha512_word *)ctx->hash);
		offset += SHA512_BLOCK_SIZE;
	}
	ft_memcpy(ctx->buf, mes + offset, messize - offset);
	ctx->bufsize = messize - offset;
}
static void	__update_sched(const t_sha512_word *word)
{
	int	i;

	i = 0;
	while (i < 16) {
# if BYTE_ORDER == BIG_ENDIAN
		sched[i] = word[i];
# else
		sched[i] = ft_uint_bswap64(word[i]);
# endif
		i++;
	}
	while (i < 80) {
		sched[i] = S3(sched[i-2]) + sched[i-7] + S2(sched[i-15]) + sched[i-16];
		i++;
	}
}

static void	__rotate(t_sha512_word *var, t_sha512_word ix)
{
	t_sha512_word	t1;
	t_sha512_word	t2;

	t1 = var[7] + E3(var[4]) + CH(var[4], var[5], var[6]) + K[ix] + sched[ix];
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

static void	__rotate_hash(t_sha512_word *var, const t_sha512_word *word)
{
	t_sha512_word	ix;

	__update_sched(word);
	ix = 0;
	while (ix < 80) {
		__rotate(var, ix++);
	}
}

static void	__update_hash(t_sha512_word *var, t_sha512_word *hash)
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

static void __swap_bytes_64(t_sha512_word *arr, size_t size)
{
	size_t ix = 0;
	while (ix < size) {
		arr[ix] = ft_uint_bswap64(arr[ix]);
		ix++;
	}
}
