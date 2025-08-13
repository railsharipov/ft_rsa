#include <common.h>
#include <hash.h>
#include <libft/bytes.h>

static t_sha1_word		sched[80];

static const t_sha1_word	K[] = {
	[0 ... 19] = 0x5a827999,
	[20 ... 39] = 0x6ed9eba1,
	[40 ... 59] = 0x8f1bbcdc,
	[60 ... 79] = 0xca62c1d6
};

static void	__update_sched(t_sha1_word *word);
static void	__rotate(t_sha1_word *var, t_sha1_word *t1, t_sha1_word *t2, t_sha1_word ix);
static void	__rotate_hash(t_sha1_word *var, t_sha1_word *word);
static void	__update_hash(t_sha1_word *var, t_sha1_word *hash);

void	hash_sha1_update(t_hash *ctx, const unsigned char *mes, size_t messize)
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
			__rotate_hash((t_sha1_word *)ctx->var, word);
			__update_hash((t_sha1_word *)ctx->var, (t_sha1_word *)ctx->hash);
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
		__rotate_hash((t_sha1_word *)ctx->var, word);
		__update_hash((t_sha1_word *)ctx->var, (t_sha1_word *)ctx->hash);
		offset += SHA1_BLOCK_SIZE;
	}
	ft_memcpy(ctx->buf, mes + offset, messize - offset);
	ctx->bufsize = messize - offset;
}

static void	__update_sched(t_sha1_word *word)
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
	while (i < 80) {
		sched[i] = LROT((sched[i-3]^sched[i-8]^sched[i-14]^sched[i-16]), 1);
		i++;
	}
}

static void	__rotate(t_sha1_word *var, t_sha1_word *t1, t_sha1_word *t2, t_sha1_word ix)
{
	*t2 = LROT(var[0], 5) + *t1 + var[4] + K[ix] + sched[ix];

	var[4] = var[3];
	var[3] = var[2];
	var[2] = LROT(var[1], 30);
	var[1] = var[0];
	var[0] = *t2;
}

static void	__rotate_hash(t_sha1_word *var, t_sha1_word *word)
{
	t_sha1_word	t1;
	t_sha1_word	t2;
	t_sha1_word	ix;

	__update_sched(word);
	ix = 0;
	while (ix < 20) {
		t1 = CH(var[1], var[2], var[3]);
		__rotate(var, &t1, &t2, ix++);
	}
	while (ix < 40) {
		t1 = H0(var[1], var[2], var[3]);
		__rotate(var, &t1, &t2, ix++);
	}
	while (ix < 60) {
		t1 = MAJ(var[1], var[2], var[3]);
		__rotate(var, &t1, &t2, ix++);
	}
	while (ix < 80) {
		t1 = H0(var[1], var[2], var[3]);
		__rotate(var, &t1, &t2, ix++);
	}
}

static void	__update_hash(t_sha1_word *var, t_sha1_word *hash)
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
