#include <ft_ssl.h>
#include <ssl_hash.h>

static const t_sha224_word	HASH_INIT_VECT[] = {
	0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
	0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
};

static t_sha224_word		var[SHA256_HASH_LEN];
static t_sha224_word		hash[SHA256_HASH_LEN];
static uint64_t				msize;

static t_hash	sha224 = {
	.var = &var,
	.hash = &hash,
	.size = SHA224_HASH_LEN * sizeof(t_sha224_word),
	.msize= &msize
};

t_hash	*hash_sha224_init(void)
{
	ft_memcpy(sha224.var, HASH_INIT_VECT, sizeof(hash));
	ft_memcpy(sha224.hash, HASH_INIT_VECT, sizeof(hash));
	msize = 0;

	return (&sha224);
}
