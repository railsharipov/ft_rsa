#include <common.h>
#include <logger.h>
#include <hash.h>
#include <libft.h>

static void __swap_bytes_64(t_sha512_word *arr, size_t size);

void	hash_sha512_final(t_hash *ctx)
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

	hash_sha512_update(ctx, pbuf, pbsize);

# if BYTE_ORDER == LITTLE_ENDIAN
	__swap_bytes_64((t_sha512_word *)ctx->hash, 8);
# endif
}

static void __swap_bytes_64(t_sha512_word *arr, size_t size)
{
	size_t ix;

	ix = 0;
	while (ix < size) {
		arr[ix] = ft_uint_bswap64(arr[ix]);
		ix++;
	}
}
