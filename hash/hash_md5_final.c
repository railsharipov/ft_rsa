#include <common.h>
#include <logger.h>
#include <hash.h>

void	hash_md5_final(t_hash *ctx)
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
	hash_md5_update(ctx, pad, pad_len);
	hash_md5_update(ctx, (uint8_t *)&msize_nbits, sizeof(msize_nbits));
}
