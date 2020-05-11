#include <ft_ssl.h>
#include <ssl_hash.h>

static const t_sha384_word	HASH_INIT_VECT[] = {
	0xcbbb9d5dc1059ed8, 0x629a292a367cd507,
	0x9159015a3070dd17, 0x152fecd8f70e5939,
	0x67332667ffc00b31, 0x8eb44a8768581511,
	0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4
};

static t_sha384_word		var[SHA512_HASH_LEN];
static t_sha384_word		hash[SHA512_HASH_LEN];
static t_uint128			msize;

static t_hash	sha384 = {
	.var = &var,
	.hash = &hash,
	.size = SHA384_HASH_LEN * sizeof(t_sha384_word),
	.msize = &msize
};

t_hash	*hash_sha384_init(void)
{
	ft_memcpy(sha384.var, HASH_INIT_VECT, sizeof(hash));
	ft_memcpy(sha384.hash, HASH_INIT_VECT, sizeof(hash));
	msize = 0;

	return (&sha384);
}
