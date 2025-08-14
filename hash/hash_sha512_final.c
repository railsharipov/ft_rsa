#include <common.h>
#include <hash.h>
#include <libft/bytes.h>

static void	__swap_bytes_64(t_sha512_word *arr, size_t size);

void	hash_sha512_final(t_hash *ctx)
{
	uint128_t	msize_nbits;
	uint8_t		pad[SHA512_BLOCK_SIZE];
	size_t		pad_len;

	if (NULL == ctx) {
		return ;
	}
	pad_len = (ctx->bufsize < 112) ? (112 - ctx->bufsize) : (SHA512_BLOCK_SIZE + 112 - ctx->bufsize);
	msize_nbits = ft_uint_bswap128(*(uint128_t *)ctx->messize * 8);
	ft_bzero(pad, pad_len);
	pad[0] = 0x80;
	hash_sha512_update(ctx, pad, pad_len);
	hash_sha512_update(ctx, (uint8_t *)&msize_nbits, sizeof(msize_nbits));

# if BYTE_ORDER == LITTLE_ENDIAN
	__swap_bytes_64((t_sha512_word *)ctx->hash, SHA512_HASH_LEN);
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