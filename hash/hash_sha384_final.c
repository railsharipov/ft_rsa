#include <common.h>
#include <logger.h>
#include <hash.h>

void	hash_sha384_final(t_hash *sha384)
{
	hash_sha512_final(sha384);
}
