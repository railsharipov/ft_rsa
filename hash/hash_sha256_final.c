#include <common.h>
#include <hash.h>
#include <libft/bytes.h>

static void	__swap_bytes_32(uint32_t *arr, size_t size);

void	hash_sha256_final(t_hash *ctx)
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
	hash_sha256_update(ctx, pad, pad_len);
	hash_sha256_update(ctx, (uint8_t *)&msize_nbits, sizeof(msize_nbits));

# if BYTE_ORDER == LITTLE_ENDIAN
	__swap_bytes_32((uint32_t *)ctx->hash, SHA256_HASH_LEN);
# endif
}

static void __swap_bytes_32(uint32_t *arr, size_t size)
{
	size_t ix;

	ix = 0;
	while (ix < size) {
		arr[ix] = ft_uint_bswap32(arr[ix]);
		ix++;
	}
}
