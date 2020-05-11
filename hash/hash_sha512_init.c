#include <ft_ssl.h>
#include <ssl_hash.h>

static const t_sha512_word	HASH_INIT_VECT[] = {
	0x6a09e667f3bcc908, 0xbb67ae8584caa73b,
	0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
	0x510e527fade682d1, 0x9b05688c2b3e6c1f,
	0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
};

static t_sha512_word		var[SHA512_HASH_LEN];
static t_sha512_word		hash[SHA512_HASH_LEN];
static t_uint128			msize;

static t_hash	sha512 = {
	.var = &var,
	.hash = &hash,
	.size = SHA512_HASH_LEN * sizeof(t_sha512_word),
	.msize = &msize
};

t_hash	*hash_sha512_init(void)
{
	ft_memcpy(sha512.var, HASH_INIT_VECT, sizeof(hash));
	ft_memcpy(sha512.hash, HASH_INIT_VECT, sizeof(hash));
	msize = 0;

	return (&sha512);
}
