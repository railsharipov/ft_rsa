#include <common.h>
#include <logger.h>
#include <hash.h>

void	hash_sha512_224_update_stream(t_hash *ctx, t_iodes *iodes)
{
	hash_sha512_update_stream(ctx, iodes);
}

void	hash_sha512_224_update(t_hash *ctx, const unsigned char *mes, size_t messize)
{
	hash_sha512_update(ctx, mes, messize);
}
